GENDIR = "generic"
DRVDIR = "Drivers"
SRCDIR = "src"
all:
	 $(CC) $(CFLAGS) -c -w -I./../$(GENDIR)/ -I./../$(DRVDIR)/ -I./../$(SRCDIR)/ *.c -fno-stack-protector

clean:
	rm -rf *.o

