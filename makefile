run : converter.o
	gcc -o run converter.o
converter.o : converter.c
	gcc -c -o converter.o converter.c
