#include <kugou/httpproxy/httpserver/roommanager/RoomManager.h>
#include <kugou/base/facility/log.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace roommanager
{

bool RoomManager::publicRoom(int id, int sex)
{
    SharedRoomInfo roomInfo;
    {
        // 如果要添加的点歌已经在列表里，并且还没有成交，就替换掉
        boost::mutex::scoped_lock scopedLock(m_mutexRoomMap);
        IntROOMInfoUMap::iterator it= this->m_roomMap.find(id);
        if(it != this->m_roomMap.end())
        {
            // 找到了旧的房间，就公开这个房间
            roomInfo = it->second;

            if(roomInfo->getStatus() != INIT)
            {
                // 如果这个房间已经是PUBLIC或者CON，就是已经public了，不用再入队了
                LOG_D("RoomID [" << roomInfo->m_roomid << "] has been in list.");
                roomInfo->updateStatus(PUBLIC);
                return true;
            }

            roomInfo->updateStatus(PUBLIC);
        }
        else
        {
            // 找不到房间，直接加入map
            roomInfo.reset(new RoomInfo(id, sex));
            this->m_roomMap.insert(IntROOMInfoUMap::value_type(id, roomInfo));
        }
    }

#ifdef  SEPARATE_QUEUE
    {
        // 男性用户放入male队列，女性用户放入female队列
        if(sex == FEMALE)
        {
            boost::mutex::scoped_lock scopedLock(m_mutexFemaleRoomQueue);
            m_femaleRoomQueue.push(roomInfo);
            LOG_T("Public RoomID [" << roomInfo->m_roomid << "] into female queue.");
        }
        else if(sex == MALE)
        {
            boost::mutex::scoped_lock scopedLock(m_mutexMaleRoomQueue);
            m_maleRoomQueue.push(roomInfo);
            LOG_T("Public RoomID [" << roomInfo->m_roomid << "] into male queue.");
        }
    }
#else
    {
        boost::mutex::scoped_lock scopedLock(m_mutexRoomList);
        m_roomList.push_back(roomInfo);
        LOG_T("Public RoomID [" << roomInfo->m_roomid << "] into list.");
    }
#endif
    
    return true;
}

bool RoomManager::concealRoom(int id)
{
    {
        // 不真正地删除这个房间，只是把它的状态置为删除状态
        boost::mutex::scoped_lock scopedLock(m_mutexRoomMap);
        IntROOMInfoUMap::iterator it= this->m_roomMap.find(id);
        if(it != this->m_roomMap.end())
        {
            if(it->second->getStatus() == PUBLIC)
            {
                it->second->updateStatus(CONCEAL);
            }
            LOG_T("Conceal RoomID [" << it->second->m_roomid << "].");
            return true;
        }
    }

    return false;
}

int  RoomManager::getRoom(int sex, int lastId)
{
#ifdef  SEPARATE_QUEUE
    if(sex == FEMALE || sex == ALL)
    {
        boost::mutex::scoped_lock scopedLock(m_mutexFemaleRoomQueue);

        SharedRoomInfo roomInfo;
        while(!m_femaleRoomQueue.empty())
        {
            roomInfo = m_femaleRoomQueue.front();
            m_femaleRoomQueue.pop();
            if(roomInfo->getStatus() == PUBLIC)
            {
                // 如果找到当前队头房间是public，返回这个房间号，并再一次进入队末
                m_femaleRoomQueue.push(roomInfo);
                LOG_T("get RoomID [" << roomInfo->m_roomid << "] for Sex [" << sex << "].");
                return roomInfo->m_roomid;
            }
            // else 如果这个房间状态不是public，就不再进入队末了，相当于出队
        }  
    }

    if(sex == MALE || sex == ALL)
    {
        boost::mutex::scoped_lock scopedLock(m_mutexMaleRoomQueue);

        SharedRoomInfo roomInfo;
        while(!m_maleRoomQueue.empty())
        {
            roomInfo = m_maleRoomQueue.front();
            m_maleRoomQueue.pop();
            if(roomInfo->getStatus() == PUBLIC)
            {
                // 如果找到当前队头房间是public，返回这个房间号，并再一次进入队末
                m_maleRoomQueue.push(roomInfo);
                LOG_T("get RoomID [" << roomInfo->m_roomid << "] for Sex [" << sex << "].");
                return roomInfo->m_roomid;
            }
            // else 如果这个房间状态不是public，就不再进入队末了，相当于出队
        }  
    }
#else
    boost::mutex::scoped_lock scopedLock(m_mutexRoomList);

    SharedRoomInfo roomInfo;
    std::list<SharedRoomInfo>::iterator it = m_roomList.begin();
    while(it != m_roomList.end())
    {
        if(sex == ALL || (*it)->m_sex == sex)
        {
            // 找到相应性别的房间
            SharedRoomInfo roomInfo = *it;
                
            // 如果找到的房间和它上一次取得的房间一样，忽略这一次查找
            if(roomInfo->m_roomid == lastId)
            {
                ++it;
                continue;
            }

            // 先把这个房间从删除，再看它是PUBLIC就回队列，否则直接出去了。
            m_roomList.erase(it++);

            //如果是为public状态，才取，否则不再回到队列
            if(roomInfo->getStatus() == PUBLIC)
            {
                m_roomList.push_back(roomInfo);
                return roomInfo->m_roomid;
            }
            else
            {
                // 不是PUBLIC的都要出队，并把状态置成INIT
                roomInfo->updateStatus(INIT);
            }
        }
        else
        {
            ++it;
        }
    }
#endif

    LOG_T("No available room for Sex [" << sex << "].");

    return -1;
}

long long RoomManager::getRoomSize(void)
{
    boost::mutex::scoped_lock scopedLock(m_mutexRoomMap);
    return this->m_roomMap.size();
}

}
}
}
}
