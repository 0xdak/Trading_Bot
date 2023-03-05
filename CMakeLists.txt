cmake_minimum_required(VERSION 3.5)

project(Trading_Bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lcurl")
set(CURL_LIBRARY "C:\\Users\\alida\\OneDrive\\Belgeler\\Trading_Bot\\libs\\curl\\bin\\libcurl-x64.dll") # normalde linux'da .lib dosyasi
set(CURL_INCLUDE_DIR "C:\\Users\\alida\\OneDrive\\Belgeler\\Trading_Bot\\libs\\curl\\include")
#add_subdirectory(libs)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(Trading_Bot main.cpp endpointhandler.cpp endpointhandler.h)

target_link_libraries(Trading_Bot ${CURL_LIBRARIES})

install(TARGETS Trading_Bot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
