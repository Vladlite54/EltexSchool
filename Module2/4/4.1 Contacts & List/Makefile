TARGET = ContactList
CONTACTS = ./src/contacts/Contact.c
MENU = ./src/menu/Menu.c
LIST = ./src/linked_list/List.c

$(TARGET) : main.o Contact.o Menu.o List.o
	gcc -g main.o Contact.o Menu.o List.o -o $(TARGET)

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

Contact.o : $(CONTACTS)
	gcc -g -c $(CONTACTS) -o Contact.o

Menu.o : $(MENU)
	gcc -g -c $(MENU) -o Menu.o

List.o : $(LIST)
	gcc -g -c $(LIST) -o List.o
clean : 
	rm -rf *.o $(TARGET)