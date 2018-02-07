CC=gcc
CFLAGS=-I.
LP=-lpthread

all: led_manager.o led_controller.o
		$(CC) -o led_manager led_manager.o
		$(CC) -o led_controller led_controller.o $(LP)

clean: 
		rm led_manager led_controller *.o
