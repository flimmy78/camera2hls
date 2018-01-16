#ifndef FIFO_SERVER_THREAD_HANDLE_H
#define FIFO_SERVER_THREAD_HANDLE_H

#include <iostream>
#include <Poco/Runnable.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

class FIFOServerHandle : public Poco::Runnable
{

public:
	FIFOServerHandle(const char *fifo_name = "./fifo",
			int fifo_mode = 0666, char* fifo_reply_dir="/tmp/");
	~FIFOServerHandle();
	
	FILE* initFifoServer(void);
	bool checkBeforeFifoInit(void);

	virtual void run(void);
	virtual void stop(void);
	virtual bool isRunning(void);

private:
	bool readLine( char *pchBuf, int nMaxSize, FILE *pStream, int &nReadLen);
	bool processFifoCommands(std::string &strCommand, std::string &strRepCont);
	bool checkFifoCommand(char *pchBuf, int nBufLen);
	bool checkFifoCommandFormat(char *pchBuf, int nBufLen);
	bool checkFifoCommandLen(int nBufLen);
	bool trimFromRight(char *pchBuf, int &nBufLen);

	bool setLogLevel(std::string &strCommand);
	bool sendReply(const std::string &strReplyFile, std::string &strRepCont);

private:
	bool m_running;
	int m_fifo_read;
	int m_fifo_write;
	
	std::string m_strFIFOName;
	int m_fifo_mode;
	std::string m_strRepDir;
};

}
}
}
}

#endif //

