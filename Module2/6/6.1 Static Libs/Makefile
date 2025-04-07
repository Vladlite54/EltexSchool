TARGET = ContactList
MENU = ./src/menu/Menu.c
LIB = ./contact_list

$(TARGET) : main.o Menu.o 
	gcc -g main.o Menu.o -o $(TARGET) -L$(LIB) -lcontactlist

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

Menu.o : $(MENU)
	gcc -g -c $(MENU) -o Menu.o

buildLibs:
	gcc -g -c $(LIB)/Contact.c -o $(LIB)/Contact.o
	gcc -g -c $(LIB)/List.c -o $(LIB)/List.o
	ar r $(LIB)/libcontactlist.a $(LIB)/*.o
	rm -rf $(LIB)/*.o

clean : 
	rm -rf *.o $(TARGET)