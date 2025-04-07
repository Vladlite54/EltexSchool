TARGET = ContactList
MENU = ./src/menu/Menu.c
LIB = /home/vladlite/EltexSchool/Module2/6/6.2\ DynLibs\ List/contact_list

$(TARGET) : main.o Menu.o 
	gcc -g main.o Menu.o -o $(TARGET) -L$(LIB) -lcontactlist -Wl,-rpath,$(LIB)

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

Menu.o : $(MENU)
	gcc -g -c $(MENU) -o Menu.o

buildLibs:
	gcc -g -c -fPIC $(LIB)/Contact.c -o $(LIB)/Contact.o
	gcc -g -c -fPIC $(LIB)/List.c -o $(LIB)/List.o
	gcc -shared -o $(LIB)/libcontactlist.so $(LIB)/*.o
	rm -rf $(LIB)/*.o

clean : 
	rm -rf *.o $(TARGET)