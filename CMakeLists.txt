# CMAKE version of Ubuntu 16.04 is 3.5.
cmake_minimum_required(VERSION 3.5)
project(MB1434)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)

#########################################
#                                       #
#        Serial Port Driver             #
#   https://github.com/wjwwood/serial   #
#                                       #
#########################################

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

## Sources
list(APPEND SRC
    lib/serial/serial.cc
    lib/serial/serial.h
    lib/serial/v8stdint.h
    )
if(APPLE)
    # If OSX
    list(APPEND SRC lib/serial/impl/unix.cc)
    list(APPEND SRC lib/serial/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # If unix
    list(APPEND SRC lib/serial/impl/unix.cc)
    list(APPEND SRC lib/serial/impl/list_ports/list_ports_linux.cc)
else()
    # If windows
    set(SRC ${SRC} lib/serial/impl/win.cc )
    set(SRC ${SRC} lib/serial/impl/list_ports/list_ports_win.cc )
endif()

## Add serial library
if(APPLE)
    set(LINKLIBS ${LINKLIBS} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY} )
elseif(UNIX)
    set(LINKLIBS ${LINKLIBS} rt pthread )
else()
    set(LINKLIBS ${LINKLIBS} setupapi )
endif()

#########################################
#                                       #
#          Qt Framework                 #
#                                       #
#########################################

if(UNIX AND NOT APPLE)
  list(APPEND LINKLIBS
    ${PROJECT_SOURCE_DIR}/lib/libQt5Widgets.so.5
    ${PROJECT_SOURCE_DIR}/lib/libQt5Core.so.5
    ${PROJECT_SOURCE_DIR}/lib/libQt5Gui.so.5

    ### X11 ###
    ${PROJECT_SOURCE_DIR}/lib/libQt5XcbQpa.so.5
    ${PROJECT_SOURCE_DIR}/lib/platforms/libqxcb.so
    )

  list(APPEND COMPILE_OPTION -fPIC)

endif()

###############################################

list(APPEND SRC
  main.cpp
  Widget.h
  
  )

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND INCL
	${CMAKE_CURRENT_SOURCE_DIR}/lib
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Qt5
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Qt5/QtCore
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Qt5/QtGui
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Qt5/QtWidgets
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
	${INCL}
  )

target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 14 )

