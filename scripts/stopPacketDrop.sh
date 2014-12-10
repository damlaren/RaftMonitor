#!/bin/sh

# Stop dropping outgoing packets from source address.
# Make sure to edit startPacketDrop.sh if you edit this file!
# Must be run as su.
# Args:
# 1: source address
# 2: fraction

frac=$2
cmd="sudo iptables -D OUTPUT -m statistic --mode random --source $1 --probability ${frac} -j DROP"
echo "Executing ${cmd}"
$cmd
