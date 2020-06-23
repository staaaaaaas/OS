all : lab2

lab2: main.o Queue.o
	gcc main.o Queue.o -o lab4

Queue.o: Queue.c Queue.h
	gcc -c Queue.c

lab2.o: main.c Queue.h 
	gcc -c main.c

clean:
	rm -rf *.o 

