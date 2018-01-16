#include <boost/lexical_cast.hpp>

#include <kugou/httpproxy/tokenserver/handler/WhiteListXml.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/tokenserver/SystemConfig.h>

#include "../tinyxml/tinyxml.h"

using namespace kugou::base::facility;

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

Poco::SingletonHolder<WhiteListXml> WhiteListXml::m_instance;

WhiteListXml::WhiteListXml(void)
{
	if (pthread_rwlock_init(&m_rwLockXMLCont, NULL) != 0)
	{
		LOG_E("Error: pthread_rwlock_init(&m_rwLockXMLCont, NULL) != 0");
	}

	loadWhiteListFromXML();
}

WhiteListXml::~WhiteListXml()
{
	pthread_rwlock_destroy(&m_rwLockXMLCont);
}

WhiteListXml* WhiteListXml::getInstance(void)
{
	return m_instance.get();
}

std::string WhiteListXml::getWhiteListContent()
{
	ScopeRWLock lock(&m_rwLockXMLCont);
	return m_strXMLContent;
}

void WhiteListXml::setWhiteListContent(const std::string &strContent)
{
	ScopeRWLock lock(&m_rwLockXMLCont, WRITE_LOCK);
	m_strXMLContent = strContent;
}


bool WhiteListXml::loadWhiteListFromXML(void)
{
	std::string strFileName = kugou::httpproxy::tokenserver::SystemConfig::instance().getWhiteListFileName();
	
	if (strFileName.empty())
	{
		return false;
	}
	
	TiXmlDocument doc(strFileName);//打开一个xml文件

	if (!doc.LoadFile())
	{
		LOG_E("Failed to Load XML File, please check file [" << strFileName << "]");
		return false;
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

#ifdef TIXML_USE_STL
	setWhiteListContent(printer.Str());
#else
	std::string strXmlContent = printer.CStr();
	setWhiteListContent(strXmlContent);
#endif
	
	LOG_D("xml_file= " << getWhiteListContent());

	return true;
}

bool WhiteListXml::getWhiteList(std::string &strWhiteList)
{
	strWhiteList = getWhiteListContent();	
	return true;
}


/****************************************************
 *
 ***************************************************/
ScopeRWLock::ScopeRWLock(pthread_rwlock_t *pLock, RWLOCK_TYPE type/*=READ_LOCK*/)
{
	m_rwLock = pLock;
	m_actionType = type;
	
	if (m_rwLock)
	{
		lock();
	}
}

ScopeRWLock::~ScopeRWLock()
{
	unlock();
}

void ScopeRWLock::lock()
{
	if (NULL == m_rwLock)
	{
		return;
	}
	
	if (READ_LOCK == m_actionType)
	{
		pthread_rwlock_rdlock(m_rwLock);
	}
	else// (WRITE_LOCK == actionType)
	{
		pthread_rwlock_wrlock(m_rwLock);
	}
}

void ScopeRWLock::unlock(void)
{
	if (NULL == m_rwLock)
	{
		return;
	}
	
	pthread_rwlock_unlock(m_rwLock);
}

}
}
}
}
