#include  <kugou/httpproxy/httpserver/handler/RoomGet.h>
#include <kugou/httpproxy/httpserver/roommanager/RoomManager.h>

using namespace kugou::httpproxy::httpserver::roommanager;

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

bool RoomGet::impl_check()
{
    if(this->m_request.Sex < 0 || this->m_request.Sex > 2)
    {
        this->m_response.Result = "NO";
        this->m_response.RoomID = -1;
        this->m_response.Description = "Sex value should in 0,1,2.";
        return false;
    }
	return true;
}

RoomGet* RoomGet::clone(void)
{
    return new RoomGet(*this);
}

bool RoomGet::impl_handle()
{
    int sex = this->m_request.Sex;
    int lastId = this->m_request.LastRoomID;
    int ret = RoomManager::instance().getRoom(sex, lastId);
    if(ret < 0)
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "Queue is empty.";
    }
    else
    {
        this->m_response.Result = "YES";
    }
    this->m_response.RoomID = ret;

    return true;
}

}
}
}
}
