CC = g++
CFLAGS = -Wall -g

run: eptp
	./eptp
eptp: eptp.o
	$(CC) $(CFLAGS) -o eptp eptp.o 
eptp.o: eptp.cpp
	$(CC) $(CFLAGS) -c -o eptp.o eptp.cpp
clean:
	rm -f eptp eptp.o
	rm -f *.txt