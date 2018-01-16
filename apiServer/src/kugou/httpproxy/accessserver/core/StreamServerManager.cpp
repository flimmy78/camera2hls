#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <kugou/base/facility/log.h>

#include <boost/lexical_cast.hpp>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/Statement.h>
/*
#ifdef _WIN32
#include <winsock2.h>
#else
#endif
*/

using namespace kugou::httpproxy::accessserver::bean;
using namespace kugou::httpproxy::accessserver::system;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{

StreamServerManager::StreamServerManager(void)
    : m_streamServerGroupMap(new STREAMSERVERGROUP_MAP)
{

}

StreamServerManager::~StreamServerManager(void)
{
}

bool StreamServerManager::flashDatabase(void)
{
    bool ret = true;
    Poco::Data::MySQL::Connector::registerConnector();

    try
    {
        // 0. connect & create table
        Poco::Data::Session session(Poco::Data::MySQL::Connector::KEY,
            "host=" + AccessServerConfig::instance().m_host + \
            ";user="+ AccessServerConfig::instance().m_user + \
            ";password=" + AccessServerConfig::instance().m_password + \
            ";db=" +  AccessServerConfig::instance().m_db);

        LOG_I("Connect to mysql host[" << AccessServerConfig::instance().m_host \
              << "]  user[" << AccessServerConfig::instance().m_user \
              << "]  password[" << AccessServerConfig::instance().m_password \
              << "]  db[" << AccessServerConfig::instance().m_db << "].");


        // 临时保存服务器组变量
        boost::shared_ptr<STREAMSERVERGROUP_MAP> tempGroupMap(new STREAMSERVERGROUP_MAP);

        // 使用utf8字符集
        {
            Poco::Data::Statement setUTF8(session);
            setUTF8 << "SET NAMES utf8";
            setUTF8.execute();
        }

        // 查找服务器组，并放入内存
        {
            StreamServerGroup group;
            Poco::Data::Statement select(session);
            select << "SELECT IDC.IDCId, IDC.testServer, IDC.available, A.des, C.des  \
                       FROM t_IDC AS IDC LEFT JOIN t_Area AS A ON A.areaId = IDC.areaId \
                                         LEFT JOIN t_Carrier AS C ON C.carrierId = IDC.carrierId", 
                Poco::Data::into(group.id),
                Poco::Data::into(group.testServer),
                Poco::Data::into(group.available),
                Poco::Data::into(group.area),
                Poco::Data::into(group.carrier),
                Poco::Data::range(0, 1);    //  iterate over result set one row at a time

            while (!select.done())
            {
                select.execute();

                // 每条信息加入map
                tempGroupMap->insert(std::make_pair(group.id, group));
                LOG_I("load IDC id[" << group.id  << "]  area[" << group.area << \
                      "] carrier[" << group.carrier << "] available[" << group.available << "].");
            }
        }

        // 查找服务器，并放入内存
        {
            StreamService service;
            Poco::Data::Statement select(session);
            select << "SELECT ipPublic, IDCId, available FROM t_StreamServer", 
                Poco::Data::into(service.ip),
                Poco::Data::into(service.groupId),
                Poco::Data::into(service.available),
                Poco::Data::range(0, 1);    //  iterate over result set one row at a time

            while (!select.done())
            {
                select.execute();
                // 找到这个服务所属的组
                STREAMSERVERGROUP_MAP::iterator it = tempGroupMap->find(service.groupId);
                if(it != tempGroupMap->end())
                {
                    // 找到所属组，并插入这个组的服务列表
                    it->second.addServer(service);
                    LOG_I("add server to IDC id[" << service.groupId   << "]  ip[" << service.ip << \
                      "] available[" << service.ip << "].");
                }
                else
                {
                    // 在所属组不存在
                }
            }
        }

        // 同步数据库成功
        {
            // 对m_streamServerGroupMap 加写锁
            WriteLock writeLock(m_streamServerGroupMapMutex);

            LOG_I("reset streamServerGroup Map begin");
            m_streamServerGroupMap = tempGroupMap;
            LOG_I("reset streamServerGroup Map end");
        }
    }
    catch (const Poco::Data::MySQL::StatementException &se)
    {
        LOG_E("connect to mysql StatementException [" << se.displayText() << "].");
        ret = false;
    }
    catch (const Poco::Data::MySQL::MySQLException &ce)
    {
        LOG_E("connect to mysql MySQLException [" << ce.displayText() << "].");
        ret = false;
    }
	catch (...)
	{
        LOG_E("connect to mysql Unknow exception.");
		ret = false;
	}

    Poco::Data::MySQL::Connector::unregisterConnector();

    return ret;
}

bool StreamServerManager::getTestServerList(std::vector<NodeListObject>& list)
{
    // 对m_streamServerGroupMap 加读锁
    ReadLock rdlock(m_streamServerGroupMapMutex);

    // 找到所有测试IP返回
    for(STREAMSERVERGROUP_MAP::iterator it = m_streamServerGroupMap->begin(); it != m_streamServerGroupMap->end(); it++)
    {
		if(it->second.available && !it->second.testServer.empty())
        {
            NodeListObject node;
            node.ID = boost::lexical_cast<std::string>(it->second.id);
            node.Addr = it->second.testServer;
            node.Port = "1935";

			//不要显示地区和运营商给用户看,只给结束ID
            node.Carrier = boost::lexical_cast<std::string>(it->second.id);//it->second.carrier; 
            node.Area = "节点";//it->second.area;	

            list.push_back(node);
        }
    }

    return true;
}

bool StreamServerManager::getAddressList(int groupid, std::vector<AddressListObject>& list)
{
    // 对m_streamServerGroupMap 加读锁
    ReadLock rdlock(m_streamServerGroupMapMutex);

    STREAMSERVERGROUP_MAP::iterator it = m_streamServerGroupMap->find(groupid);
    if(it != m_streamServerGroupMap->end())
    {
        // 用轮询的方式给服务地址
        AddressListObject obj;
        if(it->second.getNextServer(obj.Addr))
        {
            obj.Port = "1935";
            list.push_back(obj);
        }
        return true;
    }
    else
    {
    }
    return false;
}

}
}
}
}
