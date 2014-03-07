project(qxflow)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

set(CMAKE_AUTOMOC ON)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(qxflow_SRCS main.cpp blockitem.cpp linknode.cpp chartscene.cpp extra_items.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11")
qt4_automoc(${qxflow_SRCS})
add_executable(qxflow ${qxflow_SRCS})
target_link_libraries(qxflow ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} )
install(TARGETS qxflow RUNTIME DESTINATION bin)
