CC = gcc
LDLIBS = -L./../KeyNote/ -lkeynote -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
CFLAGS = -W -O

PEM: pem.o
	$(CC) -o PEM pem.o $(LDLIBS) $(LDOSSL)
pem.o: PolicyEvaluationModule.c 
	$(CC) $(CFLAGS) $(OSSLFLAG) -c PolicyEvaluationModule.c -o pem.o
clean:
	rm -f PEM *.o cred* assrt sub

