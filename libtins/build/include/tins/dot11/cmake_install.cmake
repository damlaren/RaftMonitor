# Install script for directory: /home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tins/dot11" TYPE FILE FILES
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_probe.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_data.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_beacon.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_mgmt.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_auth.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_base.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_assoc.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11/dot11_control.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Headers")

