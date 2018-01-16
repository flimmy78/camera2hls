#include <fstream>

#include  <kugou/httpproxy/httpserver/handler/OrderSongs.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

bool OrderSongs::impl_check()
{
	return true;
}

OrderSongs* OrderSongs::clone(void)
{
    return new OrderSongs(*this);
}

bool OrderSongs::impl_handle()
{
    std::string id = this->m_request.Source_ID;

    return true;
}

}
}
}
}
