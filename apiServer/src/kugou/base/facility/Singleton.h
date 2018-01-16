#ifndef SINGLETON_FACILITY_BASE_KUGOU_H
#define SINGLETON_FACILITY_BASE_KUGOU_H

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>

namespace kugou
{
namespace base
{
namespace facility
{

template <class T>
class Singleton : private boost::noncopyable
{
public:
    static T& instance()
    {
        boost::call_once(init, flag);
        return *t;
    }

    static void init()
    {
        t.reset(new T());
    }

protected:
    ~Singleton() {}
    Singleton() {}

private:
   static boost::scoped_ptr <T> t;
   static boost::once_flag flag;
};

template <class T> boost::scoped_ptr<T> Singleton<T>::t(0);     //��ʼ��
template <class T> boost::once_flag Singleton<T>::flag = BOOST_ONCE_INIT; //����flagΪʵ��call_once

}
}
}

#endif
