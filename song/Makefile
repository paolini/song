CC= gcc -c -g
CPP= g++ -c -g

VERSION=2.3

#all: songweb song

all: song sng2ps

mailfilter: mailfilter.o mail.o
	g++ -g mailfilter.o mail.o -o mailfilter

mailfilter.o: mailfilter.C mail.h
	$(CPP) mailfilter.C

mail.o: mail.C mail.h
	$(CPP) mail.C

web: songweb
	cp songweb /home/http/cgi-bin/songweb.cgi

songweb: songweb.o song.o songout.o songprin.o cgi-util.o chords.o string.o
	cc -g songweb.o song.o songout.o songprin.o cgi-util.o chords.o string.o -o songweb

song: song.o songmain.o songout.o songprin.o cmdline.o chords.o string.o
	cc -g song.o songmain.o songout.o songprin.o cmdline.o chords.o string.o -o song

sng2ps.o: songmain.c song.h songout.h songprin.h
	$(CC) -DSNG2PS -c songmain.c -o sng2ps.o

sng2ps: song.o songout.o songprin.o cmdline.o chords.o string.o sng2ps.o
	cc -g song.o sng2ps.o songout.o songprin.o cmdline.o chords.o string.o -o sng2ps

songmain.o: songmain.c song.h songout.h songprin.h 
	$(CC) -DVERSION=\"$(VERSION)\" songmain.c 

cmdline.o: cmdline.c cmdline.h
	$(CC) cmdline.c

songweb.o: songweb.c song.h chords.h string.h songprin.h songout.h
	$(CC) songweb.c

song.o: song.c song.h chords.h string.h
	$(CC) song.c

songout.o: songout.c songout.h song.h
	$(CC) songout.c

songprin.o: songprin.c song.h
	$(CC) songprin.c

cgi-util.o: cgi-util.c cgi-util.h string.h
	$(CC) cgi-util.c

chords.o: chords.c song.h chords.h
	$(CC) chords.c

string.o: string.c
	$(CC) string.c

songwrap.o: songwrap.c
	$(CC) $<

songwrap: songwrap.o song.o songout.o string.o chords.o
	cc $^ -o $@

install: song sng2ps
	cp song sng2ps /usr/local/bin

clear:: clean

clean:
	-rm -fr *~
	-rm -fr *.o	

tar:: clear
	tar -C .. -czf ../song-$(VERSION).tgz song-$(VERSION)
	@echo written tar in ../song-$(VERSION).tgz 
