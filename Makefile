SHELL=/bin/sh

BINDIR=bin
SRCDIR=src

SOURCES=main.cpp

CC=g++
CFLAGS= -c -Wall -mavx -ggdb -Ofast 
EXECUTABLE=$(BINDIR)/main

LD=$(CC)
OBJFILE=$(BINDIR)/main.o

CTAGS=ctags
TAGFLAGS=-R
TAGFILE=tags

all: $(EXECUTABLE)

.PHONY: tags clean

$(EXECUTABLE): $(OBJFILE) 
	$(LD) -o $(EXECUTABLE) $(OBJFILE)

$(OBJFILE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(OBJFILE)

tags: 
	$(CTAGS) $(TAGFLAGS) -f $(TAGFILE)

clean:
	rm $(BINDIR)/*.o

