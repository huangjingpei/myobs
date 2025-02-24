#ifndef LOCALQUERYINFO_H
#define LOCALQUERYINFO_H
#include <QStringList>
#include <windows.h>

// #include <QCoreApplication>

void after_app_run_check();
bool fileExists(const QString &filePath);
bool hasWindowsService(const QString serviceName,DWORD &status);
void remove_rust_config();
void install_rust_application( );
void send_message_to_rust();
void after_app_run_check();
void async_wait_stoptimer();
void heartbeat_timer_init();
void heartbeat_timer_start(int msec);
void restart_rust_service(QString &path);
void connect_remote_machine(QString user, QString password);
#endif // LOCALQUERYINFO_H
