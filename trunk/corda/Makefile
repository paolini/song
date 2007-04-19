## Version vedi version.h

OBJECTS=main layout media print plug debug song iso sngplug xngplugout psmedia psplug textmedia txtplug lstplug choplug htmlplugout

sources=$(shell ls Makefile Makefile.common Makefile.win song.dev README* *.cpp *.h *.xng *.cc *.dtd test.sng test.xng test.cho cordalib.dev test2.cho)

CPPFLAGS=$(COMMON_CPPFLAGS)

## se si dispone della libreria libxml2
#LDFLAGS+= -lxml2
#BJECTS+= xngplugin
#CPPFLAGS+= -I/usr/include/libxml2

## se si dispone della libreria libharu (PDF)
#LDFLAGS+= -lz -lpng  -lharu
#OBJECTS+= pdfmedia pdfplug

## se si dispone della libreria socket++ (web support)
#LDFLAGS+= -lsocket++
#OBJECTS+= web.cc
#CPPFLAGS+= -DWEB_SUPPORT

TARGET=corda

all: $(TARGET)

lib: libcorda.a

install: $(TARGET) # lib
	cp $(TARGET) /usr/local/bin
#	cp libcorda.a /usr/local/lib
#	mkdir /usr/local/include/corda || true
#	cp song.h plug.h

libcorda.a: choplug.o debug.o iso.o layout.o lstplug.o media.o pdfmedia.o pdfplug.o plug.o print.o psmedia.o psplug.o sngplug.o song.o textmedia.o txtplug.o
	ar rc $@ $^
	ranlib $@

-include Makefile.common

$(TARGET): $(addsuffix .o, $(OBJECTS) $(PDF_OBJECTS) $(XML_OBJECTS) )
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $^ $(PDF_FLAGS) $(XML_FLAGS) \
	-o $(TARGET)

clean: clear.common
	-rm -f *.o *~ delme* $(TARGET)

clear: clean

# tarball::
# 	mkdir ../$(VERSION)
# 	-cp $(sources) ../$(VERSION)
# 	mkdir ../$(VERSION)/CVS
# 	echo corda > ../$(VERSION)/CVS/Repository
# 	echo ":pserver:anonymous@cvs.sourceforge.net:/cvsroot/corda" \
# 	> ../$(VERSION)/CVS/Root
# 	cd .. && tar czvf $(VERSION).tgz $(VERSION)
# 	rm -fr ../$(VERSION)

