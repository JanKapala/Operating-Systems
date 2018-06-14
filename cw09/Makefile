#compilator's flags
FLAGS = -Wall

all: main.c
	gcc main.c -pthread

run: all
	./a.out 2 4 8 panTadeusz.txt 2 '>' quiet 0


.PHONY: clean
clean:
	-rm -f a.out
