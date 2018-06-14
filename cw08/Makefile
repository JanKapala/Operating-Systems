#compilator's flags
FLAGS = -Wall

all: main.c
	gcc main.c -pthread -lm -o filter.out

run: all
	./filter.out 8 lena.ascii.pgm filters/random.txt result.ascii.pgm

test: all
	#touch testResults.txt
	for i in $$(seq 1 4);\
	do\
		./filter.out $$((2 * i)) lena.ascii.pgm filters/random.txt result.ascii.pgm >> testResults.txt;\
	done;\

.PHONY: clean
clean:
	-rm -f filter.out
	-rm -f testResults.txt
