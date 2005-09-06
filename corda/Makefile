all: song

install: song
	cp song /usr/local/bin

sources=$(shell ls Makefile Makefile.common Makefile.win song.dev README* *.cpp *.h *.xng *.dtd test.sng test.xng)

-include Makefile.common
VERSION=$(DIRNAME)
CPPFLAGS=$(COMMON_CPPFLAGS) -I/usr/include/libxml2 -DVERSION=\"$(VERSION)\"
LDFLAGS=-lxml2 -lz -lpng  -lharu

#CXX=g++
#CC=gcc

song: main.o layout.o util.o \
	textmedia.o txtplug.o lstplug.o \
	psmedia.o psplug.o print.o \
	plug.o sngplug.o xngplug.o pdfmedia.o pdfplug.o media.o \
	chord.o debug.o
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean: clear.common
	-rm -f *.o *~ delme*

clear: clean