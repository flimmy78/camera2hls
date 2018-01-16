#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

#include <kugou/unittest/example.h>
#include <kugou/unittest/RoomManagerTest.h>
#include <kugou/unittest/HTTPRequestHandlerTest.h>
#include <kugou/unittest/RadixTreeTest.h>
#include <kugou/unittest/StrategyManagerTest.h>

using namespace boost::unit_test;
using namespace kugou::unittest;

test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite* example = BOOST_TEST_SUITE( "example" );
    test_suite* RoomManagerTest = BOOST_TEST_SUITE( "RoomManagerTest" );
    test_suite* HTTPRequestHandlerTest = BOOST_TEST_SUITE( "HTTPRequestHandlerTest" );
    test_suite* RadixTreeTest = BOOST_TEST_SUITE( "RadixTreeTest" );
    test_suite* StrategyManager = BOOST_TEST_SUITE( "StrategyManager" );
   
    int params[] = { 1, 2, 3, 4, 5 };
    example->add( BOOST_PARAM_TEST_CASE( &free_test_function, params, params+3 ) );

    RoomManagerTest->add(BOOST_TEST_CASE(&test_RoomManager));
    HTTPRequestHandlerTest->add(BOOST_TEST_CASE(&test_HTTPRequestHandler));
    RadixTreeTest->add(BOOST_TEST_CASE(&test_Radix));
    StrategyManager->add(BOOST_TEST_CASE(&test_StrategyManager));

    framework::master_test_suite().add( example );
    framework::master_test_suite().add( RoomManagerTest );
    framework::master_test_suite().add( HTTPRequestHandlerTest );
    framework::master_test_suite().add( RadixTreeTest );
    framework::master_test_suite().add( StrategyManager );

    return 0;
}
