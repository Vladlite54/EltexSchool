TARGET = Calculator
LIB = ./math_functions
MENU = ./src/menu/CalculatorMenu.c
MAIN = ./src/Main.c

$(TARGET) : Main.o CalculatorMenu.o
	gcc -g Main.o CalculatorMenu.o -o $(TARGET) -ldl && rm *.o

Main.o : $(MAIN)
	gcc -g -c $(MAIN) -o Main.o

CalculatorMenu.o : $(MENU)
	gcc -g -c $(MENU) -o CalculatorMenu.o

buildLibs:
	gcc -g -c -fPIC $(LIB)/Summary.c -o $(LIB)/Summary.o
	gcc -shared -o $(LIB)/libsumm.so $(LIB)/Summary.o
	gcc -g -c -fPIC $(LIB)/Substract.c -o $(LIB)/Substract.o
	gcc -shared -o $(LIB)/libsubstr.so $(LIB)/Substract.o
	gcc -g -c -fPIC $(LIB)/Multiply.c -o $(LIB)/Multiply.o
	gcc -shared -o $(LIB)/libmulti.so $(LIB)/Multiply.o
	gcc -g -c -fPIC $(LIB)/Devide.c -o $(LIB)/Devide.o
	gcc -shared -o $(LIB)/libdevide.so $(LIB)/Devide.o
	rm -rf $(LIB)/*.o

clean:
	rm -rf *.o $(TARGET)