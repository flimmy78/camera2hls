#ifndef STREAMSERVERMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define STREAMSERVERMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <vector>
#include <kugou/base/facility/Singleton.h>
#include <kugou/httpproxy/accessserver/core/RadixTree.h>
#include <kugou/httpproxy/accessserver/bean/GetTestNodeListResponse.h>
#include <kugou/httpproxy/accessserver/bean/GetAddressListResponse.h>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/detail/atomic.hpp>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{
// 流服务
class StreamService
{
public:
    int         groupId;
    std::string ip;
    bool        available;
};

// 流服务器组
class StreamServerGroup
{
public:
    int         id;
    std::string carrier;      // 所属运营商用
    std::string area;           // 所属地区
    std::string testServer;
    bool        available;
    int         costLevel;

    StreamServerGroup()
        : m_index(0)
        , m_size(0)
    {
    }

    void addServer(StreamService& server)
    {
        streamServiceVector.push_back(server);
        boost::interprocess::ipcdetail::atomic_inc32(&m_size);
    }

    bool getNextServer(std::string& ip)
    {
        if(m_size > 0 && available)
        {
            int size = m_size;
            while(size--)
            {
                boost::uint32_t index = boost::interprocess::ipcdetail::atomic_inc32(&m_index);
                index = index % m_size;
                if(streamServiceVector[index].available)
                {
                    ip = streamServiceVector[index].ip;
                    return true;
                }
            }
        }
        return false;
    }
private:
    volatile boost::uint32_t    m_index;
    volatile boost::uint32_t    m_size;
    std::vector<StreamService> streamServiceVector;
};

class StreamServerManager : public kugou::base::facility::Singleton<StreamServerManager>
{
    friend class kugou::base::facility::Singleton<StreamServerManager>;
    typedef std::map<int, StreamServerGroup>  STREAMSERVERGROUP_MAP;
    typedef boost::shared_lock <boost::shared_mutex> ReadLock;
    typedef boost::unique_lock <boost::shared_mutex> WriteLock;
public:
    StreamServerManager(void);
    ~StreamServerManager(void);
    
    bool getTestServerList(std::vector<kugou::httpproxy::accessserver::bean::NodeListObject>&);
    bool getAddressList(int groupid, std::vector<kugou::httpproxy::accessserver::bean::AddressListObject>&);

    bool flashDatabase(void);
private:
    boost::shared_ptr<STREAMSERVERGROUP_MAP> m_streamServerGroupMap;
    boost::shared_mutex                      m_streamServerGroupMapMutex;
};

}
}
}
}

#endif
