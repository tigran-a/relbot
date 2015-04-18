all:	netflowaggr netflowaggr-dummy

netflowaggr:	netflowaggr.cpp
	g++ -std=c++11 netflowaggr.cpp -o netflowaggr

netflowaggr-dummy:	netflowaggr-dummy.cpp
	g++ -std=c++11 netflowaggr-dummy.cpp -o netflowaggr-dummy

run:	netflowaggr
	./netflowaggr
