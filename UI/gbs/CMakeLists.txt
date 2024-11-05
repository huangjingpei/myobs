cmake_minimum_required(VERSION 3.16)

project(gbs VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        pixmapoverlay.h
        pixmapoverlay.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gbs
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Resources.qrc
        GBSMainWindow.h GBSMainWindow.cpp GBSMainWindow.ui
        GBSMainForm.h GBSMainForm.cpp GBSMainForm.ui
        GBSNormalLoginForm.h GBSNormalLoginForm.cpp GBSNormalLoginForm.ui
        GBSRegisterForm.h GBSRegisterForm.cpp GBSRegisterForm.ui
        GBSAuthorizedCodeForm.h GBSAuthorizedCodeForm.cpp GBSAuthorizedCodeForm.ui
        GBSQRCodeLoginForm.h GBSQRCodeLoginForm.cpp GBSQRCodeLoginForm.ui
        GBSMainBizWindow.h GBSMainBizWindow.cpp GBSMainBizWindow.ui
        toggleswitch.h toggleswitch.cpp
        naviWidgets/gbsnavilive.h naviWidgets/gbsnavilive.cpp naviWidgets/gbsnavilive.ui
        naviWidgets/gbsnavidata.h naviWidgets/gbsnavidata.cpp naviWidgets/gbsnavidata.ui
        naviWidgets/gbsnaviproduct.h naviWidgets/gbsnaviproduct.cpp naviWidgets/gbsnaviproduct.ui
        naviWidgets/gbsnavisetting.h naviWidgets/gbsnavisetting.cpp naviWidgets/gbsnavisetting.ui
        naviWidgets/GBSNaviTranslate.h naviWidgets/GBSNaviTranslate.cpp naviWidgets/GBSNaviTranslate.ui
        naviWidgets/gbsnaviai.h naviWidgets/gbsnaviai.cpp naviWidgets/gbsnaviai.ui
        bizWidgets/GBSBizDeviceInfo.h bizWidgets/GBSBizDeviceInfo.cpp bizWidgets/GBSBizDeviceInfo.ui
        bizWidgets/gbsbizeshopdata.h bizWidgets/gbsbizeshopdata.cpp bizWidgets/gbsbizeshopdata.ui
        bizWidgets/GBSBizAIData.h bizWidgets/GBSBizAIData.cpp bizWidgets/GBSBizAIData.ui
        bizWidgets/GBSBizSoYoung.h bizWidgets/GBSBizSoYoung.cpp bizWidgets/GBSBizSoYoung.ui
        common/DanmakuWidget.h
        common/VertNaviButton.h
        common/VertNaviButtonExt.h
        common/FatButton.h
        common/VolumeHBarWidget.h
        common/ToggleButton.h
        common/PlayerProgressBar.h
        testUI/testVertNaviButton.h
        testUI/testVolumeHBarWidget.h
        testUI/testToggleButton.h
        testui/testPlayerProgressBar.h
        testUI/testdanmakuwidget.h
        bizWidgets/gbsbizlivepusher.h bizWidgets/gbsbizlivepusher.cpp bizWidgets/gbsbizlivepusher.ui
        bizWidgets/GBSBizLiveBridger.h bizWidgets/GBSBizLiveBridger.cpp bizWidgets/GBSBizLiveBridger.ui
        bizWidgets/GBSBizLiveDanmaku.h bizWidgets/GBSBizLiveDanmaku.cpp bizWidgets/GBSBizLiveDanmaku.ui
        bizWidgets/GBSBizLiveGuarder.h bizWidgets/GBSBizLiveGuarder.cpp bizWidgets/GBSBizLiveGuarder.ui
        bizWidgets/GBSBizSettingBasic.h bizWidgets/GBSBizSettingBasic.cpp bizWidgets/GBSBizSettingBasic.ui
        bizWidgets/GBSBizSettingOutput.h bizWidgets/GBSBizSettingOutput.cpp bizWidgets/GBSBizSettingOutput.ui
        bizWidgets/GBSBizSettingAV.h bizWidgets/GBSBizSettingAV.cpp bizWidgets/GBSBizSettingAV.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET gbs APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(gbs SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(gbs
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(gbs PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gbs)
endif()
set_target_properties(gbs PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gbs
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gbs)
endif()
