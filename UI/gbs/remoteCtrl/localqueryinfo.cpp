#include "localqueryinfo.h"
#include <QFile>

#include <qdebug.h>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>


#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>


#include <QDir>
#include <thread>
#include <atomic>

#include <windows.h>
#include <QStandardPaths>

#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/common/QBizLogger.h"
std::atomic<bool> done(false);

static QString remoteUsername{};
static QString remotePpassword{};
void send_auth_info_to_gb_backend(QJsonObject& object);
QTimer heartbeat_timer;
// static std::string s_rustAppString;

void heartbeat_timer_init() {
   // qDebug() << s_rustAppString.length();
	QObject::connect(&heartbeat_timer, &QTimer::timeout, []() { async_wait_stoptimer(); }); // 连接信号和槽
}

void heartbeat_timer_start(int msec){
    done = true;  // 设置标志为 true
    if ( heartbeat_timer.isActive()) {
        heartbeat_timer.stop();
    }

    heartbeat_timer.setSingleShot(true); // 设置定时器为单次触发模式
    heartbeat_timer.start(msec);
}

bool fileExists(const QString &filePath) {
    QFile file(filePath);
    return file.exists();
}

void deleteSandboxFiles(const QString &sandboxPath) {
    QDir dir(sandboxPath);
    if(dir.exists()){
        dir.removeRecursively();
    }
}

bool hasWindowsService(const QString serviceName,DWORD &status) {
    SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) {
        qWarning() << "OpenSCManager failed:" << GetLastError();
        return false;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;
    LPENUM_SERVICE_STATUS_PROCESS services = nullptr;

    // Initial call to determine buffer size
    BOOL success = EnumServicesStatusExW(
        scm,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        nullptr,
        0,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr
        );

    if (!success && GetLastError() == ERROR_MORE_DATA) {
        DWORD bufferSize = bytesNeeded;
        services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(malloc(bufferSize));
        if (!services) {
            qWarning() << "Memory allocation failed";
            CloseServiceHandle(scm);
            return false;
        }

        // Second call with the correct buffer size
        success = EnumServicesStatusExW(
            scm,
            SC_ENUM_PROCESS_INFO,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            reinterpret_cast<LPBYTE>(services),
            bufferSize,
            &bytesNeeded,
            &servicesReturned,
            &resumeHandle,
            nullptr
            );
    }

    if (!success) {
        qWarning() << "EnumServicesStatusEx failed:" << GetLastError();
        free(services);
        CloseServiceHandle(scm);
        return false;
    }

    QString serviceNameLocal ;
    for (DWORD i = 0; i < servicesReturned; ++i) {
        if ( services != NULL &&  services[i].lpServiceName != nullptr) {
            serviceNameLocal = QString::fromWCharArray(services[i].lpServiceName);
        } else {
            serviceNameLocal = "";

        }
        //  QString displayName = QString::fromWCharArray(services[i].lpDisplayName);
        //  SERVICE_STATUS_PROCESS statusProcess = services[i].ServiceStatusProcess;

        if ( serviceName == serviceNameLocal) {
            qInfo() << "Service Name:" << QString::fromWCharArray(services[i].lpServiceName);
            qInfo() << "Display Name:" << QString::fromWCharArray(services[i].lpDisplayName);
            status =  services[i].ServiceStatusProcess.dwCurrentState;
            free(services);
            CloseServiceHandle(scm);

            return true;
        }
/*
        qInfo() << "Service Name:" << QString::fromWCharArray(services[i].lpServiceName);
        qInfo() << "Display Name:" << QString::fromWCharArray(services[i].lpDisplayName);
        qInfo() << "Status:" <<  services[i].ServiceStatusProcess.dwCurrentState;
        qInfo() << "-----------------------------";
*/

    }

    free(services);
    CloseServiceHandle(scm);

    return false;
}

void send_message_to_rust(){
    do {
        QTcpSocket socket;
        socket.connectToHost("127.0.0.1", 8070);
        if (socket.waitForConnected()) {
            // 发送消息到 Rust
            QJsonObject msg;
            msg["devid"] = "devid";
            qDebug() << "Qt 连接上" ;
            QByteArray data = QJsonDocument(msg).toJson();
            socket.write(data);
            socket.waitForBytesWritten();

            // 接收 Rust 回复
            if (socket.waitForReadyRead()) {
                QByteArray response = socket.readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                    QJsonObject jsonObject = jsonDoc.object();//转化为对象
                    //qDebug() << "Received JSON from server:" << jsonObject ;
                    if (!jsonObject["status"].toBool()) {
                        remove_rust_config();
                        install_rust_application();
                        heartbeat_timer_start(10000);
                        qDebug() << "Received JSON from server:" << jsonObject["status"].toBool()<<"   ";
		
                        break;
		    } else {
			    send_auth_info_to_gb_backend(jsonObject);
		    }
                }

                Sleep(6000);
            } else  {
                Sleep(6000);
		    QLogE("RemoteCtrl:连接不上 -----  没回复");
                qDebug() << "连接不上 -----  没回复:" ;

            }
        } else {
		QLogE("RemoteCtrl:连接不上 -----  Rust:");
            qDebug() << "连接不上 -----  Rust:" ;
            Sleep(6000);

        }
    } while (!done);
    QLogE("RemoteCtrl:退出结束");
}

