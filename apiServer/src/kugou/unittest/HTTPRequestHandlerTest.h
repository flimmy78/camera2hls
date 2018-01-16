#include <boost/test/included/unit_test.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <kugou/base/net/HTTPRequestHandler.h>
#include <kugou/httpproxy/accessserver/handler/GetAddressList.h>

namespace kugou
{
namespace unittest
{
using namespace std;
using namespace boost::algorithm;
using namespace kugou::base::net;
using namespace kugou::httpproxy::accessserver::handler;

void test_HTTPRequestHandler(void)
{
    // Symbol	Meaning
    // c	Match the literal character c once, unless it is one of the special characters.
    // ^	Match the beginning of a line.
    // .	Match any character that isn't a newline.
    // $	Match the end of a line.
    // |	Logical OR between expressions.
    // ()	Group subexpressions.
    // []	Define a character class.
    // *	Match the preceding expression zero or more times.
    // +	Match the preceding expression one ore more times.
    // ?	Match the preceding expression zero or one time.
    // {n}	Match the preceding expression n times.
    // {n,}	Match the preceding expression at least n times.
    // {n, m}	Match the preceding expression at least n times and at most m times.
    // \d	Match a digit.
    // \D	Match a character that is not a digit.
    // \w	Match an alpha character, including the underscore.
    // \W	Match a character that is not an alpha character.
    // \s	Match a whitespace character (any of \t, \n, \r, or \f).
    // \S	Match a non-whitespace character.
    // \t	Tab.
    // \n	Newline.
    // \r	Carriage return.
    // \f	Form feed.
    // \m	Escape m, where m is one of the metacharacters described above: ^, ., $, |, (), [], *, +, ?, \, or /.

    if(1)   // 测试正则表达式
    {
        std::string str = "2/8/2014";
        std::string reStr = "(\\d{1,2})\/(\\d{1,2})\/(\\d{2}|\\d{4})";
        boost::regex re;
        re = reStr;
        boost::cmatch matches;

        BOOST_CHECK(boost::regex_match(str.c_str(), matches, re));
        if (boost::regex_match(str.c_str(), matches, re))
        {
            // matches[0] contains the original string.  matches[n]
            // contains a sub_match object for each matching
            // subexpression
            for (int i = 1; i < matches.size(); i++)
            {
                // sub_match::first and sub_match::second are iterators that
                // refer to the first and one past the last chars of the
                // matching subexpression
                std::string match(matches[i].first, matches[i].second);
            }

            BOOST_CHECK(matches[1] == "2");
            BOOST_CHECK(matches[2] == "8");
            BOOST_CHECK(matches[3] == "2014");
        }

        /*
        str = "111.38.0.0/15;";
        reStr = "((\\d{1,3})\\.(\\d{1,3})(.*))";
        re = reStr;

        BOOST_CHECK(boost::regex_search(str.c_str(), matches, re));
        if(boost::regex_search(str.c_str(), matches, re))
        {
            for (int i = 1; i < matches.size(); i++)
            {
                std::string match(matches[i].first, matches[i].second);
            }
        }
        */

        str = "http://kuggou.com/api?jsonStr={abc=1}";
        reStr = "(.*)\\?jsonStr=(.*)";
        re = reStr;

        BOOST_CHECK(boost::regex_search(str.c_str(), matches, re));
        if(boost::regex_search(str.c_str(), matches, re))
        {
            for (int i = 1; i < matches.size(); i++)
            {
                std::string url(matches[i].first, matches[i].second);
            }
            BOOST_CHECK(matches[1] == "http://kuggou.com/api");
            BOOST_CHECK(matches[2] == "{abc=1}");
        }
    }
    
    HTTPRequestHandler handler("");    
    handler.registerHandler(new GetAddressList);

    string uri;
    string contentJson;
    string outJson;

    // 测试content带json
    uri = "/api/access/getAddressList.json";
    contentJson = "{\"UsageType\":1,\"User\":0}";
    BOOST_CHECK("192.168.1.1", handler.dispatch(uri, contentJson, outJson) == HTTPReply::ok);

    // 测试url带参数
    //uri = "/api/access/getAddressList.json?jsonStr={\"UsageType\":1,\"User\":0}";
    // uri 的JSON串做了base64加密
    uri = "/api/access/getAddressList.json?callback=abc123&jsonStr=eyJVc2FnZVR5cGUiOjEsIlVzZXIiOjB9&_=1392090032255";
    BOOST_CHECK("192.168.1.1", handler.dispatch(uri, contentJson, outJson) == HTTPReply::ok);
    
}

}
}
