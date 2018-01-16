#ifndef EXCEPTION_FACILITY_BASE_KUGOU_H
#define EXCEPTION_FACILITY_BASE_KUGOU_H

#include <exception>
#include <boost/thread/mutex.hpp>

#if defined WIN32
#include <windows.h>
#include <dbghelp.h>
#elif defined __linux__
#include <execinfo.h>
#include <signal.h>
#elif defined __sun__
#include <ucontext.h>
#include <dlfcn.h>
#else
#error Platform required.
#endif

namespace kugou
{
namespace base
{
namespace facility
{

class Exception : public std::exception
{
public:
    Exception();
    Exception(const std::string &what);
    virtual ~Exception() throw();

    std::string getStackTrace(bool skipSelfStack = true);

    virtual const char *what() const throw();

protected:
    void traceStack(std::ostream &out);
    Exception &operator = (Exception const &);

#if defined WIN32
    void walkStack(HANDLE thread, CONTEXT &context);
    void showFrame(STACKFRAME64 &frame);

    HANDLE m_process;
#elif defined __sun__
    static int showFrame(uintptr_t pc, int signal, void *useragr);
#endif

    std::ostream *m_out;
    std::string m_stack;
    const std::string m_what;
    int m_depth;
    static int m_skip;
    static boost::mutex m_mutex;
private:
    std::string parse_symbol(const char* symbol);
};


#define EXCEPTION(xException, baseException) \
    class xException \
        : public baseException \
    { \
    public: \
        xException() \
            : baseException()\
        {++m_depth;} \
         \
        xException(const std::string &what) \
            : baseException(what) \
        {++m_depth;} \
    } /* ���ⶪʧ�ֺţ��������û�������ӷֺţ��궨��ͺ����ĸ�ʽһ���ˡ� */

// Ԥ�����쳣��
EXCEPTION(OutOfBoundException, Exception);              // ����Խ�磬�ȡ���
EXCEPTION(ItemNotFoundException, Exception);            // ��vector��list���Զ��������У�û���ҵ�����Ŀ���ȡ���
EXCEPTION(NotImplementedException, Exception);          // ���ܻ�δʵ�֡�
EXCEPTION(NullPointerException, Exception);             // �����ÿ�ָ�롣
EXCEPTION(DereferenceFailException, Exception);         // ������ʧ�ܡ�
EXCEPTION(FileNotOpenException, Exception);             // �ļ�û�д򿪣�δ�ҵ���û��Ȩ�ޡ�������

}
}
}

#endif // EXCEPTION_FACILITY_BASE_KUGOU_H
