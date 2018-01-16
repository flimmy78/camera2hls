#include <fstream>
#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include <kugou/httpproxy/liveencoder/handler/AddEncodeChannel.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderWorker.h>

using namespace boost;
//using namespace kugou::server::http;
using namespace kugou::httpproxy::liveencoder::encoder;

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace handler
{

bool AddEncodeChannel::impl_check()
{
	return true;
}

bool AddEncodeChannel::impl_handle()
{
    this->m_el.addEncodeStreamID(this->m_request.Source_ID);

    this->m_response.Result = "YES";

    return true;
}

}
}
}
}
