#!/bin/sh

# shutdown loopback interfaces
# Must be run as su

ARGC=0

if [ $# -lt 1 ]
    then
    ARGC=3 #Default to 3 lo interfaces
    else
    ARGC=$1 #Read number of interfaces from command line if present
fi

while [ $ARGC -gt 0 ]; do
  ifconfig lo:${ARGC} down
  ARGC=$(expr $ARGC - 1)
done
