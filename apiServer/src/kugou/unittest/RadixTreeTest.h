#include <boost/test/included/unit_test.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <kugou/httpproxy/accessserver/core/RadixTree.h>

namespace kugou
{
namespace unittest
{
using namespace std;
using namespace boost::algorithm;
using namespace kugou::base::net;
using namespace kugou::httpproxy::accessserver::core;

void load_file(std::string& str, std::string fileName)
{
    std::ifstream is(fileName);
    str.erase();
    if(is.bad())
    {
        return;
    }

    str.reserve(static_cast<std::string::size_type>(is.rdbuf()->in_avail()));
    char c;
    while(is.get(c))
    {
        if(str.capacity() == str.size())
            str.reserve(str.capacity() * 3);
        str.append(1, c);
    }
    is.close();
}

void test_Radix(void)
{    
    std::string text;
    load_file(text, "anhui.txt");
    //load_file(text, "hkbn.txt");
    BOOST_CHECK(!text.empty());
    if(!text.empty())
    {
        std::string reStr = "acl (.*)\\{(.*)\\};(.*)";
        boost::regex re;
        re = reStr;
        boost::cmatch matches;

        BOOST_CHECK(boost::regex_match(text.c_str(), matches, re));
        if (boost::regex_match(text.c_str(), matches, re))
        {
            for (int i = 1; i < matches.size(); i++)
            {
                // sub_match::first and sub_match::second are iterators that
                // refer to the first and one past the last chars of the
                // matching subexpression
                std::string match(matches[i].first, matches[i].second);
            }

            typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
            {
                std::string areaOperator(matches[1].first, matches[1].second);
                boost::char_separator<char> sep("_");
                tokenizer tokens(areaOperator, sep);
                
                std::string operators;
                std::string area;
                for (tokenizer::iterator it = tokens.begin();it != tokens.end(); ++it)  
                {
                    if(operators.empty())
                    {
                        operators = *it;
                    }
                    else if((*it).find("addr", 0, 4) && area.empty())
                    {
                        area = *it;
                    }
                    else
                    {
                        break;
                    }
                }                
            }

            {
                RadixTree rt;
                std::string ip(matches[2].first, matches[2].second);
                boost::erase_all(ip,"\n");
                boost::erase_all(ip,"\r");
                boost::char_separator<char> sep(";/");
                tokenizer tokens(ip, sep);
                for (tokenizer::iterator it = tokens.begin();it != tokens.end(); ++it)
                {
                    unsigned int addr = ntohl(inet_addr((*it).c_str()));
                    ++it;

                    unsigned int mask = 0;
                    {
                        unsigned int bit = 0x80000000;
                        int count = boost::lexical_cast<int> (*it);
                        for(int i = 0; i < count; i++)
                        {
                            mask |= bit;
                            bit >>= 1;
                        }
                    }
                    rt.insertNode(addr , mask , 100);
                }

                /*
                unsigned int addr = ntohl(inet_addr("192.168.0.0"));
                unsigned int mask = ntohl(inet_addr("255.255.0.0"));                
                rt.insertNode(addr , mask , 100);
                */

                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("111.38.0.1"))));
                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("111.39.255.254"))));

                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.193.64.1"))));
                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.193.127.254"))));

                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.209.0.1"))));
                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.209.255.254"))));

                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.210.0.1"))));
                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("120.210.255.254"))));

                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("202.141.176.1"))));
                BOOST_CHECK(100 == rt.findNode(htonl(inet_addr("202.141.191.254"))));

            }
        }
    }
    
}

}
}
