COMPILE = g++ -std=c++11 -pthread -O3 -c
LINK = g++ -std=c++11 -pthread -O3 -lboost_program_options

OBJ = nodegraph.o misc.o timedetector.o

all:	timedetector

timedetector:	$(OBJ)
	clear
	clear
	@echo "===================================="
	$(LINK) $(OBJ) -o timedetector

crank:	$(OBJCRANK) crank.o
	clear
	clear
	@echo "===================================="
	$(LINK) $(OBJCRANK) -o crank

run:	timedetector
	./timedetector

run_crank:	crank
	./crank

clear:	
	rm $(OBJ)
	rm -rf  ./timedetector

%.o:%.cpp
	$(COMPILE) $< -o $@

