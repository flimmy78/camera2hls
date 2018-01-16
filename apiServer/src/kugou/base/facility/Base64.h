#ifndef BASE64_FACILITY_BASE_KUGOU_H
#define BASE64_FACILITY_BASE_KUGOU_H

#include <string>
#include <iostream>  
#include <sstream>  

namespace kugou
{
namespace base
{
namespace facility
{

class Base64
{
public:
    static bool Base64Encode(const std::string& input, std::string* output);
    static bool Base64Decode(const std::string& input, std::string* output);

};

}
}
}

#endif
