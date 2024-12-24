cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(obs-studio VERSION ${OBS_VERSION_CANONICAL})

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows/architecture.cmake")
  if(NOT OBS_PARENT_ARCHITECTURE STREQUAL CMAKE_VS_PLATFORM_NAME)
    return()
  endif()
endif()

include(compilerconfig)
include(defaults)
include(helpers)

option(ENABLE_UI "Enable building with UI (requires Qt)" ON)
option(ENABLE_SCRIPTING "Enable scripting support" ON)
option(ENABLE_HEVC "Enable HEVC encoders" ON)

# 获取当前时间
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

# 将日期和时间传递给代码
add_definitions(-DBUILD_DATE="${BUILD_DATE}")
add_definitions(-DBUILD_TIME="${BUILD_TIME}")

add_subdirectory(libobs)
if(OS_WINDOWS)
  add_subdirectory(libobs-d3d11)
  add_subdirectory(libobs-winrt)
endif()
add_subdirectory(libobs-opengl)
add_subdirectory(plugins)

add_subdirectory(test/test-input)

add_subdirectory(UI)

message_configuration()
