#!/bin/sh

# Shut down iptables rules to redirect packets to the analyzer/filter sent from
# the test nodes. Must be run as su. The arguments (or lack thereof) passed in must
# match those used when the setup script was called for this to work correctly.

#Usage: ./teardown_redirects.sh [ <# nodes> [ <Port # used by RAFT implementation> ]]
#Defaults to assuming 3 nodes that communicate on port 61023.

NUM_INTERFACES=0

if [ $# -lt 1 ]
    then
    NUM_INTERFACES=3 # Default to assuming 3 nodes
    else
    NUM_INTERFACES=$1 # If argument is present, read the number of nodes
fi

BASE_PORT=61023

if [ $# -gt 1 ]
    then
    BASE_PORT=$2 # Read the port the RAFT impl uses from the command line
fi

I_ITER=0;
while [ $I_ITER -lt $NUM_INTERFACES ]; do
    J_ITER=0;
    while [ $J_ITER -lt $NUM_INTERFACES ]; do
	CUR_PORT=$(expr $BASE_PORT + $J_ITER + 1)
	if [ $I_ITER -ne $J_ITER ]
	    then
	    iptables -t nat -D OUTPUT -s 192.168.2.$(expr $I_ITER + 1) -d 192.168.2.$(expr $J_ITER + 1) -p udp -j REDIRECT --to-ports ${CUR_PORT}
	    iptables -t nat -D OUTPUT -s 192.168.2.$(expr $I_ITER + 1) -d 192.168.2.$(expr $J_ITER + 1) -p tcp -j REDIRECT --to-ports ${CUR_PORT}
	fi
	J_ITER=$(expr $J_ITER + 1)
    done
    I_ITER=$(expr $I_ITER + 1)
done
	    
