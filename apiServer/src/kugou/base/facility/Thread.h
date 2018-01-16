#ifndef THREAD_FACILITY_BASE_KUGOU_H
#define THREAD_FACILITY_BASE_KUGOU_H

namespace kugou
{
namespace base
{
namespace facility
{
class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual void run() = 0;

    void setRunning(bool running);

    bool running();

private:
    volatile bool m_running;
};

}
}
}

#endif // THREAD_FACILITY_BASE_KUGOU_H
