#include <iomanip>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/tokenserver/handler/FIFOServerHandle.h>
#include <kugou/httpproxy/tokenserver/handler/WhiteListXml.h>

//#include <kugou/httpproxy/tokenserver/SystemConfig.h>

using namespace kugou::base::facility;
//using namespace kugou::httpproxy::tokenserver;

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{


#define MAX_MI_FIFO_BUFFER 1024
#define MI_CMD_SEPARATOR ':'

static const std::string gs_strCommandLogLevel = "set_log_level";

FIFOServerHandle::FIFOServerHandle(const char *fifo_name,
			int fifo_mode, char* fifo_reply_dir)
	: Runnable()
	, m_running(true)
	, m_fifo_read(-1)
	, m_fifo_write(-1)
	, m_strFIFOName(fifo_name)
	, m_fifo_mode(fifo_mode)
	, m_strRepDir(fifo_reply_dir)
{
}

FIFOServerHandle::~FIFOServerHandle()
{
	//stop();
	
	if (!m_strFIFOName.empty())
	{
		unlink(m_strFIFOName.c_str());
	}

	if (m_fifo_write > 0)
	{
		close(m_fifo_write);
	}

	if (m_fifo_read > 0)
	{
		close(m_fifo_read);
	}
}

void FIFOServerHandle::stop(void)
{
	FILE *pNotifyStream;
	
	m_running = false;
	pNotifyStream = fdopen(m_fifo_write, "w");
	if (NULL == pNotifyStream)
	{
		//LOG_E("FILE stream open failed: " << strerror(errno));
		return;
	}
	
	fputs("\n", pNotifyStream);
	fclose(pNotifyStream);
}

bool FIFOServerHandle::isRunning(void)
{
	return m_running;
}

FILE* FIFOServerHandle::initFifoServer(void)
{
	FILE *pFifoStream;
	long opt;

	/* create FIFO ... */
	if ((mkfifo(m_strFIFOName.c_str(), m_fifo_mode)<0))
	{
		LOG_E("can't create FIFO:" << strerror(errno) << " (mode= " << m_fifo_mode << ")");
		return 0;
	}

	LOG_D("FIFO created @" << m_strFIFOName);

	if ((chmod(m_strFIFOName.c_str(), m_fifo_mode)<0))
	{
		LOG_E("can't chmod FIFO:" << strerror(errno) << " (mode= " << m_fifo_mode << ")");
		return 0;
	}

	LOG_I("fifo:" << m_strFIFOName << " opened, mode=" << m_fifo_mode);

	/* open it non-blocking or else wait here until someone
	 * opens it for writing */
	m_fifo_read = open(m_strFIFOName.c_str(), O_RDONLY|O_NONBLOCK, 0);
	if (m_fifo_read < 0)
	{
		LOG_E("mi_fifo_read did not open: " << strerror(errno));
		return 0;
	}

	pFifoStream = fdopen(m_fifo_read, "r");
	if (NULL == pFifoStream)
	{
		LOG_E("fd open failed: " << strerror(errno));
		return 0;
	}

	/* make sure the read fifo will not close */
	m_fifo_write = open(m_strFIFOName.c_str(), O_WRONLY|O_NONBLOCK, 0);
	if (m_fifo_write < 0)
	{
		LOG_E("fifo_write did not open:" << strerror(errno));
		return 0;
	}

	/* set read fifo blocking mode */
	if ((opt=fcntl(m_fifo_read, F_GETFL))==-1)
	{
		LOG_E("fcntl(F_GETFL) failed: " << strerror(errno) << "[" << errno << "]");
		return 0;
	}
	if (fcntl(m_fifo_read, F_SETFL, opt & (~O_NONBLOCK))==-1)
	{
		LOG_E("cntl(F_GETFL) failed: " << strerror(errno) << "[" << errno << "]");
		return 0;
	}

	return pFifoStream;
}

bool FIFOServerHandle::checkBeforeFifoInit(void)
{
	int ret;
	struct stat filestat;

	if (m_strFIFOName.empty())
	{
		return false;
	}
	LOG_D("m_fifo_name = " << m_strFIFOName);

	ret = stat(m_strFIFOName.c_str(), &filestat);
	if (ret == 0)
	{
		/* FIFO exist, delete it */
		if (unlink(m_strFIFOName.c_str())<0)
		{
			LOG_E("cannot delete old fifo (" << m_strFIFOName.c_str() << "): " << strerror(errno));
			return false;
		}
	}
	else if (ret < 0 && errno != ENOENT)
	{
		LOG_E("FIFO stat failed:" << strerror(errno));
		return false;
	}

	/* checking the mi_fifo_reply_dir param */
	if(m_strRepDir.empty()){
		LOG_E("mi_fifo_reply_dir parameter is empty");
		return false;
	}

	if(stat(m_strRepDir.c_str(), &filestat) < 0)
	{
		LOG_E("directory stat failed:" << strerror(errno));
		return false;
	}
	
	LOG_D(" S_ISDIR ");
	if(S_ISDIR(filestat.st_mode) == 0)
	{
		LOG_E("mi_fifo_reply_dir parameter is not a directory");
		return false;
	}

	/* check m_fifo_mode */
	if(!m_fifo_mode)
	{
		LOG_W("cannot specify mi_fifo_mode = 0, forcing it to rw-------");
		m_fifo_mode = S_IRUSR| S_IWUSR;
	}

	return true;
}

bool FIFOServerHandle::readLine(char *pchBuf, int nMaxSize, FILE *pStream, int &nReadLen)
{
	int nRetry;
	int nLen;

	nRetry = 0;
	
	while (true)
	{
		if (fgets(pchBuf, nMaxSize, pStream) == NULL)
		{
			LOG_E("fifo_server fgets failed:" << strerror(errno));
			if (errno == ESPIPE)
			{
				nRetry++;
				if (nRetry < 4)
				{
					continue;
				}
			}			
			else if ((errno==EINTR)||(errno==EAGAIN))
			{
				LOG_I("fifo_server fgets continue:");
				continue;
			}
		}

		//其他情况，退出
		break;
	}

	nLen = strlen(pchBuf);
	if (nLen && !(pchBuf[nLen-1]=='\n' || pchBuf[nLen-1]=='\r'))
	{
		LOG_E("request line too long");
		return false;
	}

	nReadLen = nLen;

	return true;
}

bool FIFOServerHandle::trimFromRight(char *pchBuf, int &nBufLen)
{
	if (NULL == pchBuf)
	{
		return false;
	}
	
	while(nBufLen)
	{
		if(pchBuf[nBufLen-1] == '\n' || pchBuf[nBufLen-1] == '\r'
			|| pchBuf[nBufLen-1] == ' ' || pchBuf[nBufLen-1] == '\t' )
		{
			nBufLen--;
			pchBuf[nBufLen]=0;
		}
		else
		{
			break;
		}
	}
	
	return true;
}

bool FIFOServerHandle::checkFifoCommandLen(int nBufLen)
{
	if (0 == nBufLen)
	{
		//LOG_D("Command empty\n");
		return false;
	}
	
	if (nBufLen < 3)
	{
		LOG_E("Command must have at least 3 chars\n");
		return false;
	}
	
	return true;
}

bool FIFOServerHandle::checkFifoCommandFormat(char *pchBuf, int nBufLen)
{
	if (NULL == pchBuf)
	{
		LOG_D("Command is NULL\n");
		return false;
	}
	
	if (*pchBuf != MI_CMD_SEPARATOR)
	{
		LOG_E("Command format ':Command:', start at ':'.");
		return false;
	}
	
	char *pchCmdBeg = pchBuf + 1;
	if (NULL == pchCmdBeg)
	{
		return false;
	}
	
	char *pchCmdEnd = strchr(pchCmdBeg, MI_CMD_SEPARATOR);
	if (NULL == pchCmdEnd)
	{
		LOG_E("Command format ':Command:', end of cmd ':' ");
		return false;
	}
	
	/* make Command zero-terminated */
	*pchCmdEnd = 0;

	if (pchCmdEnd == pchCmdBeg) //cmd is "::xxx"
	{		
		return false;
	}
	
	return true;
}

bool FIFOServerHandle::checkFifoCommand(char *pchBuf, int nBufLen)
{
	if (!checkFifoCommandLen(nBufLen))
	{
		return false;
	}

	if (!checkFifoCommandFormat(pchBuf, nBufLen))
	{
		return false;
	}

	return true;
}

/* reply fifo security checks:
 * checks if fd is a fifo, is not hardlinked and it's not a softlink
 * opened file descriptor + file name (for soft link check)
 * returns 0 if ok, <0 if not */
static int fifo_check(int fd, const char* fname)
{
	struct stat fst;
	struct stat lst;
	
	if (fstat(fd, &fst)<0){
		//LM_ERR("fstat failed: %s\n", strerror(errno));
		return -1;
	}
	/* check if fifo */
	if (!S_ISFIFO(fst.st_mode)){
		LOG_R_E("It is not a fifo [" << fname << "]");
		return -1;
	}
	/* check if hard-linked */
	if (fst.st_nlink>1){
		LOG_R_E("security: fifo_check: " << fname << "%s is hard-linked times:" << (unsigned)fst.st_nlink);
		return -1;
	}

	/* lstat to check for soft links */
	if (lstat(fname, &lst)<0){
		//LM_ERR("lstat failed: %s\n", strerror(errno));
		return -1;
	}
	if (S_ISLNK(lst.st_mode)){
		LOG_R_E("security: fifo_check: " << fname << " is a soft link\n");
		return -1;
	}
	/* if this is not a symbolic link, check to see if the inode didn't
	 * change to avoid possible sym.link, rm sym.link & replace w/ fifo race
	 */
	if ((lst.st_dev!=fst.st_dev)||(lst.st_ino!=fst.st_ino)){
		//LOG_R_E("security: fifo_check: inode/dev number differ:" << (int)fst.st_ino << (int)lst.st_ino << fname);
		return -1;
	}
	/* success */
	return 0;
}

static const int FIFO_REPLY_RETRIES = 4;
static FILE *openReplyPipe(const char *pipe_name)
{
	int fifofd;
	FILE *file_handle;
	int flags;
	int retries = FIFO_REPLY_RETRIES;

	if (!pipe_name || *pipe_name==0) {
		LOG_R_D("no file to write to about missing cmd");
		return 0;
	}

tryagain:
	/* open non-blocking to make sure that a broken client will not 
	 * block the FIFO server forever */
	fifofd = open(pipe_name, O_WRONLY | O_NONBLOCK);
	if (fifofd == -1) {
		/* retry several times if client is not yet ready for getting
		   feedback via a reply pipe
		*/
		if (errno==ENXIO) {
			/* give up on the client - we can't afford server blocking */
			if (retries==0) {
				//LM_ERR("no client at %s\n",pipe_name );
				return 0;
			}
			
			usleep(100);
			retries--;
			goto tryagain;
		}
		/* some other opening error */
		//LM_ERR("open error (%s): %s\n", pipe_name, strerror(errno));
		return 0;
	}
	/* security checks: is this really a fifo?, is 
	 * it hardlinked? is it a soft link? */
	if (fifo_check(fifofd, pipe_name)<0) goto error;

	/* we want server blocking for big writes */
	if ( (flags=fcntl(fifofd, F_GETFL, 0))<0) {
		//LM_ERR("pipe (%s): getfl failed: %s\n", pipe_name, strerror(errno));
		goto error;
	}
	flags&=~O_NONBLOCK;
	if (fcntl(fifofd, F_SETFL, flags)<0) {
		//LM_ERR("pipe (%s): setfl cntl failed: %s\n", pipe_name, strerror(errno));
		goto error;
	}

	/* create an I/O stream */
	file_handle=fdopen( fifofd, "w");
	if (file_handle==NULL) {
		//LM_ERR("open error (%s): %s\n", pipe_name, strerror(errno));
		goto error;
	}
	return file_handle;
error:
	close(fifofd);
	return 0;
}


bool FIFOServerHandle::sendReply(const std::string &strReplyFile, std::string &strRepCont)
{
	FILE *pReplyStream = NULL;
	std::string strFilePath(m_strRepDir);
	
	if (strReplyFile.empty())
	{
		return false;
	}

	strFilePath.append(strReplyFile);
	pReplyStream = openReplyPipe(strFilePath.c_str());
	
	if (NULL == pReplyStream)
	{
		LOG_E("Failed to open reply_fifo:" << strFilePath);
		return false;
	}

	fputs(strRepCont.c_str(), pReplyStream);
	fclose(pReplyStream);
	
	return true;
}

void FIFOServerHandle::run(void)
{
	FILE *pFifoStream;
	char szRecvBuf[MAX_MI_FIFO_BUFFER] = "";
	int nLineLen;
	std::string strCmd;
	std::string strReplyFile, strRepCont;
	
	if (!checkBeforeFifoInit())
	{
		LOG_E("check_before_fifo_init");
		return ;
	}

	if ((pFifoStream = initFifoServer()) == NULL)
	{
		LOG_E("init_fifo_server");
		return ;
	}

//	m_running = true;
	int nCmdLen;
	
	while(m_running)
	{
		/* commands must look like this way ':<pchCommand>:[filename]' */
		if (!readLine(szRecvBuf, sizeof(szRecvBuf), pFifoStream, nLineLen))
		{
			LOG_D("failed to read Command\n");
			continue;
		}
		//LOG_D("szRecvBuf = " << szRecvBuf);
		
		trimFromRight(szRecvBuf, nLineLen);
		if (!checkFifoCommand(szRecvBuf, nLineLen))
		{
			continue;
		}

		nCmdLen = strlen(szRecvBuf + 1);
		strCmd.assign(szRecvBuf + 1, nCmdLen);
		
		strReplyFile.assign(szRecvBuf+2+nCmdLen, strlen(szRecvBuf+2+nCmdLen));

		LOG_I("FIFO Command:" << strCmd << " ReplyFile:" << strReplyFile);

		processFifoCommands(strCmd, strRepCont);
		sendReply(strReplyFile, strRepCont);
		strRepCont.clear();
	}//while

	m_running = false;
	unlink(m_strFIFOName.c_str());
	LOG_I("fifo handle stop.");
}

bool FIFOServerHandle::setLogLevel(std::string &strCommand)
{
	size_t nPos;

	nPos = strCommand.find(gs_strCommandLogLevel) ;
	if (std::string::npos == nPos)
	{
		return false;
	}

	nPos = strCommand.find_first_not_of(' ', nPos+gs_strCommandLogLevel.size());
	if (std::string::npos == nPos)
	{
		return false;
	}

	std::string strLevelFlag = strCommand.substr(nPos);

	LOG_I("LevelFlag : " << strLevelFlag);

	if (strLevelFlag == "debug")
	{
		log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getDebug());
	}
	else if (strLevelFlag == "info")
	{
		log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
	}
	else if (strLevelFlag == "error")
	{
		log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getError());
	}
	else
	{
		LOG_W("LevelFlag : " << strLevelFlag << " not support.");
	}
	
	return true;
}

bool FIFOServerHandle::processFifoCommands(std::string &strCommand, std::string &strRepCont)
{
	bool bOK = false;
	std::string test;

	strRepCont = "Command process successful.";
	
	if (strCommand == "reload_whitelist")
	{
		bOK = WhiteListXml::getInstance()->loadWhiteListFromXML();
	}
	else
	{
		LOG_W("Command[" << strCommand << "] not support.");
		
		strRepCont = "Command no found";
		return false;
	}
	
	if (!bOK)
	{
		strRepCont = "Command process failed!";
	}

	return bOK;
}

}
}
}
}//namespace base

