#Copyright Teodor Matei - 2020
CC = g++
CFLAGS = -c
LDFLAGS = -Ofast -funroll-loops -frename-registers
SOURCES = bit_utils.cpp board.cpp Hashtable.cpp move.cpp search.cpp main.cpp
EXECUTABLE = chewie
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all
all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) 

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(<) -o $(@)
.PHONY: build
build: all

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE)

.PHONY: run
run: all
	./$(EXECUTABLE)

.PHONY: run-engine
run-engine: all
	xboard -fcp ./$(EXECUTABLE)
	
.PHONY: test
test: build
	xboard -fcp "make run" -scp "fairymax" -secondInitString "new\nrandom\nsd 4\n" -tc 5 -inc 2 -autoCallFlag true -mg 4 -sgf partide.txt -reuseFirst false
