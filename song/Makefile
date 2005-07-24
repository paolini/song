


all: song

install: song
	cp song /usr/local/bin

sources=$(shell ls Makefile Makefile.common *.cc *.hh *.xng *.dtd)

-include Makefile.common
VERSION=$(DIRNAME)
CPPFLAGS=$(COMMON_CPPFLAGS) -I/usr/include/libxml2 -DVERSION=\"$(VERSION)\"
LDFLAGS=-lxml2 -lz -lpng  -lharu

#CXX=g++
#CC=gcc

song: main.o layout.o util.o textmedia.o psmedia.o print.o \
	plugin.o sngplugin.o xngplugin.o pdfmedia.o media.o
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean: clear.common
	-rm -f *.o *~ delme*

clear: clean