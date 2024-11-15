# 1.  saft mode 注释掉为了防止启动的时候提示安全模式运行第三方插件 obs-app.cpp 
    注释掉 check_safe_mode_sentinel();

# 2.  防止启动的时候提示有新的版本的提示
## EnableAutoUpdates 由true改为false obs-app.cpp
    这里是为了防止自动更新
## window-basic-main.cpp 
	updateCheckThread.reset(new AutoUpdateThread(manualUpdate));
	updateCheckThread->start();



代排查问题 1. 2


