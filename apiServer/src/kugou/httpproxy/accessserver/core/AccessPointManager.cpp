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
    // ������ʱ�����map;
    boost::shared_ptr<ACCESSPOINT_MAP> tempAccesspointMap(new ACCESSPOINT_MAP);

    // ������ʱspecial radixTree
    boost::shared_ptr<RadixTree> tempSpecialRadixTree(new RadixTree);

    // ������ʱAccesspoint radixTree
    boost::shared_ptr<RadixTree> tempAccesspointRadixTree(new RadixTree);

    ret = this->loadGeoIP(true, specialIPPath, tempSpecialRadixTree, tempAccesspointMap);

    ret = this->loadGeoIP(false, geoIPPath, tempAccesspointRadixTree, tempAccesspointMap);

    ret = flashAccessPointDatabase(tempSpecialRadixTree, tempAccesspointRadixTree, tempAccesspointMap);

    return ret;
}

bool AccessPointManager::loadGeoIP(bool isSpecial, std::string path, 
    boost::shared_ptr<RadixTree>& tempRadixTree, boost::shared_ptr<ACCESSPOINT_MAP>& tempAccesspointMap)
{
    // �ж�Ҫɨ���Ŀ¼�Ƿ����
    if(!exists(path))
    {
        LOG_E("scan path [" << path << "] is not exist.");
        return false;
    }

    // ��pathĿ¼�µ������ļ����ҵ���������m_allFileVector
    FILES_VECTOR tempAllFileVector;
    recursionLoadFiles(path, tempAllFileVector);

    // �������ļ���IP���ȡ����
    for(FILES_VECTOR::iterator itBegin = tempAllFileVector.begin(); itBegin != tempAllFileVector.end(); itBegin++)
    {
        LOG_I("Load geoip file [" << *itBegin << "].");
        std::string text;
        if(!loadFile(text, *itBegin))
        {
            // ��ȡʧ��
            LOG_E("Load geoip file [" << *itBegin << "] failed.");
            return false;
        }

        // ͨ��������ʽ��ȡ�������������ֺ�����IP��
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
            // matches[1] Ϊ���������
            // matches[2] Ϊ����IP��

            // ȡ����������Ӫ���á��͡�����������
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
                
                //�����ID��area+carrier�ַ�����hashֵ��ģ1000
                char hashStr[128];
                strcpy(hashStr, std::string(ap.area + ap.carrier).c_str());
                ap.id = kugou::base::facility::Hash::BKDRHash(hashStr)%10000;
                ap.id += isSpecial ? 10000 : 0;
            }

            // ���hash ֵ��ͻ������ʧ��
            if(tempAccesspointMap->find(ap.id) != tempAccesspointMap->end())
            {
                LOG_E("Hash map  conflict.");
                return false;
            }

            // �������Ϣ����MAP
            tempAccesspointMap->insert(std::make_pair(ap.id, ap));

            // ȡ��ÿ��IP�Σ�������RadixTree
            {
                std::string ip(matches[2].first, matches[2].second);
                boost::erase_all(ip,"\n");
                boost::erase_all(ip,"\r");
                boost::char_separator<char> sep(";/");
                tokenizer tokens(ip, sep);
                for (tokenizer::iterator it = tokens.begin();it != tokens.end(); ++it)
                {
                    // ȡ��IP��ַ����ת��unsigned int
                    unsigned int addr = ntohl(inet_addr((*it).c_str()));
                    ++it;

                    // ȡ����������ת��unsigned int
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

    // ������ʱmysql�����map;
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

        // ʹ��utf8�ַ���
        {
            Poco::Data::Statement setUTF8(session);
            setUTF8 << "SET NAMES utf8";
            setUTF8.execute();
        }

        // ��ȡmysql�е�AccessPoint�������浽map
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

        // ���û��Ĭ�Ͻ���㣬�����һ��Ĭ�Ͻ����
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

        // ���ұ������н���㣬���mysql��û�У��Ͳ���˽������Ϣ
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

        // ����ƴ������ӳ���
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

            // �޸ĳɺ���
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

        // ͬ�����ݿ�ɹ�
        {
            // ��m_mysqlAccesspointMap ��д��
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

    // ��m_mysqlAccesspointMap �Ӷ���
    ReadLock rdlock(m_mysqlAccesspointMapRadixTreeMutex);

    addr = htonl(inet_addr(ip.c_str()));

    // �������õ�ip���
    id = m_accesspointRadixTree->findNode(addr);

    ACCESSPOINT_MAP::iterator it = m_mysqlAccesspointMap->find(id);
    //ֻҪ�ҵ���IP��Ӧ�Ľ���㣬�ͷ��أ��������Ƿ�������IDC
    if(it != m_mysqlAccesspointMap->end())
    {
        ap = it->second;
    }
    else
    {
        //�Ҳ�����������㣬ʹ��Ĭ�Ͻ����
        ap = m_defaultAccessPoint;

        //�Ҳ������������        
        LOG_W("Can not identify the access point of IP Address : [" << ip << "]");
    }
    return ap;
}

AccessPoint AccessPointManager::getAccessPoint(std::string ip)
{
    int id;
    unsigned int addr;
    AccessPoint ap;

    // ��m_mysqlAccesspointMap �Ӷ���
    ReadLock rdlock(m_mysqlAccesspointMapRadixTreeMutex);

    addr = htonl(inet_addr(ip.c_str()));

    // �Ȳ�������ip�⣬��������ڣ��ٲ������õ�ip���
    id = m_specialRadixTree->findNode(addr);
    if(NGX_RADIX_NO_VALUE == id)
    {
        // �������õ�ip���
        id = m_accesspointRadixTree->findNode(addr);
    }

    ACCESSPOINT_MAP::iterator it = m_mysqlAccesspointMap->find(id);
    if(it != m_mysqlAccesspointMap->end() && (it->second.groupId1 != -1 || it->second.groupId2 != -1))
    {
        ap = it->second;
    }
    else
    {
        //�Ҳ�����������㣬ʹ��Ĭ�Ͻ����
        ap = m_defaultAccessPoint;
        
        LOG_W("Can not identify the access point of IP Address : [" << ip << "], use default one");
    }

    return ap;
}

}
}
}
}
