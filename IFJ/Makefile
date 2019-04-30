#Compiler
CC=gcc

#Flagz classic
CFLAGS=-Wall -std=c99 -pedantic -lm -g

#Flagz HARD version s -Wextra -Werror -Wall -pedantic
CFLAGSHARD=-Wall -Werror -Wextra -std=c99 -pedantic -lm -g

OUT=ifj18

#meno sefa timu
BOSS=xmicha70

#PRograms and parts EASY MODIFICATION
PROGRAMS=parser.c scanner.c main.c symtable.c ial.c psa.c built_in_fun.c generator.c
#USUAL compilation
all:
	$(CC) -o $(OUT) $(PROGRAMS) $(CFLAGS)
#using more flags
hard:
	$(CC) -o $(OUT) $(PROGRAMS) $(CFLAGSHARD)
#PACKS all important
pack:
	zip $(BOSS).zip *.c *.h Makefile dokumentace.pdf rozdeleni
#Tests
tests:
	cd ./testing && ./run-tests.sh 1 3 && cd .. #make all && 
#CLEAN predpripravene pre .o .out
clean:
	rm -f *.o *.out $(OUT)
