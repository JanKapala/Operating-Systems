
#compilator's flags
FLAGS = -Wall

all: barber.out client.out

barber.out: barber.c common.c salonLib.c semLib.c confirmations.c
	gcc $(FLAGS) barber.c common.c salonLib.c semLib.c confirmations.c -o barber.out

client.out: client.c common.c salonLib.c semLib.c confirmations.c
	gcc $(FLAGS) client.c common.c salonLib.c semLib.c confirmations.c -o client.out

.PHONY: clean
clean:
	-rm -f barber.out
	-rm -f client.out
