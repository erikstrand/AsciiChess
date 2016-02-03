CC=clang++ -g -std=c++11
ESTD=../estdlib
ESTDH=$(ESTD)/h

all: bin/main

bin/main: src/main.cpp bin/Chess.o bin/AsciiBoard.o bin/MoveParser.o
	$(CC) -o bin/main src/main.cpp bin/Chess.o bin/AsciiBoard.o bin/MoveParser.o -I$(ESTDH)

bin/Chess.o: src/Chess.cpp src/Chess.h $(ESTDH)/HashFunctions.h
	$(CC) -c src/Chess.cpp -o bin/Chess.o -I$(ESTDH)

bin/AsciiBoard.o: src/AsciiBoard.cpp src/AsciiBoard.h src/Chess.h
	$(CC) -c src/AsciiBoard.cpp -o bin/AsciiBoard.o -I$(ESTDH)

bin/GambitInterface.o: src/GambitInterface.cpp src/GambitInterface.h src/Chess.h
	$(CC) -c src/GambitInterface.cpp -o bin/GambitInterface.o -I$(ESTDH)

#bin/MoveParser.o: src/MoveParser.cpp src/MoveParser.h src/Chess.h
#	$(CC) -c src/MoveParser.cpp -o bin/MoveParser.o -I$(ESTDH)

clean:
	rm -rf bin/*
