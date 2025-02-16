#ifndef UPDATER_WEB_TLWEBDEF_H
#define UPDATER_WEB_TLWEBDEF_H
#include <atlstr.h>


#define WM_UPDATER_PROGRESS_VALUE     (WM_USER + 530)
#define WM_UPDATER_PROGRESS_STEP      (WM_USER + 1027)
#define WM_UPDATER_UPDATE_VERSION     (WM_USER + 999)

typedef struct tagTLProgressData {
	CString m_strProgress;
	BOOL m_bFinished;
	BOOL m_bEnableCancel;
	tagTLProgressData(CString strProgress, BOOL bFinished, BOOL bEnableCancel);
} TLProgressData;

extern const LPCTSTR UPDATE_AGENT;

typedef enum tagTLEUpdateError {
	THE_SUCCEED = 0,
	THE_VERSION_LATEST = 1,
	THE_IN_PROGRESS,
	THE_USER_CANCELED1,
	THE_USER_CANCELED2,
	THE_USER_CANCELED3,
	THE_REQUEST_CONFIG,
	THE_GET_DOWNLOAD_URL,
	THE_REDIRECT,
	THE_CREATE_SOCKET,
	THE_GET_HOST_BY_NAME,
	THE_CONNECT_SOCKET,
	THE_SET_SOCK_OPT1,
	THE_SET_SOCK_OPT2,
	THE_SEND_HTTP_HEADER,
	THE_INVALID_RECV_END,
	THE_NO_CONTENT_LENGTH,
	THE_NO_REDIRECT_LOCATION,
	THE_REDIRECT_INVALID_FORMAT,
	THE_INVALID_STATUS_CODE,
	THE_RECV_FAIL,
	THE_CREATE_FILE,
	THE_WRITE_FILE,
	THE_MD5_CHECK_FAIL,
	THE_USER_CANCEL_INSTALL,
	THE_RUN_INSTALLER,
	THE_GET_INSTALL_DIR,
	THE_RUN_PROGRAM,
	_C_ERROR_COUNT
} TLEUpdateError;

void SendPingback(LPCTSTR szPingback);

void SendStartupPingaback();
void SendEndPingback(int nRet);

#endif
