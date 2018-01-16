#include <boost/test/included/unit_test.hpp>
#include <kugou/httpproxy/httpserver/roommanager/RoomManager.h>

namespace kugou
{
namespace unittest
{
using namespace kugou::httpproxy::httpserver::roommanager;

void test_RoomManager(void)
{
    std::string str;
    BOOST_CHECK(str.size() == 0);

    // 初始化列表应该为空
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == -1);
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 0) == -1);
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == -1);

    // 添加ROOMID
    BOOST_CHECK(RoomManager::instance().publicRoom(10001, FEMALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10002, MALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10003, FEMALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10004, MALE) == true);

    // 全部取出来
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);

    // 获取两个feamale
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == 10003);

    // 全部取出来
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);

    // 锁定一个feamale
    BOOST_CHECK(RoomManager::instance().concealRoom(10001) == true);

    // 全部取出来
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);

    // 获取
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 10002) == 10004);

    // 全部取出来
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);

    // 锁定一个，马上public
    BOOST_CHECK(RoomManager::instance().concealRoom(10002) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10002, MALE) == true);

    // 全部取出来
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
}

}
}
