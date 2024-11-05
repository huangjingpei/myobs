#include <string>
#include <algorithm>
#include <cinttypes>
#include "window-basic-main.hpp"

#include <qt-wrappers.hpp>
#include "gbs/common/FatButton.h"


// 本窗体是为了初始化自己窗口的函数，区别于OBSInit的是: 后者是初始化OBS系统，前者只是简单的初始化一个窗口
// 这里你也可以理解为OBSBasic 及负责了整个除登录页面生命周期意外的所有软件的生命周期

void OBSBasic::OBSInit2() {

	ui->pushButton->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-camera.png",
					":gbs/images/gbs/biz/gbs-live-console-camera-color.png", "摄像头驱动");
	ui->pushButton_2->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-record.png",
					":gbs/images/gbs/biz/gbs-live-console-record-color.png", "录制画面");
	ui->pushButton_3->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-rmdup.png",
					":gbs/images/gbs/biz/gbs-live-console-rmdup-color.png", "源直播去重");
	ui->pushButton_4->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-ai-splite-speech.png",
					":gbs/images/gbs/biz/gbs-live-console-ai-splite-speech-color.png", "大型话术拆分");
	ui->pushButton_5->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-translate.png",
					":gbs/images/gbs/biz/gbs-live-console-translate-color.png", "开启直播翻译");
	ui->pushButton_6->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-danmaku-interaction.png",
					":gbs/images/gbs/biz/gbs-live-console-danmaku-interaction-color.png", "弹幕回复");
}
