EXT = cpp
CXX=g++
CFLAGS=-Wall 
CFLAGS= 
LDFLAGS= -lGL -lGLEW -lGLU -lglut  -lm  -llapack -lblas

#SRC=saisieinteractive.cpp courbe.cpp
SRC=$(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)

DEBUBFLAG=-g

BIN=./
EXEC=main

all: $(EXEC)

$(EXEC): $(OBJ)
		$(CXX)  -o $(BIN)$@ $^ $(LDFLAGS)

%.o: %.cpp
		$(CXX)  -o $@ -c $< $(CFLAGS)


clean:
		rm -rf *.o

mrproper: clean
		rm -rf $(BIN)$(EXEC)

