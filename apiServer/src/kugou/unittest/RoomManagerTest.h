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

    // ��ʼ���б�Ӧ��Ϊ��
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == -1);
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 0) == -1);
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == -1);

    // ���ROOMID
    BOOST_CHECK(RoomManager::instance().publicRoom(10001, FEMALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10002, MALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10003, FEMALE) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10004, MALE) == true);

    // ȫ��ȡ����
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);

    // ��ȡ����feamale
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(FEMALE, 0) == 10003);

    // ȫ��ȡ����
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10001);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);

    // ����һ��feamale
    BOOST_CHECK(RoomManager::instance().concealRoom(10001) == true);

    // ȫ��ȡ����
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);

    // ��ȡ
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(MALE, 10002) == 10004);

    // ȫ��ȡ����
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);

    // ����һ��������public
    BOOST_CHECK(RoomManager::instance().concealRoom(10002) == true);
    BOOST_CHECK(RoomManager::instance().publicRoom(10002, MALE) == true);

    // ȫ��ȡ����
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10003);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10002);
    BOOST_CHECK(RoomManager::instance().getRoom(ALL, 0) == 10004);
}

}
}
