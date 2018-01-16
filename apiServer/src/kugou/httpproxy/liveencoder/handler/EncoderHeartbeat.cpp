#include <fstream>
#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include <kugou/httpproxy/liveencoder/handler/EncoderHeartbeat.h>

using namespace boost;
//using namespace kugou::server::http;
using namespace kugou::httpproxy::liveencoder::scheduleencoder;

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace handler
{

bool EncoderHeartbeat::impl_check()
{
	return true;
}

bool EncoderHeartbeat::impl_handle()
{
    this->m_sel.onHeartbeat(this->m_request);

    this->m_response.Result = "YES";

    return true;
}

}
}
}
}
