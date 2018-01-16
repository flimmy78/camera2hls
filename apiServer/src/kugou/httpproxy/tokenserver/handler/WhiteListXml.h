#ifndef WhiteListXml_BEAN_TOKENSERVER_HTTPPROXY_KUGOU
#define WhiteListXml_BEAN_TOKENSERVER_HTTPPROXY_KUGOU

#include <string>
#include <Poco/SingletonHolder.h>
#include <boost/shared_ptr.hpp>

#include <kugou/base/net/Handler.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

typedef enum {
	WRITE_LOCK = 0,
	READ_LOCK
}RWLOCK_TYPE;

class ScopeRWLock;

class WhiteListXml
{
public:
	static WhiteListXml* getInstance(void);
	bool getWhiteList(std::string &strWhiteList);
	
	bool loadWhiteListFromXML(void);
	
	std::string getWhiteListContent();
	void setWhiteListContent(const std::string &strContent);
	
private:
    friend class Poco::SingletonHolder<WhiteListXml>;
	static Poco::SingletonHolder<WhiteListXml> m_instance;

	std::string m_strXMLContent;
	pthread_rwlock_t m_rwLockXMLCont;
	
    WhiteListXml(void);
	virtual ~WhiteListXml();
};

class ScopeRWLock
{
public:
	ScopeRWLock(pthread_rwlock_t *pLock, RWLOCK_TYPE type=READ_LOCK);
	~ScopeRWLock();
private:	
	pthread_rwlock_t *m_rwLock;
	RWLOCK_TYPE m_actionType;
	
	void lock();
	void unlock(void);
};


}
}
}
}

#endif
