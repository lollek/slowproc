CC =		cc
CFLAGS =	-Wall -Wextra -Werror -pedantic -Werror=implicit-function-declaration -std=gnu11 -g3

CFILES = $(wildcard src/*.c)
HFILES = $(wildcard src/*.h)
OBJFILES = $(addsuffix .o, $(basename $(CFILES)))

PROGNAME =	slowpro

all:	$(PROGNAME)

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $*.c

$(PROGNAME): $(OBJFILES)
	$(CC) $(LDFLAGS) $(OBJFILES) -o $@

clean ::
	$(RM) $(OBJFILES) $(PROGNAME)
.PHONY: clean

ctags:
	@ctags -R . --exclude .git
.PHONY: ctags

format:
	@clang-format -i $(CFILES) $(HFILES)
.PHONY: format
