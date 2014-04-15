
SRCS_SUFFIX = .cpp
OBJS = app.o 
EXE = rootApp


ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTGLIBS     	= $(shell root-config --glibs)

INCLUDES		= -I../include

CXX = g++
FLAGS = -Wall -g $(ROOTCFLAGS) $(INCLUDES)

COMPILE = $(CXX) $(FLAGS) -c 

all: $(EXE)

$(EXE): $(OBJS)
		$(CXX) -o $(EXE) $(OBJS) $(ROOTGLIBS)
		@cp $(EXE) ../bin

%.o: %$(SRCS_SUFFIX)
		$(COMPILE)  $<

clean:
		@rm -f $(OBJS) $(EXE)
		@rm -f ../bin/$(EXE)
		@echo "clean done"
