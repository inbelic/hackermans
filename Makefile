CC=g++
CFLAGS=-Wall
SRC=hackermans.cpp
EXE=hackermans

all: build test

build: $(SRC)
	g++ $(SRC) $(CFLAGS) -o $(EXE)

test: $(EXE)
	./$< toy_example/toy_testcase.csv

valg: $(EXE) build 
	valgrind ./$< toy_example/toy_testcase.csv

submit: build
	./run.sh

clean:
	rm $(EXE)
