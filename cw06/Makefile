# Sources' names
COMMON = common
CLIENT = client
SERVER = server

#compilator's flags
FLAGS = -Wall

all: $(CLIENT).out $(SERVER).out

$(CLIENT).out: $(CLIENT).c $(COMMON).c
	gcc $(FLAGS) $(CLIENT).c $(COMMON).c -o $(CLIENT).out

$(SERVER).out: $(SERVER).c $(COMMON).c
	gcc $(FLAGS) $(SERVER).c $(COMMON).c -o $(SERVER).out


test: all
	./server.out &

	./client.out commands1.txt &
	./client.out commands1.txt &
	./client.out commands1.txt &

	./client.out commands2.txt &

.PHONY: clean
clean:
	-rm -f $(COMMON).out
	-rm -f $(CLIENT).out
	-rm -f $(SERVER).out