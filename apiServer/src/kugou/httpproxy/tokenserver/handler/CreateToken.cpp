#include <boost/lexical_cast.hpp>
#include <cryptopp/des.h>

#include <kugou/httpproxy/tokenserver/handler/CreateToken.h>
#include <kugou/base/facility/log.h>
#include <kugou/base/facility/Clock.h>
#include <kugou/base/facility/Base64.h>

using namespace kugou::base::facility;
using namespace CryptoPP;

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

bool CreateToken::impl_check()
{
	return true;
}

CreateToken* CreateToken::clone(void)
{
    return new CreateToken(*this);
}

bool CreateToken::impl_handle()
{
    LOG_D("Create token [userid : " << this->m_request.userid 
                    << "] [roomid : " << this->m_request.roomid 
                    << "] [streamid : " << this->m_request.streamid 
                    << "] [action : " << this->m_request.action 
                    << "] [effectivetime : " << this->m_request.effectivetime 
                    << "] [ip : " << this->m_request.ip << "].");

    try
    {
        // 加入当前时间
        this->m_request.time = boost::lexical_cast<std::string> (Clock::instance().getSecond());
        LOG_D("Generate [time : " << this->m_request.time << "].");

        // 生成jsong串
        std::string token = this->m_request.toJson();

        // 使用des对称加密
        // 被编码的串要8对齐，这里要补齐
        int padSize = token.size() % 8;
        for(int i = 0; padSize > 0 && i < 8 - padSize; i++)
        {
            token.append("\n");  // 用回车填充
        }
 
        byte key[DES_EDE2::KEYLENGTH];
        char keyString[] = "1234567890abcdefg";
        memcpy(key, keyString, DES_EDE2::KEYLENGTH);
        boost::scoped_ptr<BlockTransformation> bt(new DES_EDE2_Encryption(key, DES_EDE2::KEYLENGTH));
        int steps = token.size() / bt->BlockSize();
        unsigned char value[1024];
        memset(value, '\0', 1024);
        strcpy((char*)value,token.c_str());
        for(int i = 0; i < steps; i++)
        {
            int offset = i * bt->BlockSize();
            bt->ProcessBlock(value + offset);
        }

        std::string jsonStr((char*)value, token.size());    

        // 如果加密串里出现0这个的结束符，字符串就不能处理了，所以返回失败先。
        // TODO如果加密串里出现: "之类的怎么办？
        if(jsonStr.size() % 8 != 0)
        {
            this->m_response.result = "1";
            this->m_response.token.clear();
            LOG_E("Token have \0 ");
            return  0;
        }

        // 把编码后的json串编码成base64，（因为用二进制时，json不一定能解释）
        if(Base64::Base64Encode(jsonStr, &this->m_response.token) == false)
        {
            LOG_D("Base64Encode error.");
            this->m_response.result = "1";
            return false;
        }
        this->m_response.result = "0";
    }
    catch(...)
    {
        LOG_D("Check token exception.");
        this->m_response.result = "1";
        this->m_response.token.clear();
        return false;
    }

    return true;
}

}
}
}
}
