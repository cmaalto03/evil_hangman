
CC = gcc
CFLAGS = -Wall --std=c99
OBJECTS = my_string.o main.o generic_vector.o avl_tree.o

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o main $(OBJECTS)

my_string.o: my_string.c my_string.h
	$(CC) $(CFLAGS) -c my_string.c -o my_string.o

avl_tree.o: avl_tree.c avl_tree.h
	$(CC) $(CFLAGS) -c avl_tree.c -o avl_tree.o

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o main.o

generic_vector.o: generic_vector.c generic_vector.h
	$(CC) $(CFLAGS) -c generic_vector.c -o generic_vector.o

clean:
	rm main $(OBJECTS)
