#include <boost/lexical_cast.hpp>
#include  <kugou/httpproxy/httpserver/handler/RoomConceal.h>
#include <kugou/httpproxy/httpserver/roommanager/RoomManager.h>
#include <kugou/base/facility/MD5.h>

using namespace kugou::httpproxy::httpserver::roommanager;
using namespace kugou::base::facility;

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

bool RoomConceal::impl_check()
{
    if(this->m_request.RoomID == -1)
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "RoomID is not set.";
        return false;
    }

    // 先对key进行md5加密，加密后的串+RoomID，进行二次加密
    std::string key = "(d$@b#13SP?*";
    MD5 keyMd5(key);
    std::string keyMd5String = keyMd5.md5();
    keyMd5String.append(boost::lexical_cast<std::string>(this->m_request.RoomID));
    MD5 md5(keyMd5String);   

    //检测md5是否正确
    if(this->m_request.RoomK.compare(md5.md5()))
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "MD5 error";
        return false;
    }
	return true;
}

RoomConceal* RoomConceal::clone(void)
{
    return new RoomConceal(*this);
}

bool RoomConceal::impl_handle()
{
    int id = this->m_request.RoomID;

    if(RoomManager::instance().concealRoom(id))
    {
        this->m_response.Result = "YES";
    }
    else
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "Can not find RoomID";
    }

    return true;
}

}
}
}
}
