CXX = $(shell wx-config --cxx)

##VERSION= vedi version.h

FLAGS = -I.. -L../corda -g 

PROGRAM = edison

OBJECTS = main.o wxmedia.o frame.o canvas.o editor.o list.o

CORDA_OBJECTS = choplug.o debug.o iso.o layout.o lstplug.o media.o pdfmedia.o pdfplug.o plug.o print.o psmedia.o psplug.o sngplug.o song.o textmedia.o txtplug.o cursor.o



#common directives

#
# set $(sources) to list all source files (*.h *.cpp Makefile...)
# $(TAR) file di TAR
#

first: edison

sources=$(shell ls *.cpp *.h Makefile)

version:
	@echo package $(DIRNAME) CPPFLAGS $(CPPFLAGS)

DIRNAME=$(notdir $(CURDIR))

CPPFLAGS+= -Wall

ifeq ($(findstring -rev,$DIRNAME),)
  ## WORKING VERSION 
  COMMON_CPPFLAGS+= -g
else
  ## FINAL VERSION
  COMMON_CPPFLAGS+= -DNDEBUG -O3
endif

TAR=$(DIRNAME).tgz

-include $(filter %.d, $(sources:.cpp=.d))

%.d: %.cpp
	set -e; $(CXX) -MM `wx-config --cxxflags` $(FLAGS) $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

clear.common:
	rm -f *.o *~ delme* *.d $(TAR)
	for file in "" $(filter-out $(sources),$(wildcard *)) ; do \
	  if [ -f "$$file" ] ; then rm -i "$$file" ; fi; done

tar.common:
	tar -C .. -czf $(TAR) \
	$(patsubst %,$(DIRNAME)/%,$(sources))

## fine common

%.o : %.cpp
	$(CXX) -c `wx-config --cxxflags` $(FLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS) $(addprefix ../corda/, $(CORDA_OBJECTS))
	$(CXX) -o $(PROGRAM) $^ $(FLAGS) `wx-config --libs` -lharu -lxml2

clean: 
	rm -f *.o *~ $(PROGRAM)

clear: clean


