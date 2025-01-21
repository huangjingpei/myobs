#pragma once
#include "TLDownloadTask.h"
#include<winhttp.h>
#include "TLBufferVector.h"

class TLWinHttpDownLoaderEventHandler {
public:
	virtual ~TLWinHttpDownLoaderEventHandler() {};
	virtual void onDownloadPercentage(int32_t percentage) = 0;
	virtual void onDownloadBytes(uint32_t downloadBytes, uint32_t allBytes) = 0;
};

class TLWinHttpDownloader
{
public:
	TLWinHttpDownloader();
	virtual ~TLWinHttpDownloader();

	
	//外部调用接口
	DWORD DownloadToBuffer(TLDownloadTask &task, TLByteBufferVector &bufVec);
	// 下载到一个文件
	DWORD DownloadToFile(TLDownloadTask &task, CString strOutputFile);
	void setEventHandler(TLWinHttpDownLoaderEventHandler *handler) {
		mHandler = handler;
	}


protected:
	HINTERNET hSession = 0;
	HINTERNET hConnect = 0;
	HINTERNET hRequest = 0;
	char *buff;
	TLWinHttpDownLoaderEventHandler *mHandler{nullptr};
	//TLDownloadTask m_task;
protected:
	int GetSleepSecCount(int nTryCount) const;
	int GetBufferSize(const TLDownloadTask &task) const;

	CStringA GenerateRequest(TLDownloadTask &task) const;

	DWORD ConnectServer(const TLDownloadTask &task);
	DWORD DoDownloadToBuffer(TLDownloadTask &task, TLByteBufferVector &bufVec);
	DWORD DoDownloadToBufferInner(TLDownloadTask &task, TLByteBufferVector &bufVec);
	void closeWinHTTP();
};

