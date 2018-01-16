#include <kugou/base/facility/Base64.h>

#include <boost/archive/iterators/base64_from_binary.hpp>  
#include <boost/archive/iterators/binary_from_base64.hpp>  
#include <boost/archive/iterators/transform_width.hpp>

#include <stdio.h>
#include <string.h>

namespace kugou
{
namespace base
{
namespace facility
{
bool Base64::Base64Encode(const std::string& input, std::string* output)
{  
    typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;  
    std::stringstream result;  
    copy(Base64EncodeIterator(input.begin()) , Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));  
    size_t equal_count = (3 - input.length() % 3) % 3;  
    for (size_t i = 0; i < equal_count; i++) 
    {  
        result.put('=');  
    }
    *output = result.str();
    return output->empty() == false;  
}  

bool Base64::Base64Decode(const std::string& input, std::string* output) 
{  
  typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;  
  std::stringstream result;  
  try {  
    copy(Base64DecodeIterator(input.begin()) , Base64DecodeIterator(input.end()), std::ostream_iterator<char>(result));  
  } catch(...) {  
    return false;  
  }  
  *output = result.str();  
  return output->empty() == false;  
}  

}
}
}
