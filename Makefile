CC = g++
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc/.cpp files here (source files, excluding header files)
SIM_SRC = sim_bp.cc

# List corresponding compiled object files here (.o files)
SIM_OBJ = sim_bp.o
 
#################################

# default rule
all: sim
	@echo "my work is done here..."

# rule for making sim
sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH sim-----------"

# generic rule for converting any .cpp file to any .o file
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

.cpp.o:
	$(CC) $(CFLAGS)  -c $*.cpp

# rule for running script
exec: sim
	mkdir -p output
	./run.sh

# rule for parsing results
parse: exec
	echo type,trace,m1,m2,n,k,hash,mpr > re.csv
	python3 parse.py

# type "make clean" to remove all .o files plus the sim binary
clean:
	rm -f *.o sim

# type "make clobber" to remove all .o files (leaves sim binary)
clobber:
	rm -f *.o

# remove all output text files
cleanout:
	rm -f output/*.txt
