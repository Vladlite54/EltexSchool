TARGET = ContactList
CONTACTS = ./src/contacts/Contact.c
MENU = ./src/menu/Menu.c
TREE = ./src/btree/Btree.c

$(TARGET) : main.o Contact.o Menu.o Btree.o
	gcc -g main.o Contact.o Menu.o Btree.o -o $(TARGET)

main.o : ./src/main.c
	gcc -g -c ./src/main.c -o main.o

Contact.o : $(CONTACTS)
	gcc -g -c $(CONTACTS) -o Contact.o

Menu.o : $(MENU)
	gcc -g -c $(MENU) -o Menu.o

Btree.o : $(TREE)
	gcc -g -c $(TREE) -o Btree.o

clean : 
	rm -rf *.o $(TARGET)