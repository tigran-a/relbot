COMPILE = g++ -std=c++11 -pthread -O3 -c
LINK = g++ -std=c++11 -pthread -O3 -lboost_program_options

OBJCRANK = nodegraph.o misc.o prank.o crank.o

all:	crank


crank:	$(OBJCRANK) crank.o
	clear
	clear
	@echo "===================================="
	$(LINK) $(OBJCRANK) -o crank

run:	crank
	./crank

run_crank:	crank
	./crank

clear:	
	rm $(OBJCRANK)
	rm -rf ./crank

%.o:%.cpp
	$(COMPILE) $< -o $@

