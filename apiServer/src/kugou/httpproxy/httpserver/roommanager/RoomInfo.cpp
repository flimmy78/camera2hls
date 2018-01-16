#include <iostream>
#include <kugou/httpproxy/httpserver/roommanager/RoomInfo.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace roommanager
{

RoomInfo::RoomInfo(int id, int sex)
    : m_status(PUBLIC)
{
    m_roomid = id;
    m_sex = sex;
}


RoomInfo::~RoomInfo(void)
{
    //std::cout << "release SongInfo id : " << this->userid << std::endl;
}

}
}
}
}
