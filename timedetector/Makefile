COMPILE = g++ -std=c++11 -pthread -O3 -c
LINK = g++ -std=c++11 -pthread -O3 
LINK_LIBS = -lboost_program_options

OBJ = nodegraph.o misc.o timedetector.o

all:	timedetector

timedetector:	$(OBJ)
	clear
	clear
	@echo "===================================="
	$(LINK) $(OBJ) -o timedetector $(LINK_LIBS)

crank:	$(OBJCRANK) crank.o
	clear
	clear
	@echo "===================================="
	$(LINK) $(OBJCRANK) -o crank $(LINK_LIBS)

run:	timedetector
	./timedetector

run_crank:	crank
	./crank

clear:	
	rm $(OBJ)
	rm -rf  ./timedetector

%.o:%.cpp
	$(COMPILE) $< -o $@

