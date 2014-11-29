# Install script for directory: /home/dmclaren/project/cs244b_final_project/libtins/include/tins

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
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/offline_packet_filter.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/handshake_capturer.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/packet_writer.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/stp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/macros.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/small_uint.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/utils.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/pdu_allocator.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot3.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/snap.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/bootp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/hw_address.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/crypto.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ip.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/pppoe.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ppi.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/sll.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/arp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ipsec.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/pdu_cacher.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/data_link_type.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/tcp_stream.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ieee802_3.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/tcp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/packet_sender.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/endianness.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/network_interface.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dhcp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/pdu.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/icmp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/cxxstd.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/address_range.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot11.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/timestamp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/sniffer.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/rawpdu.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/tins.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/config.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/rsn_information.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/pdu_option.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/constants.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dns.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/exceptions.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/udp.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/radiotap.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ethernetII.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/eapol.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/internals.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ipv6.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ipv6_address.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/llc.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/packet.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ip_reassembler.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/icmpv6.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/loopback.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dot1q.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/ip_address.h"
    "/home/dmclaren/project/cs244b_final_project/libtins/include/tins/dhcpv6.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Headers")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/dmclaren/project/cs244b_final_project/libtins/build/include/tins/dot11/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

