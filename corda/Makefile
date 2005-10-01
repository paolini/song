
VERSION=corda-0.2
## last release: VERSION=0.1.1


OBJECTS=main layout media print plug debug song iso sngplug xngplugout psmedia psplug textmedia txtplug lstplug choplug

## se si dispone della libreria libxml2
XML_FLAGS=$(LDFLAGS) -lxml2
XML_OBJECTS=$(OBJECTS) xngplugin

## se si dispone della libreria libharu (PDF)
PDF_FLAGS=$(LDFLAGS) -lz -lpng  -lharu
PDF_OBJECTS=$(OBJECTS) pdfmedia pdfplug

TARGET=corda

CPPFLAGS=$(COMMON_CPPFLAGS) -I/usr/include/libxml2 -DVERSION=\"$(VERSION)\"

all: target

install: target
	cp $(TARGET) /usr/local/bin

sources=$(shell ls Makefile Makefile.common Makefile.win song.dev README* *.cpp *.h *.xng *.dtd test.sng test.xng test.cho)

-include Makefile.common

target: $(addsuffix .o, $(OBJECTS) $(PDF_OBJECTS) $(XML_OBJECTS) )
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $^ $(PDF_FLAGS) $(XML_FLAGS) \
	-o $(TARGET)

clean: clear.common
	-rm -f *.o *~ delme* $(TARGET)

clear: clean

tarball::
	mkdir ../$(VERSION)
	make clear
	-cp * ../$(VERSION)
	mkdir ../$(VERSION)/CVS
	echo corda > ../$(VERSION)/CVS/Repository
	echo ":pserver:anonymous@cvs.sourceforge.net:/cvsroot/corda" \
	> ../$(VERSION)/CVS/Root
	cd .. && tar czvf $(VERSION).tgz $(VERSION)
	rm -fr ../$(VERSION)