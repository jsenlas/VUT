NAME = proj
EXEC = $(NAME)
sources = $(NAME).c 

CXX = gcc
RM = rm -f

CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS = -lrt -pthread
#CFLAGS = -std=gnu99 -Wall -Wextra -pedantic
#CFLAGS = -std=gnu99 -Wall -pedantic
#CFLAGS = -std=gnu99 -Wall 
#CFLAGS = -std=gnu99
#-g0 -s -O3

OBJFILES = $(sources:.c=.o)

.PHONY : all

all : $(EXEC) 

%.o : %.c 
	$(CXX) $(CFLAGS) -c $< -o $@

$(EXEC) : $(OBJFILES)
	$(CXX) $(CFLAGS) -o $@ $(OBJFILES) $(LDFLAGS)

clean:
	$(RM) *.o core *.out $(NAME)

cleanall: clean
	$(RM) $(EXEC) $(NAME)
