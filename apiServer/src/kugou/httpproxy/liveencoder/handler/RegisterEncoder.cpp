#include <fstream>
#include <kugou/httpproxy/liveencoder/handler/RegisterEncoder.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderWorker.h>
#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

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

bool RegisterEncoder::impl_check()
{
	return true;
}

bool RegisterEncoder::impl_handle()
{
    this->m_sel.addEncoder(m_request.Encoder_ID, m_response.StreamIDList);
    this->m_response.Result = "YES";

    return true;
}

}
}
}
}
