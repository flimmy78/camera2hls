#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <kugou/httpproxy/accessserver/bean/Encoding.h>
#include <kugou/base/facility/log.h>
#include <kugou/base/facility/Hash.h>

#include <fstream>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/Statement.h>
#include <Poco/Hash.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _MSC_VER
std::string Encoding_File = "config\\encoding.json";
#elif defined __GNUC__
std::string Encoding_File = "config/encoding.json";
#endif

using namespace boost::filesystem;
using namespace kugou::httpproxy::accessserver::system;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{

AccessPointManager::AccessPointManager(void)
    : m_mysqlAccesspointMap(new ACCESSPOINT_MAP)
{

}


AccessPointManager::~AccessPointManager(void)
{
}

bool AccessPointManager::loadFile(std::string& text, std::string& fileName)
{
    std::ifstream is(fileName.c_str());
    text.erase();
    if(is.bad())
    {
        LOG_E("Failed to open  [" << fileName << "] .");
        return false;
    }

    text.reserve(static_cast<std::string::size_type>(is.rdbuf()->in_avail()));
    char c;
    while(is.get(c))
    {
        if(text.capacity() == text.size())
            text.reserve(text.capacity() * 3);
        text.append(1, c);
    }
    is.close();

    if(text.empty())
    {
        LOG_E("File [" << fileName << "] is empty.");
        return false;
    }
    return true;
}

void AccessPointManager::recursionLoadFiles(std::string path, FILES_VECTOR& v)
{
    if(exists(path))
    {
        directory_iterator itBegin(path);
        directory_iterator itEnd;
        for(;itBegin != itEnd; itBegin++)
        {
            if (is_directory(*itBegin))
            {
                recursionLoadFiles(itBegin->path().string(), v);
            }
            else
            {
                v.push_back(itBegin->path().string());
            }
        }
    }
}

bool AccessPointManager::loadIP(const std::string& geoIPPath, const std::string& specialIPPath)
{
    bool ret = false;
    // 定义临时接入点map;
    boost::shared_ptr<ACCESSPOINT_MAP> tempAccesspointMap(new ACCESSPOINT_MAP);

    // 定义临时special radixTree
    boost::shared_ptr<RadixTree> tempSpecialRadixTree(new RadixTree);

    // 定义临时Accesspoint radixTree
    boost::shared_ptr<RadixTree> tempAccesspointRadixTree(new RadixTree);

    ret = this->loadGeoIP(true, specialIPPath, tempSpecialRadixTree, tempAccesspointMap);

    ret = this->loadGeoIP(false, geoIPPath, tempAccesspointRadixTree, tempAccesspointMap);

    ret = flashAccessPointDatabase(tempSpecialRadixTree, tempAccesspointRadixTree, tempAccesspointMap);

    return ret;
}

bool AccessPointManager::loadGeoIP(bool isSpecial, std::string path, 
    boost::shared_ptr<RadixTree>& tempRadixTree, boost::shared_ptr<ACCESSPOINT_MAP>& tempAccesspointMap)
{
    // 判断要扫描的目录是否存在
    if(!exists(path))
    {
        LOG_E("scan path [" << path << "] is not exist.");
        return false;
    }

    // 把path目录下的所有文件读找到，并存入m_allFileVector
    FILES_VECTOR tempAllFileVector;
    recursionLoadFiles(path, tempAllFileVector);

    // 把所有文件的IP库读取出来
    for(FILES_VECTOR::iterator itBegin = tempAllFileVector.begin(); itBegin != tempAllFileVector.end(); itBegin++)
    {
        LOG_I("Load geoip file [" << *itBegin << "].");
        std::string text;
        if(!loadFile(text, *itBegin))
        {
            // 读取失败
            LOG_E("Load geoip file [" << *itBegin << "] failed.");
            return false;
        }

        // 通过正则表达式，取出这个接入点名字和所有IP段
        boost::regex re("acl (.*)\\{(.*)\\};(.*)");
        boost::cmatch matches;
        if (boost::regex_match(text.c_str(), matches, re))
        {
            AccessPoint ap;
            ap.groupId1 = ap.groupId2 = -1;
            for (int i = 1; i < matches.size(); i++)
            {
                std::string match(matches[i].first, matches[i].second);
            }
            // matches[1] 为接入点名字
            // matches[2] 为所有IP段

            // 取出“所属运营商用”和“所属地区”
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            {
                std::string areaOperator(matches[1].first, matches[1].second);
                boost::char_separator<char> sep("_");
                tokenizer tokens(areaOperator, sep);

                for (tokenizer::iterator it = tokens.begin();it != tokens.end(); ++it)  
                {
                    if(ap.carrier.empty())
                    {
                        ap.carrier = *it;
                    }
                    else if((*it).find("addr", 0, 4) && ap.area.empty())
                    {
                        ap.area = *it;
                    }
                    else
                    {
                        break;
                    }
                }
                
                //接入点ID以area+carrier字符串的hash值再模1000
                char hashStr[128];
                strcpy(hashStr, std::string(ap.area + ap.carrier).c_str());
                ap.id = kugou::base::facility::Hash::BKDRHash(hashStr)%10000;
                ap.id += isSpecial ? 10000 : 0;
            }

            // 如果hash 值冲突，返回失败
            if(tempAccesspointMap->find(ap.id) != tempAccesspointMap->end())
            {
                LOG_E("Hash map  conflict.");
                return false;
            }

            // 接入点信息放入MAP
            tempAccesspointMap->insert(std::make_pair(ap.id, ap));

            // 取出每个IP段，并放入RadixTree
            {
                std::string ip(matches[2].first, matches[2].second);
                boost::erase_all(ip,"\n");
                boost::erase_all(ip,"\r");
                boost::char_separator<char> sep(";/");
                tokenizer tokens(ip, sep);
                for (tokenizer::iterator it = tokens.begin();it != tokens.end(); ++it)
                {
                    // 取出IP地址，再转成unsigned int
                    unsigned int addr = ntohl(inet_addr((*it).c_str()));
                    ++it;

                    // 取数掩码数，转成unsigned int
                    unsigned int mask = 0;
                    {
                        unsigned int bit = 0x80000000;
                        int count = boost::lexical_cast<int> (*it);
                        for(int i = 0; i < count; i++)
                        {
                            mask |= bit;
                            bit >>= 1;
                        }
                    }
                    tempRadixTree->insertNode(addr , mask , ap.id);
                }
            }
        }
    }

    return true;
}

bool AccessPointManager::flashAccessPointDatabase(boost::shared_ptr<RadixTree>& tempSpecialRadixTree,
        boost::shared_ptr<RadixTree>& tempAccesspointRadixTree, boost::shared_ptr<ACCESSPOINT_MAP>& tempAccesspointMap)
{
    bool ret = true;

    // 定义临时mysql接入点map;
    boost::shared_ptr<ACCESSPOINT_MAP> tempMysqlAccesspointMap(new ACCESSPOINT_MAP);

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

        // 使用utf8字符集
        {
            Poco::Data::Statement setUTF8(session);
            setUTF8 << "SET NAMES utf8";
            setUTF8.execute();
        }

        // 获取mysql中的AccessPoint，并保存到map
        {
            AccessPoint ap;
            Poco::Data::Statement select(session);
            select << "SELECT pointId, area, carrier, IDCId1, IDCId2 FROM t_AccessPoint" , 
                Poco::Data::into(ap.id),
                Poco::Data::into(ap.area),
                Poco::Data::into(ap.carrier),
                Poco::Data::into(ap.groupId1),
                Poco::Data::into(ap.groupId2),
                Poco::Data::range(0, 1);    //  iterate over result set one row at a time
            while (!select.done())
            {
                select.execute();
                tempMysqlAccesspointMap->insert(std::make_pair(ap.id, ap));
            }
        }

        // 如果没有默认接入点，则插入一个默认接入点
        {
            m_defaultAccessPoint.groupId1 = -1;
            m_defaultAccessPoint.groupId2 = -1;
            m_defaultAccessPoint.area = "default";
            m_defaultAccessPoint.carrier = "default";
            ACCESSPOINT_MAP::iterator it = tempMysqlAccesspointMap->find(0);
            if(it == tempMysqlAccesspointMap->end())
            {
                Poco::Data::Statement insert(session);
                insert << "INSERT INTO t_AccessPoint VALUES(?, ?, ?, ?, ?)",
                    Poco::Data::use(0),
                    Poco::Data::use(m_defaultAccessPoint.area),
                    Poco::Data::use(m_defaultAccessPoint.carrier),
                    Poco::Data::use(m_defaultAccessPoint.groupId1),
                    Poco::Data::use(m_defaultAccessPoint.groupId2);
                insert.execute();
            }
            else
            {
                m_defaultAccessPoint = it->second;
            }
        }

        // 查找本地所有接入点，如果mysql中没有，就插入此接入点信息
        ACCESSPOINT_MAP::iterator it = tempAccesspointMap->begin();
        while(it != tempAccesspointMap->end())
        {
            if(tempMysqlAccesspointMap->find(it->first) != tempMysqlAccesspointMap->end())
            {
                tempAccesspointMap->erase(it++); 
            }
            else
            {
                {
                    Poco::Data::Statement insert(session);
                    insert << "INSERT INTO t_AccessPoint VALUES(?, ?, ?, ?, ?)",
                        Poco::Data::use(it->second.id),
                        Poco::Data::use(it->second.area),
                        Poco::Data::use(it->second.carrier),
                        Poco::Data::use(it->second.groupId1),
                        Poco::Data::use(it->second.groupId2);
                    insert.execute();
                }
                tempMysqlAccesspointMap->insert(std::make_pair(it->second.id, it->second));
                ++it;
            }
        }

        // 导入拼音汉字映射表
        {
            kugou::httpproxy::accessserver::bean::Encoding t_Encoding;
            if(!t_Encoding.fromJsonFile(Encoding_File))
            {
                LOG_E("Read encoding list failed.");
                return false;
            }


            std::map<std::string,std::string> encodingMap;
            std::vector<struct kugou::httpproxy::accessserver::bean::EncodingMapObject>::iterator beginIt;            
            for(beginIt = t_Encoding.EncodingMap.begin(); \
                beginIt != t_Encoding.EncodingMap.end(); beginIt++)
            {
                encodingMap.insert(std::make_pair(beginIt->en, beginIt->cn));
            }

            // 修改成汉字
            ACCESSPOINT_MAP::iterator it = tempMysqlAccesspointMap->begin();
            while(it != tempMysqlAccesspointMap->end())
            {
                std::map<std::string,std::string>::iterator findIt = encodingMap.find(it->second.area);
                it->second.area = (findIt != encodingMap.end()) ? findIt->second  : it->second.area;
                
                findIt = encodingMap.find(it->second.carrier);
                it->second.carrier = (findIt != encodingMap.end()) ? findIt->second  : it->second.carrier;
                it++;
            }

            std::map<std::string,std::string>::iterator findIt = encodingMap.find(m_defaultAccessPoint.area);
            m_defaultAccessPoint.area = (findIt != encodingMap.end()) ? findIt->second  : it->second.area;

            findIt = encodingMap.find(m_defaultAccessPoint.carrier);
            m_defaultAccessPoint.carrier = (findIt != encodingMap.end()) ? findIt->second  : it->second.carrier;            
        }

        // 同步数据库成功
        {
            // 对m_mysqlAccesspointMap 加写锁
            WriteLock writeLock(m_mysqlAccesspointMapRadixTreeMutex);

            LOG_I("reset mysqlAccesspoint Map and RadixTree begin");

            m_specialRadixTree = tempSpecialRadixTree;
            m_accesspointRadixTree = tempAccesspointRadixTree;
            m_mysqlAccesspointMap = tempMysqlAccesspointMap;

            LOG_I("reset mysqlAccesspoint Map and RadixTree end");
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

AccessPoint AccessPointManager::getDefaultAccessPoint(void)
{
    return m_defaultAccessPoint;
}

AccessPoint AccessPointManager::getAccessPointNoSpecial(std::string ip)
{
    int id;
    unsigned int addr;
    AccessPoint ap;

    // 对m_mysqlAccesspointMap 加读锁
    ReadLock rdlock(m_mysqlAccesspointMapRadixTreeMutex);

    addr = htonl(inet_addr(ip.c_str()));

    // 查找配置的ip库存
    id = m_accesspointRadixTree->findNode(addr);

    ACCESSPOINT_MAP::iterator it = m_mysqlAccesspointMap->find(id);
    //只要找到了IP对应的接入点，就返回，不管它是否有配置IDC
    if(it != m_mysqlAccesspointMap->end())
    {
        ap = it->second;
    }
    else
    {
        //找不到所属接入点，使用默认接入点
        ap = m_defaultAccessPoint;

        //找不到所属接入点        
        LOG_W("Can not identify the access point of IP Address : [" << ip << "]");
    }
    return ap;
}

AccessPoint AccessPointManager::getAccessPoint(std::string ip)
{
    int id;
    unsigned int addr;
    AccessPoint ap;

    // 对m_mysqlAccesspointMap 加读锁
    ReadLock rdlock(m_mysqlAccesspointMapRadixTreeMutex);

    addr = htonl(inet_addr(ip.c_str()));

    // 先查找特殊ip库，如果不存在，再查找配置的ip库存
    id = m_specialRadixTree->findNode(addr);
    if(NGX_RADIX_NO_VALUE == id)
    {
        // 查找配置的ip库存
        id = m_accesspointRadixTree->findNode(addr);
    }

    ACCESSPOINT_MAP::iterator it = m_mysqlAccesspointMap->find(id);
    if(it != m_mysqlAccesspointMap->end() && (it->second.groupId1 != -1 || it->second.groupId2 != -1))
    {
        ap = it->second;
    }
    else
    {
        //找不到所属接入点，使用默认接入点
        ap = m_defaultAccessPoint;
        
        LOG_W("Can not identify the access point of IP Address : [" << ip << "], use default one");
    }

    return ap;
}

}
}
}
}
