CXX = $(shell wx-config --cxx)

LAST_VERSION=0.2

CVS_REVISION=$(shell echo $Revision 1.1$ | cut -f2 -d\\$ | cut -f2 -d' ')

VERSION=$(LAST_VERSION)-CVS-$(CVS_REVISION)

FLAGS = -I.. -L../corda -g -DVERSION=\"$(VERSION)\"

PROGRAM = edison

OBJECTS = main.o wxmedia.o frame.o canvas.o editor.o list.o

CORDA_OBJECTS = choplug.o debug.o iso.o layout.o lstplug.o media.o pdfmedia.o pdfplug.o plug.o print.o psmedia.o psplug.o sngplug.o song.o textmedia.o txtplug.o cursor.o

%.o : %.cpp
	$(CXX) -c `wx-config --cxxflags` $(FLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS) $(addprefix ../corda/, $(CORDA_OBJECTS))
	$(CXX) -o $(PROGRAM) $^ $(FLAGS) `wx-config --libs` -lharu -lxml2

clean: 
	rm -f *.o *~ $(PROGRAM)

clear: clean
