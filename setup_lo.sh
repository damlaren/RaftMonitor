#!/bin/sh

# set up loopback interfaces
# Must be run as su

ARGC=0

if [ $# -lt 1 ]
    then
    ARGC=3 #Default to 3 lo interfaces
    else
    ARGC=$1 #Read number of interfaces from command line if present
fi

while [ $ARGC -gt 0 ]; do
  ifconfig lo:${ARGC} 192.168.2.${ARGC}
  # Prevent the interfaces from overlapping in the address space
  ifconfig lo:${ARGC} netmask 255.255.255.255
  ARGC=$(expr $ARGC - 1)
done
