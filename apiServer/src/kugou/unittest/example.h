#include <boost/test/included/unit_test.hpp>

namespace kugou
{
namespace unittest
{

void free_test_function( int i )  
{     
    BOOST_CHECK( i < 4 /* test assertion */ ); 
}

#if 0
#define BOOST_TEST_MODULE stringtest
#include <boost/test/included/unit_test.hpp>
#include "str.h"

BOOST_AUTO_TEST_SUITE (stringtest) // name of the test suite is stringtest

BOOST_AUTO_TEST_CASE (test1)
{
  mystring s;
  BOOST_CHECK(s.size() == 0);
}

BOOST_AUTO_TEST_CASE (test2)
{
  mystring s;
  s.setbuffer("hello world");
  BOOST_REQUIRE_EQUAL ('h', s[0]); // basic test 
}

BOOST_AUTO_TEST_SUITE_END( )

BOOST_AUTO_TEST_SUITE (aenumtest) 

BOOST_AUTO_TEST_CASE (test1)
{
  typedef enum {red = 8, blue, green = 1, yellow, black } color;
  color c = green;
  BOOST_WARN(sizeof(green) > sizeof(char));
  BOOST_CHECK(c == 2); 
  BOOST_REQUIRE(yellow > red); 
  BOOST_CHECK(black != 4);
}

BOOST_AUTO_TEST_SUITE_END( )
#endif

}
}
