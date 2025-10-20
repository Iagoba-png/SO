CC = gcc
CFLAGS = -Wall -Werror

SRCFILES = p0.c firstshellfunctions.c filefunctions.c list.c

p0: $(SRCFILES)
	$(CC) $(CFLAGS) -o p0 $(SRCFILES)

clean:
	rm -f p0

.PHONY: clean


