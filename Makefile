CFLAGS = -Wall -g -pthread -lrt
PROGS = timerptr

all: $(PROGS)

timerptr: timerptr.c
	$(CC) $(CFLAGS) -std=gnu99 -o $@ $^ -lrt


clean:
	rm -f *.o
	rm -f $(PROGS)

install:
	cp $(PROGS) $(TARGETDIR)/bin
