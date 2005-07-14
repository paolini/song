CC= gcc -g
CPP= g++ -c -g

VERSION=cmd-2.4

#all: songweb song

all: song

song: song.o songmain.o songout.o songprin.o chords.o string.o
	$(CC) $^  -o song

songmain.o: songmain.c song.h songout.h songprin.h 
	$(CC) -c -DVERSION=\"$(VERSION)\" songmain.c 

song.o: song.c song.h chords.h string.h
	$(CC) -c song.c

songout.o: songout.c songout.h song.h
	$(CC) -c songout.c

songprin.o: songprin.c song.h
	$(CC) -c songprin.c

chords.o: chords.c song.h chords.h
	$(CC) -c chords.c

string.o: string.c
	$(CC) -c string.c

install: song 
	cp song /usr/local/bin

clear:: clean

clean:
	-rm -fr *~
	-rm -fr *.o	

tar:: clear
	tar -C .. -czf ../song-$(VERSION).tgz song-$(VERSION)
	@echo written tar in ../song-$(VERSION).tgz 
