# make arg="entrada100.txt saida.txt"

all: build run

run:
	@bin/A-star $(arg)

build:
	@gcc -Wall -g -c "./list.c" -o obj/list.o
	@gcc -Wall -g -c "./a-star.c" -o obj/a-star.o
	@gcc -Wall -g -c "./main.c" -o obj/main.o
	@gcc -o bin/a-star obj/a-star.o obj/list.o obj/main.o