TARGET = ContactList

$(TARGET) : main.o ContactList.o Menu.o
	gcc -g main.o ContactList.o Menu.o -o $(TARGET)

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

ContactList.o : ./src/contacts/ContactList.c
	gcc -g -c ./src/contacts/ContactList.c -o ContactList.o

Menu.o : ./src/menu/Menu.c
	gcc -g -c ./src/menu/Menu.c -o Menu.o

clean : 
	rm -rf *.o $(TARGET)