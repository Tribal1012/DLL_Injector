.SUFFIXES : .cpp .o

MAKEFILE = dll_injector.mk

SOURCE = main.cpp

MAKE = nmake -f $(MAKEFILE)
CC = cl.exe
CFLAGS = /Ox /D_UNICODE /DUNICODE
LINKLIB = /link advapi32.lib

OBJECT = main.o
TARGET = DLLInjector

$(TARGET):
	ECHO "------------------------------------"
	ECHO [Complie] Make $(TARGET)
	$(CC) $(SOURCE) $(CFLAGS) /Fe$(TARGET) $(LINKLIB)
	ECHO [OK] Make $(TARGET)
	ECHO "------------------------------------"

clean:
	ECHO "------------------------------------"
	DEL $(TARGET) $(OBJECT)
	ECHO "------------------------------------"
 
new:
	$(MAKE) clean
	$(MAKE)

all:
	$(MAKE) clean
	$(MAKE) $(TARGET)
