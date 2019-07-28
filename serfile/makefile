all:ser

ser:main.o thread.o socket.o
	gcc -o ser main.o thread.o socket.o -lpthread
main.o:main.c
	gcc -c main.c 
thread.o:thread.c
	gcc -c thread.c 
socket.o:socket.c
	gcc -c socket.c 
clean:
	rm -f *.o ser

