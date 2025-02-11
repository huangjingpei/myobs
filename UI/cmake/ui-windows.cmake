target_sources(
  obs-studio
  PRIVATE
    window-basic-about.cpp
    window-basic-about.hpp
    window-basic-adv-audio.cpp
    window-basic-adv-audio.hpp
    window-basic-auto-config-test.cpp
    window-basic-auto-config.cpp
    window-basic-auto-config.hpp
    window-basic-filters.cpp
    window-basic-filters.hpp
    window-basic-interaction.cpp
    window-basic-interaction.hpp
    window-basic-main-browser.cpp
    window-basic-main-dropfiles.cpp
    window-basic-main-icons.cpp
    window-basic-main-outputs.cpp
    window-basic-main-outputs.hpp
    window-basic-main-profiles.cpp
    window-basic-main-scene-collections.cpp
    window-basic-main-screenshot.cpp
    window-basic-main-transitions.cpp
    window-basic-main.cpp
    window-basic-main.hpp
    window-basic-main-gbs.cpp


    gbs/common/VertNaviButtonExt.h
    gbs/common/VertNaviButton.h
    gbs/common/HoriNaviButton.h
    gbs/common/FatButton.h
    gbs/common/PlayerProgressBar.h
    gbs/common/VolumeHBarWidget.h
    gbs/common/ShadowWidget.h
    gbs/common/ToggleButton.h
    gbs/common/DanmakuWidget.h
    gbs/common/SelectedIDSDialog.h
    gbs/common/EllipticalSlider.h
    gbs/common/EllipticalSliderExt.h
    gbs/common/CountdownDialog.h

    gbs/common/SystemUtils.h
    gbs/common/QIniFile.h
    gbs/common/QBizLogger.h
    gbs/common/QToast.h

    gbs/common/WebSocketClient.cpp
    gbs/common/WebSocketClient.h
    gbs/common/GBSHttpClient.cpp
    gbs/common/GBSHttpClient.h
    gbs/common/DoubleTextButton.h

    gbs/common/FileIOUtils.h
    gbs/common/FileIOUtils.cpp
    
    gbs/common/XORPasswordProtecter.h

    gbs/updater/GBSXMLParser.h
    gbs/updater/GBSXMLParser.cpp
    gbs/updater/GBSFileDownloader.h
    gbs/updater/GBSFileDownloader.cpp

    gbs/updater/winhttp/TLBufferVector.h
    gbs/updater/winhttp/TLBufferVector.cpp
    gbs/updater/winhttp/TLWebDef.h
    gbs/updater/winhttp/TLDownloadTask.h
    gbs/updater/winhttp/TLDownloadTask.cpp
    gbs/updater/winhttp/TLWinHttpDownloader.h
    gbs/updater/winhttp/TLWinHttpDownloader.cpp


    gbs/GBSMainBizWindow.cpp
    gbs/GBSMainBizWindow.h
    gbs/GBSMainForm.cpp
    gbs/GBSAuthorizedCodeForm.cpp
    gbs/GBSNormalLoginForm.cpp
    gbs/GBSQRCodeLoginForm.cpp
    gbs/GBSRegisterForm.cpp
    gbs/GBSMainForm.h
    gbs/GBSRegisterForm.h
    gbs/GBSQRCodeLoginForm.h
    gbs/GBSAuthorizedCodeForm.h
    gbs/GBSNormalLoginForm.h
    gbs/pixmapoverlay.h
    gbs/pixmapoverlay.cpp
  
    gbs/naviWidgets/GBSNaviData.cpp
    gbs/naviWidgets/GBSNaviData.h
    gbs/naviWidgets/GBSNaviAI.cpp
    gbs/naviWidgets/GBSNaviAI.h
    gbs/naviWidgets/GBSNaviProduct.cpp
    gbs/naviWidgets/GBSNaviProduct.h
    gbs/naviWidgets/GBSNaviSetting.h
    gbs/naviWidgets/GBSNaviLive.cpp
    gbs/naviWidgets/GBSNaviLive.h
    gbs/naviWidgets/GBSNaviTranslate.cpp
    gbs/naviWidgets/GBSNaviTranslate.h
    gbs/naviWidgets/GBSNaviSetting.cpp

    gbs/bizWidgets/GBSBizDeviceInfo.cpp
    gbs/bizWidgets/GBSBizDeviceInfo.h
    gbs/bizWidgets/GBSBizAIData.cpp
    gbs/bizWidgets/GBSBizLiveBroker.cpp
    gbs/bizWidgets/GBSBizLiveBridger.cpp
    gbs/bizWidgets/GBSBizSettingAV.cpp
    gbs/bizWidgets/GBSBizSettingAV.h
    gbs/bizWidgets/GBSBizSettingOutput.cpp
    gbs/bizWidgets/GBSBizSettingOutput.h
    gbs/bizWidgets/GBSBizSettingBasic.cpp
    gbs/bizWidgets/GBSBizSettingBasic.h
    gbs/bizWidgets/GBSBizSettingPage.cpp
    gbs/bizWidgets/GBSBizSettingPage.h
    gbs/bizWidgets/GBSBizSettingProductDupRM.h
    gbs/bizWidgets/GBSBizSettingProductDupRM.cpp
    gbs/bizWidgets/GBSBizSettingTimbreSquare.h
    gbs/bizWidgets/GBSBizSettingTimbreSquare.cpp
    gbs/bizWidgets/GBSBizSettingBridgerMgr.h
    gbs/bizWidgets/GBSBizSettingBridgerMgr.cpp
    gbs/bizWidgets/GBSBizSettingLiveEditor.h
    gbs/bizWidgets/GBSBizSettingLiveEditor.cpp
    gbs/bizWidgets/GBSBizSettingLiveSourceDupRM.h
    gbs/bizWidgets/GBSBizSettingLiveSourceDupRM.cpp

    gbs/bizWidgets/GBSBizLiveGuarderCtrl.h
    gbs/bizWidgets/GBSBizLiveGuarderAuth.cpp
    gbs/bizWidgets/GBSBizLiveGuarderAuth.h
    gbs/bizWidgets/GBSBizLiveGuarderCtrl.cpp
    gbs/bizWidgets/GBSBizLivePusherAuth.h
    gbs/bizWidgets/GBSBizLivePusherAuth.cpp
    gbs/bizWidgets/GBSBizLivePusherCtrl.h
    gbs/bizWidgets/GBSBizLivePusherCtrl.cpp

    gbs/bizWidgets/GBSMsgDialog.h
    gbs/bizWidgets/GBSMsgDialog.cpp


    gbs/bizWidgets/GBSBizLivePusher.cpp
    gbs/bizWidgets/GBSBizLiveGuarder.h
    gbs/bizWidgets/GBSBizLiveGuarder.cpp
    gbs/bizWidgets/GBSBizLiveDanmaku.h
    gbs/bizWidgets/GBSBizLiveDanmaku.cpp
    gbs/bizWidgets/GBSBizSoYoung.h
    gbs/bizWidgets/GBSBizSoYoung.cpp
    gbs/bizWidgets/GBSBizLiveBroker.h
    gbs/bizWidgets/GBSBizLiveBridger.h
    gbs/bizWidgets/GBSBizLivePusher.h
    gbs/bizWidgets/GBSBizAIData.h
    gbs/bizWidgets/GBSBizEShopData.cpp
    gbs/bizWidgets/GBSBizEShopData.h
    gbs/bizWidgets/GBSAddBroker.cpp
    gbs/bizWidgets/GBSAddBroker.h
	  gbs/bizWidgets/GBSModifyDevice.cpp
    gbs/bizWidgets/GBSModifyDevice.h
    gbs/bizWidgets/GBSActivateDevice.cpp
    gbs/bizWidgets/GBSActivateDevice.h
    gbs/bizWidgets/GBSRemoveBroker.cpp
    gbs/bizWidgets/GBSRemoveBroker.h
    gbs/bizWidgets/GBSStreamSelector.cpp
    gbs/bizWidgets/GBSStreamSelector.h

    gbs/dto/GBSUserInfo.h
    gbs/dto/GBSUserInfo.cpp
    gbs/dto/GBSRoomInfo.h
    gbs/dto/GBSRoomInfo.cpp
    gbs/dto/GBSMemberInfo.h
    gbs/dto/GBSMemberInfo.cpp
    gbs/dto/GBSQrCodeResponse.h
    gbs/dto/GBSQrCodeResponse.cpp
    gbs/dto/GBSLiveAccountInfo.h
    gbs/dto/GBSLiveAccountInfo.cpp
    gbs/dto/GBSLiveDevices.h
    gbs/dto/GBSLiveDevices.cpp
    gbs/dto/GBSBundleData.h
    gbs/dto/GBSBundleData.cpp
    gbs/dto/GBSPushStreamInfo.h
    gbs/dto/GBSPushStreamInfo.cpp
    gbs/dto/GBSLiveTranscribe.h
    gbs/dto/GBSLiveTranscribe.cpp

    gbs/codec/GBSOpusEncoder.h
    gbs/codec/GBSOpusEncoder.cpp
    gbs/codec/GBSAACEncoder.h
    gbs/codec/GBSAACEncoder.cpp
    gbs/codec/GBSOpusDecoder.h
    gbs/codec/GBSOpusDecoder.cpp
    gbs/codec/GBSAudioCodec.h

    gbs/media/GBSAudioReader.h
    gbs/media/GBSAudioReader.cpp
    gbs/media/GBSAudioWriter.h
    gbs/media/GBSAudioWriter.cpp
    gbs/media/GBSAudioResampler.h
    

    gbs/utils/TaskExecutor.h
    gbs/GBSToolKits.h
    gbs/GBSToolKits.cpp
    gbs/GBSHttpsServer.h
    gbs/GBSHttpsServer.cpp
    gbs/GBSAsioHttpServer.h
    gbs/GBSAsioHttpServer.cpp
    gbs/GBSMainProfile.h
    gbs/GBSMainProfile.cpp
    
    gbs/GBSMainCollector.h
    gbs/GBSMainCollector.cpp


    window-basic-preview.cpp
    window-basic-preview.hpp
    window-basic-properties.cpp
    window-basic-properties.hpp
    window-basic-settings-a11y.cpp
    window-basic-settings-appearance.cpp
    window-basic-settings-stream.cpp
    window-basic-settings-gbs.cpp
    window-basic-settings.cpp
    window-basic-settings.hpp
    window-basic-source-select.cpp
    window-basic-source-select.hpp
    window-basic-stats.cpp
    window-basic-stats.hpp
    window-basic-status-bar.cpp
    window-basic-status-bar.hpp
    window-basic-transform.cpp
    window-basic-transform.hpp
    window-basic-vcam-config.cpp
    window-basic-vcam-config.hpp
    window-basic-vcam.hpp
    window-dock.cpp
    window-dock.hpp
    window-importer.cpp
    window-importer.hpp
    window-log-reply.cpp
    window-log-reply.hpp
    window-main.hpp
    window-missing-files.cpp
    window-missing-files.hpp
    window-namedialog.cpp
    window-namedialog.hpp
    window-projector.cpp
    window-projector.hpp
    window-remux.cpp
    window-remux.hpp
)
target_compile_definitions(
  obs-studio PRIVATE ASIO_STANDALONE 
                        $<$<PLATFORM_ID:Windows>:_WEBSOCKETPP_CPP11_STL_> $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0603>)