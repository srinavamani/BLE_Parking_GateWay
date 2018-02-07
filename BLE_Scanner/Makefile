GENDIR = "generic"
DRVDIR = "Drivers"
SRCDIR = "src"
APPNAME = "ChatterBoxHostApp"
CUSTOM_FLAGS = -Werror=unused-variable -D MAX_NUM_PERIPHERALS=5

all:
	make -C ./$(DRVDIR)/
	make -C ./$(GENDIR)/
	make -C ./$(SRCDIR)/
	$(CC) $(CUSTOM_FLAGS) $(LDFLAGS)-o $(APPNAME) ./$(GENDIR)/*.o ./$(DRVDIR)/*.o ./$(SRCDIR)/*.o -lpthread  -fno-stack-protector

clean:
	rm -rf ./$(DRVDIR)/*.o  ./$(GENDIR)/*.o ./$(SRCDIR)/*.o $(APPNAME)
