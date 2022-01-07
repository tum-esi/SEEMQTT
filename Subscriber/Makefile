CC = gcc
LDLIBS = -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

Subscriber: subscriber.o ss.o
	$(CC) -o Subscriber subscriber.o  ss.o $(LDLIBS) $(LDOSSL)
subscriber.o: subscriber.c subscriber.h
		$(CC) $(CFLAGS) $(OSSLFLAG) -c subscriber.c -o subscriber.o
ss.o: shamirsecretshare.c shamirsecretshare.h
		$(CC) $(CFLAGS) $(OSSLFLAG) -c  shamirsecretshare.c -o ss.o
clean:
		rm -f Subscriber *.o
