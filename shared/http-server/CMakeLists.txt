cmake_minimum_required(VERSION 3.28...3.30)

add_library(http-server INTERFACE)
add_library(OBS::http-server ALIAS http-server)

# 使用通配符匹配所有源文件和头文件
file(GLOB MONGOOSE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB MONGOOSE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "MONGOOSE_SOURCES: ${MONGOOSE_SOURCES}")


target_sources(
  http-server
  INTERFACE
     ${MONGOOSE_SOURCES}
     ${MONGOOSE_SOURCES}
)

target_include_directories(http-server INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(http-server PROPERTIES FOLDER deps)
