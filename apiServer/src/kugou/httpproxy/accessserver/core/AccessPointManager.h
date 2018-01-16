#ifndef ACCESSPOINTMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define ACCESSPOINTMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <vector>
#include <kugou/base/facility/Singleton.h>
#include <kugou/httpproxy/accessserver/core/RadixTree.h>
#include <boost/shared_ptr.hpp>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{
class AccessPoint
{
public:
    int         id;
    std::string carrier;        // 所属运营商用
    std::string area;           // 所属地区
    int         groupId1;         // 对应第一个流服务器组
    int         groupId2;         // 对应第二个流服务器组
};

class AccessPointManager : public kugou::base::facility::Singleton<AccessPointManager>
{
    friend class kugou::base::facility::Singleton<AccessPointManager>;
    typedef std::vector<std::string> FILES_VECTOR;
    typedef std::map<int, AccessPoint> ACCESSPOINT_MAP;
    typedef boost::shared_lock <boost::shared_mutex> ReadLock;
    typedef boost::unique_lock <boost::shared_mutex> WriteLock;
public:
    AccessPointManager(void);
    ~AccessPointManager(void);

    bool loadIP(const std::string& geoIPPath, const std::string& specialIPPath);
    AccessPoint getAccessPoint(std::string ip);
    AccessPoint getAccessPointNoSpecial(std::string ip);
    AccessPoint getDefaultAccessPoint(void);
private:
    bool loadGeoIP(bool isSpecial, std::string path, boost::shared_ptr<RadixTree>&, boost::shared_ptr<ACCESSPOINT_MAP>&);
    bool loadFile(std::string& text, std::string& fileName);
    void recursionLoadFiles(std::string path, FILES_VECTOR& v);
    bool flashAccessPointDatabase(boost::shared_ptr<RadixTree>&, boost::shared_ptr<RadixTree>&, boost::shared_ptr<ACCESSPOINT_MAP>&);
    //FILES_VECTOR m_allFileVector;

    AccessPoint     m_defaultAccessPoint;

    boost::shared_ptr<RadixTree>       m_accesspointRadixTree;
    boost::shared_ptr<RadixTree>       m_specialRadixTree;
    
    boost::shared_ptr<ACCESSPOINT_MAP> m_mysqlAccesspointMap;
    boost::shared_mutex                m_mysqlAccesspointMapRadixTreeMutex;
};

}
}
}
}

#endif
