TARGET=ChmodSim

ChmodSim : Main.o
	gcc -g Main.o -o $(TARGET)

Main.o : main.c
	gcc -g -c main.c -o Main.o

clean:
	rm -rf *.o $(TARGET)