void remove_rust_config ()
{
    /*
    C:\Windows\ServiceProfiles\LocalService\AppData\Roaming\RustDesk\config

    C:\Users\Administrator\AppData\Roaming\RustDesk\config

    C:\ProgramData\RustDesk\config
    */
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Windows Sandbox", QSettings::Registry64Format);
    QString install_dir_path = registry.value("InstallLocation").toString();
    qDebug() << "已安装路径:" << install_dir_path;

#define appDataPath QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) // 本应用程序数据路径 （参考：C:/用户/.../AppData/Roaming/ + 本项目名称 ，大部分情况下也是存储一些配置文件或者缓存文件）
    int lastSlashIndex = appDataPath.lastIndexOf('/');
    if (lastSlashIndex != -1) {
        QString lastFolder = appDataPath.left(lastSlashIndex); // 获取最后一个反斜杠之前的所有内容
        qDebug() << "Last folder:" << lastFolder ;
        QString sandbx =  lastFolder + "/RustDesk";
        deleteSandboxFiles(sandbx);
        int colonPos = lastFolder.indexOf(':'); // 查找":"的位置，通常在盘符后出现
        if (colonPos != -1 ) { // 确保":"后面是"\\"
            QString drive = lastFolder.left(colonPos); // 获取从开始到":"后一个字符的位置，通常为"C:\\"
            qDebug() << "Drive:" << drive;

            QString ws_data = drive + ":/Windows/ServiceProfiles/LocalService/AppData/Roaming/RustDesk";
            deleteSandboxFiles(ws_data);
            QString pro_data = drive + ":/ProgramData/RustDesk";
            deleteSandboxFiles(pro_data);
        }
    }
}


wchar_t* stringToWchar(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wcharStr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wcharStr, len);
    return wcharStr;
}


void install_rust_application() {
    QString currentPath = QDir::currentPath();
    currentPath = currentPath + "/Rust/rustdesk.exe";

    std::string install = currentPath.toStdString() + " --silent-install";
    qDebug() << install;



    // 转换为 wchar_t*
    const wchar_t* wstr_ptr = stringToWchar(install);
    wchar_t cmd[256] ={0} ;
    wcscpy ( cmd, wstr_ptr );

    delete [] wstr_ptr;
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL OK = CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (OK)// check if Process is created
    {
        printf("Applicaiton is running\n");
        printf("PID = %d\n", pi.dwProcessId);
        //Wait forever till Process object is signaled (terminated)
        // DWORD status = WaitForSingleObject(pi.hProcess, INFINITE);
        // if (status == WAIT_OBJECT_0)//The state of the specified object is signaled.

        // {
        //     printf("PID = %d is closed!\n", pi.dwProcessId);
        // }

        CloseHandle(pi.hProcess);//Handles must be explicitly closed if not parent process will hold on to it even if child process is terminated.
        CloseHandle(pi.hThread);
    }
    else
    {
        printf("Application NOT running! \t Error code %d", GetLastError());
    }
}



void async_wait_stoptimer() {
     done = false;
  // 执行异步任务后的操作
    std::thread r(send_message_to_rust);
    r.detach();
}

