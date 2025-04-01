TARGET = PriorityQueue

$(TARGET) : Main.o Queue.o
	gcc -g Main.o Queue.o -o $(TARGET)

Main.o : Main.c
	gcc -g -c Main.c -o Main.o

Queue.o : Queue.c
	gcc -g -c Queue.c -o Queue.o

clean:
	rm -rf *.o $(TARGET)