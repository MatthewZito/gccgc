CC=gcc
CFLAGS=-g -fPIC
LDFLAGS=-shared -o

WIN_BIN=lib_gccgc.dll
UNIX_BIN=libgccgc.so

OBJFILES=$(wildcard src/*.c)

all: unix

unix:
	$(CC) -I ./include $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(UNIX_BIN)

win:
	$(CC) $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(WIN_BIN)

clean:
	rm $(TARGET)