void after_app_run_check( ) {
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RustDesk", QSettings::Registry64Format);
    QString install_dir_path = registry.value("InstallLocation").toString() + "\\rustdesk.exe";
    qDebug() << "已安装路径:" << install_dir_path;
    //s_rustAppString = install_dir_path.toStdString();

    if (fileExists(install_dir_path)) {
        QString serviceName = "RustDesk";
        DWORD status = 0;
        bool hasService = hasWindowsService(serviceName,status);
        qInfo() << "Status:" <<status << hasService;

        if (hasService) {
            if (status != 4) {
                QProcess *process = new QProcess();
                // 连接到finished信号，该信号在进程结束时发出
                QObject::connect(process, &QProcess::finished, [process](int exitCode, QProcess::ExitStatus exitStatus) {
                    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                        QString output = process->readAllStandardOutput();
                        qDebug() << "Output:" << output;
                        heartbeat_timer_start(8000);
                    } else {
                        qDebug() << "Error:" << process->readAllStandardError();
                    }


                    process->deleteLater(); // 删除进程对象，防止内存泄漏
                });

                process->start("cmd", QStringList() << "/c"  << "net start rustdesk" );
            } else  {
                heartbeat_timer_start(10);
            }
        } else  {
            restart_rust_service(install_dir_path);
             heartbeat_timer_start(8000);
        }

        qDebug() << "文件存在!";
    } else {
        qDebug() << "文件不存在,开始安装......";
        remove_rust_config();
        install_rust_application();
         heartbeat_timer_start(10000);
    }
}

void restart_rust_service(QString &path) {
    std::string install = path.toStdString() + " --re-service";
    qDebug() << install;

    // 转换为 wchar_t*
    const wchar_t* wstr_ptr = stringToWchar(install);
    wchar_t cmd[256] ={0} ;
    wcscpy ( cmd, wstr_ptr );

    delete [] wstr_ptr;

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;


    BOOL OK = CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (OK)// check if Process is created
    {
        printf("Applicaiton is running\n");
        printf("PID = %d\n", pi.dwProcessId);
        //Wait forever till Process object is signaled (terminated)
        // DWORD status = WaitForSingleObject(pi.hProcess, INFINITE);
        // if (status == WAIT_OBJECT_0)//The state of the specified object is signaled.

        // {
        //     printf("PID = %d is closed!\n", pi.dwProcessId);
        // }

        CloseHandle(pi.hProcess);//Handles must be explicitly closed if not parent process will hold on to it even if child process is terminated.
        CloseHandle(pi.hThread);
    }
    else
    {
        printf("Application NOT running! \t Error code %d", GetLastError());
    }

    heartbeat_timer_start(10000);
}

void connect_remote_machine(QString user, QString password) {
	QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RustDesk",
			   QSettings::Registry64Format);
	QString install_dir_path = registry.value("InstallLocation").toString() + "\\rustdesk.exe";

	 std::string install =
		install_dir_path.toStdString() + " --connect " + user.toStdString() + " " + password.toStdString();
	qDebug() << install;

	const wchar_t *wstr_ptr = stringToWchar(install);
	wchar_t cmd[256] = {0};
	wcscpy(cmd, wstr_ptr);

	delete[] wstr_ptr;

	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;

	BOOL OK = CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (OK) // check if Process is created
	{
		printf("Applicaiton is running\n");
		printf("PID = %d\n", pi.dwProcessId);
		//Wait forever till Process object is signaled (terminated)
		// DWORD status = WaitForSingleObject(pi.hProcess, INFINITE);
		// if (status == WAIT_OBJECT_0)//The state of the specified object is signaled.

		// {
		//     printf("PID = %d is closed!\n", pi.dwProcessId);
		// }

		CloseHandle(
			pi.hProcess); //Handles must be explicitly closed if not parent process will hold on to it even if child process is terminated.
		CloseHandle(pi.hThread);
	} else {
		printf("Application NOT running! \t Error code %d", GetLastError());
	}

}

#include "gbs/common/QBizLogger.h"
void send_auth_info_to_gb_backend(QJsonObject &jsonObject) {
	if (!jsonObject["username"].isNull() && !jsonObject["password"].isNull()) {
		QString username = jsonObject["username"].toString();
		QString password = jsonObject["password"].toString();
		GBSMainCollector::getInstance()->setRemoteAuth(username, password);
		if ((remoteUsername != username) || (remotePpassword != password)) {
			if (GBSMainCollector::getInstance()->isLogined()) {
				remoteUsername = username;
				remotePpassword = password;
				GBSLiveAccountInfo info = GBSMainCollector::getInstance()->getAccountInfo();
				std::string deviceName = info.getDeviceName();
				int id = info.getLiveDeviceId();
				std::string livePlatform = info.getLivePlatform();
				std::string notes = info.getNotes();
				std::string platformAccount = info.getPlatformAccount();
				int remoteSwitch = info.getRemoteSwitch();
				if (remoteSwitch) {
					QLogE("RemoteCtrl: Update auth info user: %s, pass: %s.",
					      username.toStdString().c_str(), password.toStdString().c_str());
					GBSHttpClient::getInstance()->modifyZlmLiveDevice(
						deviceName, id, livePlatform, notes, platformAccount, remoteSwitch,
						username.toStdString(), password.toStdString());
				}
			}
		}
	}
}

