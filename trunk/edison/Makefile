CXX = $(shell wx-config --cxx)

FLAGS = -I../corda -L../corda -g

PROGRAM = edison

OBJECTS = main.o wxmedia.o frame.o canvas.o

CORDA_OBJECTS = choplug.o debug.o iso.o layout.o lstplug.o media.o pdfmedia.o pdfplug.o plug.o print.o psmedia.o psplug.o sngplug.o song.o textmedia.o txtplug.o cursor.o

%.o : %.cpp
	$(CXX) -c `wx-config --cxxflags` $(FLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS) $(addprefix ../corda/, $(CORDA_OBJECTS))
	$(CXX) -o $(PROGRAM) $^ $(FLAGS) `wx-config --libs` -lharu -lxml2

clean: 
	rm -f *.o *~ $(PROGRAM)

clear: clean
