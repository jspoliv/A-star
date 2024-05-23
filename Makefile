# make arg="entrada100.txt saida.txt"

all: build run

run:
	@bin/A-star $(arg)

build:
	@gcc -Wall -g -c "./heap.c" -o obj/heap.o
	@gcc -Wall -g -c "./a-star.c" -o obj/a-star.o
	@gcc -Wall -g -c "./main.c" -o obj/main.o
	@gcc -o bin/a-star obj/a-star.o obj/heap.o obj/main.o

opt:
	gcc -O2 -o bin/a-starO2 heap.c a-star.c main.c
	gcc -O3 -o bin/a-starO3 heap.c a-star.c main.c

runO2:
	@bin/a-starO2 $(arg)

runO3:
	@bin/a-starO3 $(arg)

runO: runO2 runO3