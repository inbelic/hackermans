CC=g++
CFLAGS=-Wall
SRC=hackermans.cpp
EXE=hackermans

all: build

build: $(SRC)
	g++ $(SRC) $(CFLAGS) -o $(EXE)

test:	$(EXE)
	./$< toy_example/toy_testcase.csv

clean:
	rm $(EXE)
