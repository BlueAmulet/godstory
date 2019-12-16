#ifndef _BACKGROUNDLOAD_H_
#define _BACKGROUNDLOAD_H_

#include "platform/threads/semaphore.h"
#include <string>

typedef void *HANDLE;

/************************************************************************/
/* 重载CBackgroundLoad来实现对象的异步载入
/************************************************************************/
class CBackgroundLoad
{
public:
	enum LoadStatus
	{
		IDLE = 0,
		LOADING,
		CANCELLING,
		PAUSING,
		PAUSED
	};

	CBackgroundLoad(void);

	virtual ~CBackgroundLoad(void);

	/************************************************************************/
	/* 开始异步载入
	/************************************************************************/
	void	BackgroundLoad(const char* name);

	LoadStatus GetStatus(void);

	bool	Finish(void);
	void	Cancel(void);
	void	Pause(void);
	void	Resume(void);

    void    SetFinished(void) {m_isSuccess = true;}

	/************************************************************************/
	/* 重载以加载资源
	/************************************************************************/
	virtual bool OnLoad(const char* name) = 0;
	
	/************************************************************************/
	/* 在加载完成后，此时仍然是异步的
	/************************************************************************/
	virtual void OnLoadComplete(bool isSuccess) {}
private:
	static unsigned long __WorkThread(void* pThis);
	void		 __OnBackgroundLoad(void);

	void		 __Clean(void);

	Semaphore   m_semaphore;
	HANDLE      m_thread;
	LoadStatus  m_status;
	bool	    m_isSuccess;
	std::string m_loadFileName;
};

#endif /*_BACKGROUNDLOAD_H_*/