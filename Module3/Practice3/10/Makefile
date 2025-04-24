TARGET=Rand2

$(TARGET) : Main.o
	gcc -g Main.o -o $(TARGET) -lrt -lpthread && rm *.o

Main.o : Main.c
	gcc -g -c Main.c -o Main.o

clean:
	rm -rf *.o $(TARGET)