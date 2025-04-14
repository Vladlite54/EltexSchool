TARGET=MyShell

$(TARGET) : Main.o
	gcc -g Main.o -o $(TARGET) && rm *.o

Main.o : Main.c
	gcc -g -c Main.c -o Main.o

build_programs:
	gcc -g Argsum.c -o Argsum
	gcc -g Strcat.c -o Strcat

clean:
	rm -rf *.o $(TARGET) Argsum Strcat