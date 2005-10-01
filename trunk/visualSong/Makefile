#
# File:		Makefile for samples
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling

CXX = $(shell wx-config --cxx)

OTHER = -D"wxCLOSE_BOX=0" -D"PATH_TO_SONG_EXE=\"~/share/public_html/src/song/song-2.1/sng2ps\""

PROGRAM = visualSong

OBJECTS = SongEditorFrm.o VisualSongApp.o VisualSongFrm.o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) $(OTHER) -c `wx-config --cxxflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) `wx-config --libs`

clean: 
	rm -f *.o $(PROGRAM)
