#!/bin/bash

NFCAPDDir=../../netflow/zeus
FirstNFCAPDFile=nfcapd.201411181315
LastNFCAPDFile=nfcapd.201411181330

# -dummy does not do bidirectional flows aggregation; will work faster, but timedetector
# will get more input => more memory will be used.
# One can use 
#    AGGR=bidirectional/netflowaggr
# to collect the agregated flows within a given timeframe
AGGR=bidirectional/netflowaggr-dummy

if [ -z "$1" ]
then
	nfdump -q  -N  -R $NFCAPDDir/$FirstNFCAPDFile:$LastNFCAPDFile  -o "fmt:%ts %sa %sp %da %dp %byt"   | $AGGR | timedetector/timedetector --config timedetector/timedetector.conf
	exit 0
fi
	nfdump -q  -N  -R $NFCAPDDir/$FirstNFCAPDFile:$LastNFCAPDFile  -o "fmt:%ts %sa %sp %da %dp %byt" -c $1  | $AGGR | timedetector/timedetector
