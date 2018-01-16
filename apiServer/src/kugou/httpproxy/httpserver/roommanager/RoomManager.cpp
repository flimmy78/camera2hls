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
        // ���Ҫ��ӵĵ���Ѿ����б�����һ�û�гɽ������滻��
        boost::mutex::scoped_lock scopedLock(m_mutexRoomMap);
        IntROOMInfoUMap::iterator it= this->m_roomMap.find(id);
        if(it != this->m_roomMap.end())
        {
            // �ҵ��˾ɵķ��䣬�͹����������
            roomInfo = it->second;

            if(roomInfo->getStatus() != INIT)
            {
                // �����������Ѿ���PUBLIC����CON�������Ѿ�public�ˣ������������
                LOG_D("RoomID [" << roomInfo->m_roomid << "] has been in list.");
                roomInfo->updateStatus(PUBLIC);
                return true;
            }

            roomInfo->updateStatus(PUBLIC);
        }
        else
        {
            // �Ҳ������䣬ֱ�Ӽ���map
            roomInfo.reset(new RoomInfo(id, sex));
            this->m_roomMap.insert(IntROOMInfoUMap::value_type(id, roomInfo));
        }
    }

#ifdef  SEPARATE_QUEUE
    {
        // �����û�����male���У�Ů���û�����female����
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
        // ��������ɾ��������䣬ֻ�ǰ�����״̬��Ϊɾ��״̬
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
                // ����ҵ���ǰ��ͷ������public�������������ţ�����һ�ν����ĩ
                m_femaleRoomQueue.push(roomInfo);
                LOG_T("get RoomID [" << roomInfo->m_roomid << "] for Sex [" << sex << "].");
                return roomInfo->m_roomid;
            }
            // else ����������״̬����public���Ͳ��ٽ����ĩ�ˣ��൱�ڳ���
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
                // ����ҵ���ǰ��ͷ������public�������������ţ�����һ�ν����ĩ
                m_maleRoomQueue.push(roomInfo);
                LOG_T("get RoomID [" << roomInfo->m_roomid << "] for Sex [" << sex << "].");
                return roomInfo->m_roomid;
            }
            // else ����������״̬����public���Ͳ��ٽ����ĩ�ˣ��൱�ڳ���
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
            // �ҵ���Ӧ�Ա�ķ���
            SharedRoomInfo roomInfo = *it;
                
            // ����ҵ��ķ��������һ��ȡ�õķ���һ����������һ�β���
            if(roomInfo->m_roomid == lastId)
            {
                ++it;
                continue;
            }

            // �Ȱ���������ɾ�����ٿ�����PUBLIC�ͻض��У�����ֱ�ӳ�ȥ�ˡ�
            m_roomList.erase(it++);

            //�����Ϊpublic״̬����ȡ�������ٻص�����
            if(roomInfo->getStatus() == PUBLIC)
            {
                m_roomList.push_back(roomInfo);
                return roomInfo->m_roomid;
            }
            else
            {
                // ����PUBLIC�Ķ�Ҫ���ӣ�����״̬�ó�INIT
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
