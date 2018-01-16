#ifndef CLOCK_FACILITY_BASE_KUGOU_H
#define CLOCK_FACILITY_BASE_KUGOU_H

#include <boost/thread/mutex.hpp>
//#include <com/alu/base/BaseHeader.h>
#include <kugou/base/facility/Exception.h>
#include <kugou/base/facility/Singleton.h>
//#include <com/alu/base/facility/GlobalMutex.h>
//#include <com/alu/base/facility/Thread.h>
//#include <com/alu/base/debug/rtslog.h>
//#include <com/alu/base/facility/SingletonLongevity.h> //unify lifetime

namespace kugou
{
namespace base
{
namespace facility
{

EXCEPTION(GetSystemClockErrorException, kugou::base::facility::Exception);

class Clock : public kugou::base::facility::Singleton<Clock>
{
    friend class kugou::base::facility::Singleton<Clock>;
    /*
    class Flywheel : public Thread, private boost::noncopyable
    {
    public:
        virtual void run();

        long long m_now;
    };

    typedef ThreadHolder <Flywheel> FlywheelThread;
    typedef Loki::SingletonHolder <FlywheelThread, Loki::CreateUsingNew, com::alu::base::facility::Die2nd,
        Loki::ClassLevelLockable, com::alu::base::facility::GlobalMutex> SFW;
        */

public:
    class EventHandler
    {
    protected:
        virtual ~EventHandler() {}

    public:
        virtual void onTimerChange(long long shift) = 0;
    };

    void setEventHandler(EventHandler *eh);

    void sleepSecond(long long second, bool UTC = false);
    void sleepMillisecond(long long millisecond, bool UTC = false);
    void sleepMicrosecond(long long microsecond, bool UTC = false);
    void sleepNanosecond(long long nanosecond, bool UTC = false);

    long long getSecond();
    long long getMillisecond();
    long long getMicrosecond();
    long long getNanosecond();

    /*
    long long getFastSecond();
    long long getFastMillisecond();
    long long getFastMicrosecond();
    long long getFastNanosecond();
    */

    static time_t timegm(tm *ptm);
    static int gmtime(const time_t *ptime, tm *presult);

    // ����������ֵʱ��ת��ΪUTC�ַ�����ʾ��ʱ�䡣�̰߳�ȫ��
    // ������[nanosecond]����ֵʱ�䣬��λΪ���롣
    // ���أ�UTC�ַ���ʱ�䣬����[20090729T100000Z]�������ؿ��ַ�����
    static std::string getUTCString(const long long &nanosecond);

    // ��������UTC�ַ�����ʾ��ʱ��ת��Ϊ��ֵʱ�䣬�̰߳�ȫ��
    // ������[utcstr]��UTC�ַ���ʱ�䣬����[20090729T100000Z]��
    // ���أ���ֵʱ�䣬��λΪ���룻������-1��
    static long long getUTCTime(const std::string &utcstr);

    long long getMonoTick();                    // ��õ�����������ֵ��

    ~Clock();
private:
    Clock(long long sensitivty = 1000*1000*1000*10);
    
    long long getTime();                        // ���ϵͳ�ľ���ʱ�䡣

    boost::mutex m_mutex;
    long long m_sensitivty;                     // ����ʱ���޸ĵļ�������̶ȡ�
    long long m_lastTick;                       // ��һ�������㡣
    long long m_baseTick;                       // �������㡣
    long long m_baseTime;                       // ʱ����㡣
    long long m_snapTime;                       // ʱ������㡣
    long long m_lastClockTime;                  // ��һ��ʱ��ֵ��
    bool m_flywheelStart;
    long long m_nowSecond;                      // ��ǰʱ���롣
    long long m_nowMillisecond;                 // ��ǰʱ����롣
    long long m_nowMicrosecond;                 // ��ǰʱ��΢�롣
    long long m_nowNanosecond;                  // ��ǰʱ�����롣

    long long m_resumePoint;

    EventHandler *m_eh;
};

}
}
}

#endif
