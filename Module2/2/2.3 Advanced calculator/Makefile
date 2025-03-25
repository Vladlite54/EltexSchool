TARGET = Calculator
FUNCTIONS = ./src/functions/CalculatorFunctions.c
MENU = ./src/menu/CalculatorMenu.c
MAIN = ./src/Main.c

$(TARGET) : Main.o CalculatorMenu.o CalculatorFunctions.o
	gcc -g Main.o CalculatorMenu.o CalculatorFunctions.o -o $(TARGET)

Main.o : $(MAIN)
	gcc -g -c $(MAIN) -o Main.o

CalculatorMenu.o : $(MENU)
	gcc -g -c $(MENU) -o CalculatorMenu.o

CalculatorFunctions.o : $(FUNCTIONS)
	gcc -g -c $(FUNCTIONS) -o CalculatorFunctions.o

clean:
	rm -rf *.o $(TARGET)