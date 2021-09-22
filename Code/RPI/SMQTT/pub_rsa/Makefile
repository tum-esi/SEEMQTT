CC = gcc
LDLIBS = -lpaho-mqtt3c
CFLAGS = -W -O 
LDOSSL = -lssl -lcrypto
OSSLFLAG = -DOSSL

PUB_OSSL: pub_ossl.o ossl.o dbg.o
	$(CC) -o PUB_OSSL pub_ossl.o ossl.o dbg.o $(LDLIBS) $(LDOSSL)
pub_ossl.o: sec_pub.c   config.h
	$(CC) $(CFLAGS) $(OSSLFLAG) -c  sec_pub.c -o pub_ossl.o
ossl.o: ../lib/ossl.c ../lib/ossl.h
	$(CC) $(CFLAGS) $(OSSLFLAG)  -c  ../lib/ossl.c
dbg.o: ../lib/dbg.c ../lib/dbg.h
	$(CC) $(CFLAGS) -c  ../lib/dbg.c
clean:
	rm -f PUB_* *.o
