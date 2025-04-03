TARGET = ContactList
MENU = ./src/menu/Menu.c
LIB = /home/vladlite/EltexSchool/Module2/6/6.2\ DynLibs\ List/contact_list/

$(TARGET) : main.o Menu.o 
	gcc -g main.o Menu.o -o $(TARGET) -L$(LIB) -lcontactlist -Wl,-rpath,$(LIB)

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

Menu.o : $(MENU)
	gcc -g -c $(MENU) -o Menu.o

clean : 
	rm -rf *.o $(TARGET)