#include <boost/lexical_cast.hpp>
#include  <kugou/httpproxy/httpserver/handler/RoomPublic.h>
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

bool RoomPublic::impl_check()
{
    if(this->m_request.RoomID == -1)
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "RoomID is not set.";
        return false;
    }
    if(this->m_request.Sex != MALE && this->m_request.Sex != FEMALE)
    {
        this->m_response.Result = "NO";
        this->m_response.Description = "Sex value should in 1,2";
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

RoomPublic* RoomPublic::clone(void)
{
    return new RoomPublic(*this);
}

bool RoomPublic::impl_handle()
{
    int RoomID = this->m_request.RoomID;
    int Sex = this->m_request.Sex;

    if(RoomManager::instance().publicRoom(RoomID, Sex))
    {
        this->m_response.Result = "YES";
    }
    else
    {
        this->m_response.Result = "NO";
    }

    return true;
}

}
}
}
}
