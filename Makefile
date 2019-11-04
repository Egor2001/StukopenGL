SHELL=/bin/sh

BINDIR=bin
SRCDIR=src

SOURCES=main.cpp

CC=g++
CFLAGS= -c -std=c++17 -lpthread -mavx -Wall -ggdb -Ofast 
EXECUTABLE=$(BINDIR)/main

LD=$(CC)
LDFLAGS= -lpthread -mavx 
OBJFILE=$(BINDIR)/main.o

CTAGS=ctags
TAGFLAGS=-R
TAGFILE=tags

all: $(EXECUTABLE)

.PHONY: tags clean

$(EXECUTABLE): $(OBJFILE) 
	$(LD) $(LDFLAGS) -o $(EXECUTABLE) $(OBJFILE)

$(OBJFILE): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(OBJFILE)

tags: 
	$(CTAGS) $(TAGFLAGS) -f $(TAGFILE)

clean:
	rm $(BINDIR)/*.o

