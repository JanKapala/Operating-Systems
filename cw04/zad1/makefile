# Sources' names
MAIN1 = main1
MAIN2 = main2

# Outputs' names
OUTPUTNAME1 = a
OUTPUTNAME2 = b

#compilator's flags
FLAGS = -Wall

all: $(OUTPUTNAME1).out $(OUTPUTNAME2).out

$(OUTPUTNAME1).out: $(MAIN1).c
	gcc $(FLAGS) $(MAIN1).c -o $(OUTPUTNAME1).out

$(OUTPUTNAME2).out: $(MAIN2).c
	gcc $(FLAGS) $(MAIN2).c -o $(OUTPUTNAME2).out

.PHONY: clean
clean:
	-rm -f $(OUTPUTNAME1).out
	-rm -f $(OUTPUTNAME2).out
	