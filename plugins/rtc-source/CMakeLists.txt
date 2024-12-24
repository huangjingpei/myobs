cmake_minimum_required(VERSION 3.28...3.30)

add_library(rtc-source MODULE)
add_library(OBS::rtc-source ALIAS rtc-source)

target_sources(rtc-source PRIVATE rtc-source.c rtc-source.h)
target_link_libraries(rtc-source PRIVATE OBS::libobs $<$<PLATFORM_ID:Windows>:OBS::w32-pthreads>)

add_library(rtc-pcm-source INTERFACE)
add_library(OBS::rtc-pcm-source ALIAS rtc-pcm-source)

target_sources(rtc-pcm-source INTERFACE rtc-source.c rtc-source.h)
target_link_libraries(rtc-pcm-source INTERFACE OBS::libobs $<$<PLATFORM_ID:Windows>:OBS::w32-pthreads>)

if(OS_WINDOWS)
  configure_file(cmake/windows/obs-module.rc.in rtc-source.rc)
  target_sources(rtc-source PRIVATE rtc-source.rc)
endif()

set_target_properties_obs(rtc-source PROPERTIES FOLDER plugins PREFIX "")



# # 使用通配符匹配所有源文件和头文件
# file(GLOB RTC_PCM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# file(GLOB RTC_PCM_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# message(STATUS "RTC_PCM_SOURCES: ${RTC_PCM_SOURCES}")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")

# target_sources(
#   rtc-pcm-source
#   PRIVATE
#      ${RTC_PCM_SOURCES}
#      ${RTC_PCM_HEADERS}
# )

