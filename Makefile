all:
	g++ -o game.o src/main.cpp -lSDL2

run:
	./game.o
