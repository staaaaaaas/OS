all : lab5

lab5: mainStat.o Vector.o
	gcc mainStat.o Vector.o -o stat

Vector.o: Vector.c Vector.h
	gcc -c Vector.c

mainStat.o: mainStat.c Vector.h 
	gcc -c mainStat.c

clean:
	rm -rf *.o 

