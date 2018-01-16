/**
 * Title: manage encoder
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-11
 * Version: 1.0
 * History: [1.0 2013-10-11]
 */


#ifndef LIBRARYIMPL_HANDLE_SERVER_KUGOU_H
#define LIBRARYIMPL_HANDLE_SERVER_KUGOU_H

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

class LibraryImpl
{
public:
    virtual bool addEncodeStreamID(std::string id, std::string encoderID = "") = 0;
    virtual void delEncodeStreamID(std::string id) = 0;
    virtual void addEncodeStreamIDSuccess(std::string id) = 0;
};

}
}
}
}

#endif
