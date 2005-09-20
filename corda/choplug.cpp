#include <iostream>
#include <string>
#include <cassert>
#include <sstream>


#include "plug.h"
#include "song.h"

using namespace std;

class ChoPlugin:public Plugin {
private:
  static const bool dummy;
  static bool startup() {
    Plugin::Register("cho",&(ChoPlugin::Createin));
    return true;
  };
  static Plugin* Createin() {return new ChoPlugin();};
public:

  virtual int Read(istream &input, SongList &list) {
    in=&input;
    init();
    while (true) {
      song=new Song;
      stanza=new Stanza;
      verse=new Verse; // il verso e` ancora vuoto
      song->setHead(new Head);
      song->setBody(new Body);
      type=Stanza::STROPHE;
      if (read()) 
	list.push_back(song);
      else {
	delete song;
	delete stanza;
	delete verse;
	break;
      }
    }
    return 1;
  };
  ChoPlugin(): 
             Plugin("chordpro","cho","chordpro song format http://www.nada.kth.se/~f91-jsc")
  {};
  

private:
  istream *in;
  int nline;
  int ncol;
  int c;
  Song *song;
  Stanza *stanza;
  Verse *verse;
  Stanza::Type type;

  void error(const string &msg) {
    stringstream m;
    m<<nline<<":"<<ncol<<" ERROR: "<<msg<<"\n";
    throw runtime_error(m.str());
  };

  void nextChar() {
    if (in->eof()) {c=4;return;}
    c=in->get();
    if (c==-1) {c=4;return;}
    if (c==10 && ncol>=0) {nline++;ncol=1;}
    else ncol++;
    //    return c;
  };

//   void ungetChar(int c) {
//     in->putback(c);
//     ncol--;
//   };

  void init() {
    nline=1;
    ncol=0;
    nextChar();
  };

  void newVerse() {
    if (verse->size()) { 
      stanza->push_back(verse);
      verse=new Verse;
    } 
  };

  void newStanza() {
    newVerse();
    if (stanza->size()) {
      song->body()->push_back(stanza);
      stanza=new Stanza;
      stanza->setType(type);
    } else {
      stanza->setType(type);
    }
  };
  
  int read() { 
    // torna 1 se ho letto una canzone, 0 altrimenti
    while (c!=4) {
      if (false) {
      } else if (c=='[') { // chord
	nextChar();
	string chord;
	while(c!=4 && c!=']') {
	  chord+=c;
	  nextChar();
	}
	if (c!=']') error("']' expected");
	nextChar();
	verse->push_back(new Chord(chord));
      } else if (c=='{') { // directive
	nextChar();
	string cmd;
	string args;
	while (c!=4 && c!='}' && c!=':' && c!=' ' && c!=10) {
	  cmd+=c;
	  nextChar();
	}
	if (c==':') nextChar();
	while (c==' ' || c=='\t') nextChar();
	while (c!=4 && c!='}' && c!=10) {
	  args+=c;
	  nextChar();
	}
	if (c!='}') error("'}' expected in command '"+cmd+"'");
	nextChar();
	if (false) {
	} else if (cmd=="title" || cmd=="t") {
	  song->head()->title=args;
	} else if (cmd=="subtitle" || cmd=="st") {
	  song->head()->author.push_back(new Author("",args));
	} else if (cmd=="start_of_chorus" || cmd=="soc") {
	  type=Stanza::REFRAIN;
	  newStanza();
	} else if (cmd=="end_of_chorus" || cmd=="eoc") {
	  type=Stanza::STROPHE;
	  newStanza();
	} else if (cmd=="comment" || cmd=="c") {
	  PhraseList *list=new PhraseList;
	  list->push_back(new Word(args));
	  verse->push_back(new Modifier(Modifier::NOTES,list));
	} else if (cmd=="comment_italic" || cmd=="ci"
		   || 
		   cmd=="comment_box" || cmd=="cb") {
	  PhraseList *list=new PhraseList;
	  list->push_back(new Word(args));
	  verse->push_back(new Modifier(Modifier::STRUM,list));
	} else if (cmd=="textfont" || cmd=="tf") {
	  // ignore
	} else if (cmd=="chordfont" || cmd=="cf") {
	  // ignore
	} else if (cmd=="chordsize" || cmd=="cs") {
	  // ignore
	} else if (cmd=="textsize" || cmd=="ts") {
	  // ignore
	} else if (cmd=="new_song" || cmd=="ns") {
	  break; // finisce di leggere il file
	} else if (cmd=="define" || cmd=="d") {
	} else if (cmd=="no_grid" || cmd=="ng") {
	} else if (cmd=="grid" || cmd=="g") {
	} else if (cmd=="new_page" || cmd=="np") {
	} else if (cmd=="start_of_tab" || cmd=="sot") {
	  type=Stanza::TAB;
	  newStanza();
	} else if (cmd=="end_of_tab" || cmd=="eot") {
	  type=Stanza::STROPHE;
	  newStanza();
	} else if (cmd=="column_break" || cmd=="colb") {
	} else if (cmd=="columns" || cmd=="col") {
	} else if (cmd=="new_physical_page" || cmd=="np") {
	} else error("invalid directive: '"+cmd+"'");
      } else if (c==13) { //ignore
	nextChar();
      } else if (c==10) { // newline
	nextChar();
	if (verse->size())
	  newVerse();
	else 
	  newStanza();
      } else if (c=='#') { // comment
	nextChar();
	while (c!=4 && c!=10) nextChar();
      } else if (c==' ' || c=='\t') { // space
	string spaces;
	spaces+=c;
	nextChar();
	while (c==' ' || c=='\t') {
	  spaces+=c;
	  nextChar();
	}
	if (stanza->type==Stanza::TAB)
	  verse->push_back(new Word(spaces));
	else
	  verse->push_back(new Word(" "));
      } else if (c==']' || c=='}') {
	error(string("unexpected brace '")+char(c)+"'");
      } else if (c>32) { // normal text
	string word;
	while (c!=4 && c!='[' && c!='{' && c!=' ' && c!='\t' &&
	       c!=10 && c!=13 && c!='#' && c!='}' && c!=']') {
	  word+=c; 
	  nextChar();
	}
	verse->push_back(new Word(word));
      } else { // unrecognized char
	error("unrecognized character code: "+c);
      }
    }
    if (song->body()->size())
      return 1;
    else return 0;
  };
};

// registra il plugin
const bool ChoPlugin::dummy=ChoPlugin::startup();

