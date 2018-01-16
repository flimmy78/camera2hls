#ifndef STRATEGYMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define STRATEGYMANAGER_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <set>
#include <kugou/base/facility/Singleton.h>
#include <kugou/httpproxy/accessserver/bean/WhiteRoomListStrategy.h>
#include <kugou/httpproxy/accessserver/bean/RelayRoomStrategy.h>
#include <kugou/httpproxy/accessserver/bean/AllowRoomStrategy.h>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/detail/atomic.hpp>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{

class StrategyManager : public kugou::base::facility::Singleton<StrategyManager>
{
    friend class kugou::base::facility::Singleton<StrategyManager>;
    typedef std::set<std::string>  ROOM_SET;
    typedef boost::shared_lock <boost::shared_mutex> ReadLock;
    typedef boost::unique_lock <boost::shared_mutex> WriteLock;
public:
    StrategyManager(void);
    ~StrategyManager(void);
    
	bool isInWhiteList(std::string& roomid);
    bool isRelay(std::string& roomid);
    bool isAllow(std::string& roomid);

    inline std::string getRelayIP(void){return m_relayIP;}
    inline std::string getRelayPort(void){return m_relayPort;}

    bool updateStrategy(void);
private:
	bool isInMod(std::string& roomid, int mod);
	boost::shared_ptr<ROOM_SET>         m_whiteRoomSet;
    boost::shared_ptr<ROOM_SET>         m_relayRoomSet;
    boost::shared_ptr<ROOM_SET>         m_allowRoomSet;
    boost::shared_mutex                 m_allRoomSetMutex;

    int     m_relayRoomSetEnable;
	int     m_relayRoomMod;
    int     m_allowRoomSetEnable;
	int     m_allowRoomMod;

    std::string     m_relayIP;
    std::string     m_relayPort;
};

}
}
}
}

#endif
