find_package(Qt6 REQUIRED Widgets Network Svg Xml)

if(OS_LINUX OR OS_FREEBSD OR OS_OPENBSD)
  find_package(Qt6 REQUIRED Gui DBus)
endif()

if(NOT TARGET OBS::qt-wrappers)
  add_subdirectory("${CMAKE_SOURCE_DIR}/shared/qt/wrappers" "${CMAKE_BINARY_DIR}/shared/qt/wrappers")
endif()

target_link_libraries(
  obs-studio
  PRIVATE Qt::Widgets Qt::Svg Qt::Xml Qt::Network OBS::qt-wrappers
)

set_target_properties(
  obs-studio
  PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
)

set_property(TARGET obs-studio APPEND PROPERTY AUTOUIC_SEARCH_PATHS forms forms/source-toolbar gbs gbs/bizWidgets gbs/naviWidgets)

set(
  _qt_sources
  forms/AutoConfigFinishPage.ui
  forms/AutoConfigStartPage.ui
  forms/AutoConfigStartPage.ui
  forms/AutoConfigStreamPage.ui
  forms/AutoConfigTestPage.ui
  forms/AutoConfigVideoPage.ui
  forms/ColorSelect.ui
  forms/obs.qrc
  forms/OBSAbout.ui
  forms/OBSAdvAudio.ui
  # ע�͵�forms�����OBSBasic, ʹ��gbs Ŀ¼�µ�GBSBasic
  #forms/OBSBasic.ui
  forms/OBSBasic.ui
  forms/OBSBasicControls.ui
  forms/OBSBasicFilters.ui
  forms/OBSBasicInteraction.ui
  forms/OBSBasicProperties.ui
  forms/OBSBasicSettings.ui
  forms/OBSBasicSourceSelect.ui
  forms/OBSBasicTransform.ui
  forms/OBSBasicVCamConfig.ui
  forms/OBSExtraBrowsers.ui
  forms/OBSImporter.ui
  forms/OBSLogReply.ui
  forms/OBSLogViewer.ui
  forms/OBSMissingFiles.ui
  forms/OBSRemux.ui
  forms/OBSUpdate.ui
  forms/OBSYoutubeActions.ui
  forms/source-toolbar/browser-source-toolbar.ui
  forms/source-toolbar/color-source-toolbar.ui
  forms/source-toolbar/device-select-toolbar.ui
  forms/source-toolbar/game-capture-toolbar.ui
  forms/source-toolbar/image-source-toolbar.ui
  forms/source-toolbar/media-controls.ui
  forms/source-toolbar/text-source-toolbar.ui
)


set(
  _qt_sources1
  gbs/naviWidgets/GBSNaviProduct.ui
  gbs/naviWidgets/GBSNaviAI.ui
  gbs/naviWidgets/GBSNaviTranslate.ui
  gbs/naviWidgets/GBSNaviSetting.ui
  gbs/naviWidgets/GBSNaviLive.ui
  gbs/naviWidgets/GBSNaviData.ui

  gbs/bizWidgets/GBSBizDeviceInfo.ui
  gbs/bizWidgets/GBSBizLiveGuarder.ui
  gbs/bizWidgets/GBSBizSoYoung.ui
  gbs/bizWidgets/GBSBizLiveDanmaku.ui
  gbs/bizWidgets/GBSBizLiveBridger.ui
  gbs/bizWidgets/GBSBizLivePusher.ui
  gbs/bizWidgets/GBSBizLiveBroker.ui
  gbs/bizWidgets/GBSBizAIData.ui
  gbs/bizWidgets/GBSBizEShopData.ui

  gbs/bizWidgets/GBSBizSettingAV.ui
  gbs/bizWidgets/GBSBizSettingOutput.ui
  gbs/bizWidgets/GBSBizSettingBasic.ui
  gbs/bizWidgets/GBSBizSettingPage.ui
  gbs/bizWidgets/GBSBizSettingProductDupRM.ui
  gbs/bizWidgets/GBSBizSettingTimbreSquare.ui
  gbs/bizWidgets/GBSBizSettingBridgerMgr.ui
  gbs/bizWidgets/GBSBizSettingLiveEditor.ui
  gbs/bizWidgets/GBSBizSettingLiveSourceDupRM.ui

  gbs/bizWidgets/GBSBizLiveGuarderCtrl.ui
  gbs/bizWidgets/GBSBizLiveGuarderAuth.ui
  gbs/bizWidgets/GBSBizLivePusherAuth.ui
  gbs/bizWidgets/GBSBizLivePusherCtrl.ui


  gbs/bizWidgets/GBSAddBroker.ui
  gbs/bizWidgets/GBSModifyDevice.ui
  gbs/bizWidgets/GBSRemoveBroker.ui
  gbs/bizWidgets/GBSActivateDevice.ui

  gbs/bizWidgets/GBSMsgDialog.ui

  gbs/GBSMainBizWindow.ui
  gbs/GBSNormalLoginForm.ui
  gbs/GBSQRCodeLoginForm.ui
  gbs/GBSAuthorizedCodeForm.ui
  gbs/GBSMainForm.ui
  gbs/GBSRegisterForm.ui
  gbs/GBSMainProfile.ui

)




source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/forms" PREFIX "UI Files/forms" FILES ${_qt_sources})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/gbs" PREFIX "UI Files/gbs" FILES ${_qt_sources1})

set(_qt_sources2 ${_qt_sources} ${_qt_sources1})

target_sources(obs-studio PRIVATE ${_qt_sources2})

unset(_qt_sources)
unset(_qt_sources1)
unset(_qt_sources2)

