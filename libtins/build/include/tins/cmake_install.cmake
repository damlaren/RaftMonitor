# Install script for directory: /home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/tins" TYPE FILE FILES
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ipv6_address.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/loopback.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/endianness.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/cxxstd.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dhcpv6.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dns.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/data_link_type.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/timestamp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/rsn_information.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dhcp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/arp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/pdu_option.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/sll.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/constants.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/hw_address.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/tins.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/packet_sender.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/packet_writer.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/utils.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/icmpv6.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/eapol.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ip.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/config.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/offline_packet_filter.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dot11.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/pdu_allocator.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/llc.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/bootp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/icmp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/pdu.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/small_uint.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ip_address.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ipsec.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/network_interface.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/sniffer.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/internals.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/radiotap.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/address_range.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/udp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/crypto.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ieee802_3.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/exceptions.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/tcp_stream.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/snap.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/pdu_cacher.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/tcp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ipv6.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/pppoe.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/packet.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/rawpdu.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/macros.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dot3.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/stp.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/handshake_capturer.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ppi.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/dot1q.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ip_reassembler.h"
    "/home/elizabeth/Desktop/cs244b_final_project/libtins/include/tins/ethernetII.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Headers")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/elizabeth/Desktop/cs244b_final_project/libtins/build/include/tins/dot11/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

