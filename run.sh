#!/bin/bash

NFCAPDDir=../../netflow/zeus
FirstNFCAPDFile=nfcapd.201411181315
LastNFCAPDFile=nfcapd.201411181330

if [ -z "$1" ]
then
	nfdump -q  -N  -R $NFCAPDDir/$FirstNFCAPDFile:$LastNFCAPDFile  -o "fmt:%ts %sa %sp %da %dp %byt"   | bidirectional/netflowaggr-dummy | timedetector/timedetector --config timedetector/timedetector.conf
	exit 0
fi
	nfdump -q  -N  -R $NFCAPDDir/$FirstNFCAPDFile:$LastNFCAPDFile  -o "fmt:%ts %sa %sp %da %dp %byt" -c $1  | bidirectional/netflowaggr-dummy | timedetector/timedetector
