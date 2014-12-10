#!/bin/sh

# Start dropping outgoing packets from source address.
# Make sure to edit stopPacketDrop.sh if you edit this file!
# Must be run as su.
# Args:
# 1: source address
# 2: fraction

cmd="sudo iptables -A OUTPUT -m statistic --mode random --source $1 --probability $2 -j DROP"
echo "Executing ${cmd}"
$cmd
