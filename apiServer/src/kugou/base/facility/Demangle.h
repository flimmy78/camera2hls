/**
 * Title: <Please input title>
 * Description: Use "c++filt" to transform mangling c++ name into origin name.
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef DEMANGLE_FACILITY_BASE_KUGOU_H
#define DEMANGLE_FACILITY_BASE_KUGOU_H

#include <iosfwd>
#include <string>

namespace kugou
{
namespace base
{
namespace facility
{

class Demangle
{
public:
	static std::string demangle(const std::string& name);

	template<typename T> static std::string demangle();

private:
	//call system method to demangle
    static std::string impl_demangle(const std::string& name);
};

}
}
}

#endif //
