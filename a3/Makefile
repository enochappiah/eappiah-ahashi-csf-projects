
CC=g++ 
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)


csim: main.o cache-rep.o
	$(CC) -o csim main.o cache-rep.o

main.o: main.cpp cache-rep.h
	$(CC) -c main.cpp $(CFLAGS)

cache-rep.o: cache-rep.cpp cache-rep.h
	$(CC) -c cache-rep.cpp $(CFLAGS)

solution.zip: 
	rm -f solution.zip
	zip -9r $@ Makefile README.txt *.h *.cpp

clean:
	rm -f *.o $(EXES) *~