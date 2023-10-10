
CC=g++ 
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)


csim: main.o csimFunc.o
	$(CC) -o csim main.o csimFunc.o

main.o: main.cpp csim_func.h
	$(CC) -c main.cpp $(CFLAGS)

csimFunc.o : csim_func.cpp csim_func.h
	$(CC) -c csim_func.cpp $(CFLAGS)

clean:
	rm -f *.o csim *~