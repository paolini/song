#include <cstdio>
#include <vector>
#include <iostream>
#include <cassert>
#include <map>

#include "song.h"
#include "plug.h"

#define XNG_VERSION "0.1"
#define XNG_DTD "http://www.math.unifi.it/paolini/src/song/xng-" XNG_VERSION ".dtd"

using namespace std;

class XngPlug: public Plugout {
private:
  static const bool dummy;
  static bool startup() {
    Plugout::Register("xng",&(XngPlug::Createout));
    return true;
  };
  static Plugout* Createout() {return new XngPlug();};

  ostream *op;

public:

  virtual void Write(std::ostream &out, std::vector<Song *> &list,
		     const PlugoutOptions &opt) {
    op=&out;
    (*op)<<"<?xml version='1.0' encoding='utf8' ?>\n"
      "<!DOCTYPE song PUBLIC '" XNG_DTD "' ''>\n"
      "<songs>\n";
    for (size_t i=0;i<list.size();++i) {
      assert (list[i]!=NULL);
      writeSong(list[i]);
    }
    (*op)<<"</songs>\n";
  };

  XngPlug(): 
    Plugout("xsong","xng","xml song format [manu-fatto]")
  {};

  void writeSong(const Song *song) {
    (*op)<<"<song>\n";
    if (song->head)
      writeHead(song->head);
    if (song->body)
      writeBody(song->body);
    (*op)<<"</song>\n";
  };

  void writeHead(const Head *head) {
    (*op)<<"<title>"<<head->title<<"</title>\n";
    for (size_t i=0;i<head->author.size();++i)
      writeAuthor(head->author[i]);
  };

  void writeAuthor(const Author *author) {
    (*op)<<"<author><name>"<<author->Name<<"</name>"
      "<firstname>"<<author->firstName<<"</firstname></author>\n";
  };

  map<const Stanza *,int> ids;

  void writeBody(const Body *body) {
    (*op)<<"<body>\n";
    for (size_t i=0;i<body->stanza.size();++i)
      ids[body->stanza[i]]=i;
    for (size_t i=0;i<body->stanza.size();++i)
      writeStanza(body->stanza[i]);
    (*op)<<"</body>\n";
  };

  void writeStanza(const Stanza *stanza) {
    (*op)<<"<stanza id='"<<ids[stanza]<<"'";
    if (stanza->type==Stanza::REFRAIN)
      (*op)<<" type='refrain'";
    else if (stanza->type==Stanza::SPOKEN)
      (*op)<<" type='talking'";
    else if (stanza->type==Stanza::TAB)
      (*op)<<" type='tab'";
    if (stanza->copy) (*op)<<" copy='"<<ids[stanza->copy]<<"'";
    if (stanza->chords) (*op)<<" chords='"<<ids[stanza->chords]<<"'";
    (*op)<<">\n";
    for (size_t i=0;i<stanza->verse.size();++i) {
      (*op)<<"<v>";
      writeVerse(stanza->verse[i]);
      (*op)<<"</v>\n";
    }
    (*op)<<"</stanza>\n";
  };

  void writeVerse(const PhraseList *phrase) {
    for (size_t i=0;i<phrase->list.size();++i)
      writeItem(phrase->list[i]);
  };
  
  void writeItem(const PhraseItem *item) {
    const Word *w=dynamic_cast<const Word*>(item);
    if (w) {
      (*op)<<w->word;
      return;
    } 
    
    const Modifier *m=dynamic_cast<const Modifier *>(item);
    if (m) {
      if (m->attribute==Modifier::STRUM) {
	(*op)<<"<strum>";
	writeVerse(m->child);
	(*op)<<"</strum>";
      }
      else if (m->attribute==Modifier::NOTES) {
	(*op)<<"<note>";
	writeVerse(m->child);
	(*op)<<"</note>";
      }
      else assert(false);
      return;
    }

    const Chord* c=dynamic_cast<const Chord *>(item);
    if (c) {
      (*op)<<"<c>"<<c->modifier<<"</c>";
      return;
    }
    
    const Tab* t=dynamic_cast<const Tab *>(item); 
    if (t) {
      (*op)<<"<tab />";
      return;
    }
    
    assert(false);
  };

};

// registra il plugin
const bool XngPlug::dummy=XngPlug::startup();


