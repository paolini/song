
CPPFLAGS=-g -I/usr/include/libxml2
CXX=g++
CC=gcc


all: song

install: song
	cp song /usr/local/bin

sources=$(shell ls Makefile Makefile.common *.cc *.hh)

-include Makefile.common

song: main.o layout.o util.o textmedia.o psmedia.o print.o \
	plugin.o sngplugin.o xngplugin.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lxml2 -o $@

clean: clear.common
	-rm -f *.o *~ delme*

clear: clean