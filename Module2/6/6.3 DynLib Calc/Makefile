TARGET = Calculator
FUNCTIONS = ./src/functions/CalculatorFunctions.c
MENU = ./src/menu/CalculatorMenu.c
MAIN = ./src/Main.c

$(TARGET) : Main.o CalculatorMenu.o
	gcc -g Main.o CalculatorMenu.o -o $(TARGET) -ldl && rm *.o

Main.o : $(MAIN)
	gcc -g -c $(MAIN) -o Main.o

CalculatorMenu.o : $(MENU)
	gcc -g -c $(MENU) -o CalculatorMenu.o

clean:
	rm -rf *.o $(TARGET)