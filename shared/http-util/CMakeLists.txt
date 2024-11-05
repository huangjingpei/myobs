cmake_minimum_required(VERSION 3.28...3.30)

add_library(http-util INTERFACE)
add_library(OBS::http-util ALIAS http-util)

# 使用通配符匹配所有源文件和头文件
file(GLOB HTTP_UTIL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpr/*.cpp")
file(GLOB HTTP_UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/cpr/*.h")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "HTTP_UTIL_SOURCES: ${HTTP_UTIL_SOURCES}")


target_sources(
  http-util
  INTERFACE
     ${HTTP_UTIL_SOURCES}
     ${HTTP_UTIL_SOURCES}
)

target_include_directories(http-util INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(http-util PROPERTIES FOLDER deps)
