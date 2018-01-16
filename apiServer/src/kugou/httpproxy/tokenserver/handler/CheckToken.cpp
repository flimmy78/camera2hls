#include <cryptopp/des.h>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <kugou/httpproxy/tokenserver/handler/CheckToken.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/tokenserver/bean/CreateTokenRequest.h>
#include <kugou/base/facility/Clock.h>
#include <kugou/base/facility/Base64.h>

using namespace CryptoPP;
using namespace kugou::base::facility;
using namespace kugou::httpproxy::tokenserver::bean;

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

bool CheckToken::impl_check()
{
	return true;
}

CheckToken* CheckToken::clone(void)
{
    return new CheckToken(*this);
}

bool CheckToken::impl_handle()
{
    LOG_D("Check token [userid : " << this->m_request.userid 
                    << "] [roomid : " << this->m_request.roomid 
                    << "] [streamid : " << this->m_request.streamid 
                    << "] [ip : " << this->m_request.ip 
                    << "] [token size : " << this->m_request.token.size() << "].");

    try
    {
        this->m_response.result = "1";

        // 解token
        std::string token;
        if(Base64::Base64Decode(this->m_request.token, &token) == false)
        {
            LOG_D("Base64Decode error.");
            return false;
        }

        unsigned char value[1024];
        memset(value, '\0', 1024);
        memcpy((char*)value, token.c_str(), token.size() + 1);

        byte key[DES_EDE2::KEYLENGTH];
        char keyString[] = "1234567890abcdefg";
        memcpy(key, keyString, DES_EDE2::KEYLENGTH);
        boost::scoped_ptr<BlockTransformation> bt(new DES_EDE2_Decryption(key, DES_EDE2::KEYLENGTH));

        int steps = token.size() / bt->BlockSize();
        for(int i = 0; i < steps; i++)
        {
            int offset = i * bt->BlockSize();
            bt->ProcessBlock(value + offset);
        }

        //判断streamid等是否一致
        CreateTokenRequest tokenInfo;
        tokenInfo.fromJson((char*)value);

        LOG_D("token value [userid : " << tokenInfo.userid 
                        << "] [roomid : " << tokenInfo.roomid 
                        << "] [streamid : " << tokenInfo.streamid 
                        << "] [ip : " << tokenInfo.ip 
                        << "] [action : " << tokenInfo.action 
                        << "] [effectivetime : " << tokenInfo.effectivetime 
                        << "] [time : " << tokenInfo.time << "].");

        // 判断时间是否过期
        long long time = boost::lexical_cast<long long> (tokenInfo.time);
        if(!tokenInfo.effectivetime.empty())
        {
            long long effective = boost::lexical_cast<long long> (tokenInfo.effectivetime);
            if(Clock::instance().getSecond() - time > effective)
            {
                LOG_D("Token time exceed the effective time : " << effective << " second.");
                return false;
            }
        }
        else if(Clock::instance().getSecond() - time > 30)
        {
            LOG_D("Token time exceed the time limit 30 second.");
            return false;
        }
        
        if(this->m_request.userid.compare(tokenInfo.userid))
        {
            LOG_D("userid is not the same.");
            return false;
        }

        if(this->m_request.roomid.compare(tokenInfo.roomid))
        {
            LOG_D("roomid is not the same.");
            return false;
        }

        if(this->m_request.streamid.compare(tokenInfo.streamid))
        {
            LOG_D("streamid is not the same.");
            return false;
        }

        if(this->m_request.action.compare(tokenInfo.action))
        {
            LOG_W("action is not the same, but we still let it go.");
        }

        if(this->m_request.ip.compare(tokenInfo.ip))
        {
            LOG_W("ip is not the same, but we still let it go.");
        }
    }
    catch(...)
    {
        LOG_E("Check token exception, token error.");
        return false;
    }

    this->m_response.result = "0";
    return true;
}

}
}
}
}
