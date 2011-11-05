objects = main.o Parse.o Utility.o Stack.o


Interpreter : $(objects)
	cc -o Interpreter $(objects) \
        `pkg-config --libs --cflags glib-2.0 gtk+-2.0` -lm

main.o : main.c Parse.h term_icon.xpm
	cc -c main.c -o main.o \
        `pkg-config --libs --cflags gtk+-2.0`
Parse.o : Utility.h Stack.h
	cc -c Parse.c -o Parse.o \
        `pkg-config --libs --cflags glib-2.0`
Utility.o :
	cc -c Utility.c -o Utility.o `pkg-config --libs --cflags glib-2.0`
Stack.o :
	cc -c Stack.c -o Stack.o `pkg-config --libs --cflags glib-2.0`

.PHONY : clean
clean :
	rm Interpreter $(objects)