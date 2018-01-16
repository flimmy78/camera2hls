#ifndef ROOMMANAGER_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMMANAGER_HTTPSERVER_HTTPPROXY_KUGOU
#include <map>
#include <queue>
#include <list>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <kugou/httpproxy/httpserver/roommanager/RoomInfo.h>
#include <kugou/base/facility/Singleton.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace roommanager
{
typedef enum SEX_TYPE{ALL = 0, MALE = 1, FEMALE = 2};

class RoomManager : public kugou::base::facility::Singleton<RoomManager>
{
    friend class kugou::base::facility::Singleton<RoomManager>;
public:
    typedef boost::shared_ptr<RoomInfo> SharedRoomInfo;
    typedef boost::unordered_map<int, SharedRoomInfo> IntROOMInfoUMap;
public:

    bool publicRoom(int id, int sex);
    bool concealRoom(int id);
    int  getRoom(int sex, int lastId);
    long long getRoomSize(void);
protected:
    RoomManager(void){}
private:
    IntROOMInfoUMap     m_roomMap;
    boost::mutex        m_mutexRoomMap;

#ifdef  SEPARATE_QUEUE
    std::queue<SharedRoomInfo>      m_femaleRoomQueue;
    boost::mutex                    m_mutexFemaleRoomQueue;

    std::queue<SharedRoomInfo>      m_maleRoomQueue;
    boost::mutex                    m_mutexMaleRoomQueue;
#else
    std::list<SharedRoomInfo>       m_roomList;
    boost::mutex                    m_mutexRoomList;
#endif
};

}
}
}
}

#endif
