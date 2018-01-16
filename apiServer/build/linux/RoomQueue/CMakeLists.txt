cmake_minimum_required(VERSION 2.6)
PROJECT(roomQueue)

ADD_DEFINITIONS(-DDEBUG)
SET(SRC_PATH ../../../src)
SET(EXECUTABLE_OUTPUT_PATH ../../../bin/linux/RoomQueue/bin)

INCLUDE_DIRECTORIES(${SRC_PATH})
INCLUDE_DIRECTORIES(/home/3rd-party/libevent-2.0.21/include)
INCLUDE_DIRECTORIES(/home/3rd-party/boost/include)
INCLUDE_DIRECTORIES(/home/3rd-party/log4cxx/include)
INCLUDE_DIRECTORIES(/home/3rd-party/json-c-0.9/include)

LINK_DIRECTORIES(/home/3rd-party/boost/lib)
LINK_DIRECTORIES(/home/3rd-party/log4cxx/lib)
LINK_DIRECTORIES(/home/3rd-party/libevent-2.0.21/lib)
LINK_DIRECTORIES(/home/3rd-party/json-c-0.9/lib)

SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/ErrorMsg.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/Demangle.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/Exception.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/Json.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/log.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/SysHealth.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/Thread.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/facility/MD5.cpp)

SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/HTTPServer.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/StreamSocket.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/HTTPConnection.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/HTTPConnectionManager.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/HTTPReply.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/base/net/HTTPRequestHandler.cpp)


SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/roommanager/RoomInfo.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/roommanager/RoomManager.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/roommanager/SystemConfig.cpp)

SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/handler/OrderSongs.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/handler/RoomConceal.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/handler/RoomGet.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/handler/RoomPublic.cpp)
SET(SRC_FILE ${SRC_FILE} ${SRC_PATH}/kugou/httpproxy/httpserver/main.cpp)

ADD_EXECUTABLE(roomQueue ${SRC_FILE})

TARGET_LINK_LIBRARIES(roomQueue log4cxx)
TARGET_LINK_LIBRARIES(roomQueue boost_system)
TARGET_LINK_LIBRARIES(roomQueue boost_thread)
TARGET_LINK_LIBRARIES(roomQueue boost_program_options)
TARGET_LINK_LIBRARIES(roomQueue event)
TARGET_LINK_LIBRARIES(roomQueue json)
