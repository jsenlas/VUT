#Compiler
CC=gcc

#Flagz classic
CFLAGS=-lm -g

#Flagz HARD version s -Wextra -Werror -Wall -pedantic
CFLAGSHARD=-Wall -Werror -Wextra -pedantic -lm -g

#Flagz for packet catching
CNETFLAGS=-lpcap

OUT=myripsniffer

#meno sefa timu
BOSS=xsenca00

#PRograms and parts EASY MODIFICATION
PROGRAMS=myripsniffer.c
#USUAL compilation
all:
	$(CC) -o $(OUT) $(PROGRAMS) $(CFLAGS) $(CNETFLAGS)
	$(CC) -o myripresponse ./response/myripresponse.c $(CFLAGS) $(CNETFLAGS)
#using more flags
hard:
	$(CC) -o $(OUT) $(PROGRAMS) $(CFLAGSHARD) $(CNETFLAGS)

#PACKS all important
pack:
	okular --print ./manual.md #print to pdf
	tar -cvf $(BOSS).tar \
						manual.pdf \
						README \
						myripsniffer.c \
						myripsniffer.h \
						./response/myripresponse.c \
						./response/myripresponse.h \
						Makefile

#cleans folder from shit created during packing						
depack:
	rm xsenca00.tar manual.pdf

#Tests
#test:
#	make all && tests/./auto-test.sh

#CLEAN predpripravene pre .o .out
clean:
	rm -f *.o *.out $(OUT)



