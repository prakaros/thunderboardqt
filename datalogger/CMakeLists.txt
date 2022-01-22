cmake_minimum_required(VERSION 3.2.0)

set(TARGET_NAME datalogger)

set(${TARGET_NAME}_IDE_FOLDER "datalogger")

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QT_PATH "C:/Qt/Qt5.13.0/5.13.0/msvc2015_64")

set(QT_CMAKE_PATH "${QT_PATH}/lib/cmake")
set(QT_HOME $(QT_PATH))
set(CMAKE_MODULE_PATH  "${QT_PATH}/lib/cmake;${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH  "${QT_CMAKE_PATH}")

set(QT_INCLUDES "${QT_PATH}")
##include_directories(${Qt5Core_INCLUDE_DIRS})
##include_directories(${Qt5Gui_INCLUDE_DIRS})
##include_directories(${Qt5Widgets_INCLUDE_DIRS})

message(STATUS QT_PATH  = ${QT_PATH})
message(STATUS CMAKE_MODULE_PATH  = ${CMAKE_MODULE_PATH})

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5SerialPort REQUIRED)

set(${TARGET_NAME}_DIRS
	${QT_INCLUDES}
)

set(${TARGET_NAME}_SRCS
    ui_qt_thuderboard_sense.h
    mainwindow.hpp
    applicationtypes.hpp
    main.cpp
    mainwindow.cpp
    connectionmanager.hpp
    connectionmanagervcom.hpp
    connectionmanagervcom.cpp
    device.hpp
    vcomdevice.hpp
    vcomdevice.cpp
    messageformat.hpp
    messageformat.cpp
    cmdinterface.hpp
    cmdresponse.hpp
    cmdresponse.cpp
    capturethread.hpp
    capturethread.cpp
    dataplotter.hpp
    dataplotter.cpp
    sensemonstringlistmodel.hpp
    sensemonstringlistmodel.cpp
)


set(${TARGET_NAME}_UIS
	qt_thuderboard_sense.ui
)

source_group("QT Ui" FILES ${${TARGET_NAME}_UIS})

add_executable(${TARGET_NAME} ${${TARGET_NAME}_SRCS}
)

target_link_libraries(${TARGET_NAME} Qt5::Core)
target_link_libraries(${TARGET_NAME} Qt5::Gui)
target_link_libraries(${TARGET_NAME} Qt5::Widgets)
target_link_libraries(${TARGET_NAME} Qt5::SerialPort)





