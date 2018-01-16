#include <boost/test/included/unit_test.hpp>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>

namespace kugou
{
namespace unittest
{
using namespace kugou::httpproxy::accessserver::core;

void test_StrategyManager(void)
{
    BOOST_CHECK(StrategyManager::instance().updateStrategy() == true);

    std::string okRoom = "123456";
    BOOST_CHECK(StrategyManager::instance().isAllow(okRoom) == true);

    std::string noRoom = "11111";
    BOOST_CHECK(StrategyManager::instance().isAllow(noRoom) == false);
}

}
}
