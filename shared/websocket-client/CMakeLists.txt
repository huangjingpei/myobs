cmake_minimum_required(VERSION 3.28...3.30)

add_library(websocket-client INTERFACE)
add_library(OBS::websocket-client ALIAS websocket-client)

# 使用通配符匹配所有源文件和头文件
file(GLOB MONGOOSE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB MONGOOSE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "MONGOOSE_SOURCES: ${MONGOOSE_SOURCES}")


target_sources(
  websocket-client
  INTERFACE
     ${MONGOOSE_SOURCES}
     ${MONGOOSE_SOURCES}
    
)


target_include_directories(websocket-client INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(websocket-client PROPERTIES FOLDER deps)
