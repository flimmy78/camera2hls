#include <kugou/httpproxy/accessserver/core/StrategyManager.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <kugou/base/facility/log.h>

#include <boost/lexical_cast.hpp>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/Statement.h>

#ifdef _MSC_VER
std::string WhiteRoomList_File = "config\\whiteRoomList.json";
std::string RelayRoom_File = "config\\relayRoom.json";
std::string AllowRoom_File = "config\\allowRoom.json";
#elif defined __GNUC__
std::string WhiteRoomList_File = "config/whiteRoomList.json";
std::string RelayRoom_File = "config/relayRoom.json";
std::string AllowRoom_File = "config/allowRoom.json";
#endif

using namespace kugou::httpproxy::accessserver::bean;
using namespace kugou::httpproxy::accessserver::system;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{

StrategyManager::StrategyManager(void)
    : m_relayRoomSet(new ROOM_SET)
    , m_allowRoomSet(new ROOM_SET)
    , m_relayRoomSetEnable(0)
	, m_relayRoomMod(0)
    , m_allowRoomSetEnable(0)
	, m_allowRoomMod(0)
{

}

StrategyManager::~StrategyManager(void)
{
}

bool StrategyManager::updateStrategy(void)
{
    bool ret = true;

    try
    {
        // ����relay�����ļ�relayRoom.json
        RelayRoomStrategy relayRoom;
        if(!relayRoom.fromJsonFile(RelayRoom_File))
        {
            LOG_E("Read relay room list failed.");
            return false;
        }

        boost::shared_ptr<ROOM_SET> tempRelayRoom(new ROOM_SET);
        std::string relayRoomList;
        for(std::vector<std::string>::iterator it = relayRoom.RelayRoomList.begin(); it != relayRoom.RelayRoomList.end(); it++)
        {
            tempRelayRoom->insert(*it);
            relayRoomList.append(*it).append(",");
        }

        LOG_I("relay room : " << relayRoomList);

        // ����allow�����ļ�allowRoom.json
        AllowRoomStrategy allowRoom;
        if(!allowRoom.fromJsonFile(AllowRoom_File))
        {
            LOG_E("Read allow room list failed.");
            return false;
        }

        boost::shared_ptr<ROOM_SET> tempAllowRoom(new ROOM_SET);
        std::string allowRoomList;
        for(std::vector<std::string>::iterator it = allowRoom.AllowRoomList.begin(); it != allowRoom.AllowRoomList.end(); it++)
        {
            tempAllowRoom->insert(*it);
            allowRoomList.append(*it).append(",");
        }
        LOG_I("allow room : " << allowRoomList);

        // ����white�����ļ�whiteRoomList.json
        WhiteRoomListStrategy whiteRoom;
        if(!whiteRoom.fromJsonFile(WhiteRoomList_File))
        {
            LOG_E("Read white room list failed.");
            return false;
        }

        boost::shared_ptr<ROOM_SET> tempWhiteRoom(new ROOM_SET);
        std::string whiteRoomList;
		for(std::vector<std::string>::iterator it = whiteRoom.WhiteRoomList.begin(); it != whiteRoom.WhiteRoomList.end(); it++)
        {
            tempWhiteRoom->insert(*it);
            whiteRoomList.append(*it).append(",");
        }
        LOG_I("white room : " << whiteRoomList);

        // ͬ�����Գɹ�
        {
            // ��m_relayRoomSet m_allowRoomSet ��д��
            WriteLock writeLock(m_allRoomSetMutex);

            LOG_I("Reset relay room begin");
            m_relayRoomSet = tempRelayRoom;
            m_relayRoomSetEnable = relayRoom.Enable;
			m_relayRoomMod = relayRoom.Mod;
			m_relayIP = relayRoom.RelayIP;
			m_relayPort = relayRoom.RelayPort;
            LOG_I("Reset relay room end, is enable ? [" << m_relayRoomSetEnable << "].");

            LOG_I("Reset allow room begin");
            m_allowRoomSet = tempAllowRoom;
            m_allowRoomSetEnable = allowRoom.Enable;
			m_allowRoomMod = allowRoom.Mod;
            LOG_I("Reset allow room end, is enable ? [" << m_allowRoomSetEnable << "].");

            LOG_I("Reset white room begin");
            m_whiteRoomSet = tempWhiteRoom;
            LOG_I("Reset white room end.");
        }
    }
	catch (...)
	{
        LOG_E("Update strategy exception.");
		ret = false;
	}

    return ret;
}

bool StrategyManager::isInWhiteList(std::string& roomid)
{
	// ��m_relayRoomSet m_allowRoomSet�Ӷ���
    ReadLock rdlock(m_allRoomSetMutex);

    ROOM_SET::iterator id = m_whiteRoomSet->find(roomid);

    if(id != m_whiteRoomSet->end())
    {
        // �ҵ��˷��䣬����true
        LOG_I("Room id [" << roomid << "] is white room.");
        return true;
    }

	return false;
}

bool StrategyManager::isRelay(std::string& roomid)
{
    // ��m_relayRoomSet m_allowRoomSet�Ӷ���
    ReadLock rdlock(m_allRoomSetMutex);

    // �������˲�������£���roomid�Ƿ���relay room
    if(m_relayRoomSetEnable == 1)
    {
        ROOM_SET::iterator id = m_relayRoomSet->find(roomid);

        if(id != m_relayRoomSet->end())
        {
            // �ҵ��˷��䣬����true
            LOG_I("Room id [" << roomid << "] is going to relay.");
            return true;
        }

		// ����һ�²��ԣ����ȡģ��100����С���趨ֵ���ͻ�Ҷ�
		if(isInMod(roomid, m_relayRoomMod))
		{
			LOG_I("Relay room id [" << roomid << "] is in Mod, going to relay.");
			return true;
		}
    }

    return false;
}

bool StrategyManager::isAllow(std::string& roomid)
{
    // ��m_relayRoomSet m_allowRoomSet�Ӷ���
    ReadLock rdlock(m_allRoomSetMutex);

    // �������˲�������£���roomid�Ƿ���allow room
    if(m_allowRoomSetEnable == 1)
    {
        ROOM_SET::iterator id = m_allowRoomSet->find(roomid);

        if(id != m_allowRoomSet->end())
        {
            // �ҵ��˷��䣬����true
            LOG_I("Room id [" << roomid << "] is allow.");
            return true;
        }

		// ����һ�²��ԣ����ȡģ��100����С���趨ֵ���ͻ�Ҷ�
		if(isInMod(roomid, m_allowRoomMod))
		{
			LOG_I("Room id [" << roomid << "] is in Mod.");
			return true;
		}
    }

    return false;
}

bool StrategyManager::isInMod(std::string& roomid, int mod)
{
	try
	{
		int numberId = boost::lexical_cast <int> (roomid);
		if(numberId % 100 < mod)
		{
			return true;
		}
	}
	catch(...)
	{
	}
	return false;
}

}
}
}
}
