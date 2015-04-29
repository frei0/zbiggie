CFLAGS += -m32 -Wall -nostdlib -ffreestanding -g
LDFLAGS += -melf_i386 -nostdlib
CC = gcc

ALL: cat grep hello ls pingpong counter shell sigtest testprint syserr

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -Wall -o $@ $<

%.exe: ece391%.o ece391syscall.o ece391support.o
	ld $(LDFLAGS) -o $@ $^

%: %.exe
	../elfconvert $<
	mv $<.converted to_fsdir/$@

clean::
	rm -f *~ *.o

clear: clean
	rm -f *.converted
	rm -f *.exe
	rm -f to_fsdir/*
