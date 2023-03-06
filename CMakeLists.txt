cmake_minimum_required(VERSION 3.5)

project(Trading_Bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CURL_LIBRARY "${CMAKE_SOURCE_DIR}\\libs\\curl\\bin\\libcurl-x64.dll") # normalde linux'da .lib dosyasi
set(CURL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}\\libs\\curl\\include")

find_package(CURL REQUIRED)

find_package(RapidJSON REQUIRED)


include_directories(${CURL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}//libs//rapidjson/include)


add_executable(Trading_Bot main.cpp endpointhandler.cpp endpointhandler.h)

target_link_libraries(Trading_Bot ${CURL_LIBRARIES})

install(TARGETS Trading_Bot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

