#ifndef ROOMINFO_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMINFO_HTTPSERVER_HTTPPROXY_KUGOU

#include <iostream>
#include <boost/thread/mutex.hpp>
namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace roommanager
{
typedef enum ROOM_STATUS{INIT, PUBLIC, CONCEAL};

class RoomInfo
{
public:
    RoomInfo(int id, int sex);
    ~RoomInfo(void);
public:
    int             m_roomid;         // ����id
    int             m_sex;            // ����ӵ�м��Ա�

    void updateStatus(ROOM_STATUS status)
    {
        boost::mutex::scoped_lock scopedLock(m_mutexStatus);
        this->m_status = status;
    }

    ROOM_STATUS getStatus(void)
    {
        return this->m_status;
    }
private:
    RoomInfo(void){}
    enum ROOM_STATUS    m_status;         //  ���״̬
    boost::mutex        m_mutexStatus;
};

}
}
}
}

#endif
