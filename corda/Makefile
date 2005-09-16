all: song

install: song
	cp song /usr/local/bin

sources=$(shell ls Makefile Makefile.common Makefile.win song.dev README* *.cpp *.h *.xng *.dtd test.sng test.xng)

-include Makefile.common
VERSION=corda-0.1.0
CPPFLAGS=$(COMMON_CPPFLAGS) -I/usr/include/libxml2 -DVERSION=\"$(VERSION)\"
LDFLAGS=-lxml2 -lz -lpng  -lharu

#CXX=g++
#CC=gcc

song: 	main.o \
	layout.o \
	media.o \
	print.o \
	plug.o \
	debug.o \
	song.o \
	iso.o \
	sngplug.o \
	xngplugout.o \
	psmedia.o \
	psplug.o \
	pdfmedia.o \
	pdfplug.o \
	textmedia.o \
	txtplug.o \
	lstplug.o \
	xngplugin.o \

	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean: clear.common
	-rm -f *.o *~ delme*

clear: clean