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

  virtual void Write(std::ostream &out, const SongArray &list,
		     const PlugoutOptions &opt) {
    op=&out;
    (*op)<<
      "<?xml version='1.0' encoding='utf8' ?>\n"
      "<?xml-stylesheet type='text/xsl' href='songml.xsl'?>"
      "<!DOCTYPE song SYSTEM 'songml.dtd' >\n";
      for (size_t i=0;i<list.size();++i) {
	assert (list[i]!=NULL);
	writeSong(list[i],&opt);
      }
  };
  
  XngPlug(): 
    Plugout("xsong","xng","xml song format [manu-fatto]")
  {};

  void writeSong(const Song *song, const PlugoutOptions *options) {
    (*op)<<"<song xmlns='http://sourceforge.net/projects/songml'>\n";    
    if (song->head())
      writeHead(song->head());
    if (song->body())
      writeBody(song->body(),options);
    (*op)<<"</song>\n";
  };

  void writeHead(const Head *head) {
    (*op)<<"<description>\n";
    (*op)<<"<title>"<<head->title<<"</title>\n";
    for (size_t i=0;i<head->author.size();++i)
      writeAuthor(head->author[i]);
    (*op)<<"</description>\n";
  };

  void writeAuthor(const Author *author) {
    (*op)<<"<artist>"<<author->firstName<<" "
	 <<"<!-- name-->"<<author->Name<<"<!-- /name--></artist>\n";
  };

  map<const Stanza *,int> ids;

  void writeBody(const Body *body, const PlugoutOptions *options) {
    (*op)<<"<lyrics>\n";
    for (size_t i=0;i<body->size();++i)
      ids[(*body)[i]]=i;
    for (size_t i=0;i<body->size();++i)
      writeStanza((*body)[i],options);
    (*op)<<"</lyrics>\n";
  };

  void writeStanza(const Stanza *stanza, const PlugoutOptions *options) {
    (*op)<<"<stanza id='"<<ids[stanza]<<"'";
    if (stanza->type==Stanza::REFRAIN)
      (*op)<<" label='Refrain' phrase='C'";
    else if (stanza->type==Stanza::SPOKEN)
      (*op)<<" label='Talking' phrase=''";
    else if (stanza->type==Stanza::TAB)
      (*op)<<" label='Tablature' phrase=''";
    else 
      (*op)<<" label='Strophe' phrase='V'";
    if (stanza->copy) (*op)<<" repeat='"<<ids[stanza->copy]<<"'";
    //    if (stanza->chords) (*op)<<" chords='"<<ids[stanza->chords]<<"'";
    (*op)<<">\n";
    for (size_t i=0;i<stanza->size();++i) {
      (*op)<<"<line>";
      writeVerse((*stanza)[i],options);
      (*op)<<"</line>\n";
    }
    (*op)<<"</stanza>\n";
  };

  void writeVerse(const PhraseList *phrase, const PlugoutOptions *options) {
    for (size_t i=0;i<phrase->size();++i)
      writeItem((*phrase)[i],options);
  };

  void writeItem(const PhraseItem *item, const PlugoutOptions *options) {
    const Word *w=dynamic_cast<const Word*>(item);
    if (w) {
      (*op)<<w->word;
      return;
    } 
    
    const Modifier *m=dynamic_cast<const Modifier *>(item);
    if (m) {
      if (m->attribute==Modifier::STRUM) {
	(*op)<<"<!-- strum-->";
	writeVerse(m->child,options);
	(*op)<<"<!-- /strum-->";
      }
      else if (m->attribute==Modifier::NOTES) {
	(*op)<<"<!-- note-->";
	writeVerse(m->child,options);
	(*op)<<"<!-- /note-->";
      }
      else assert(false);
      return;
    }

    const Chord* c=dynamic_cast<const Chord *>(item);
    if (c) {
      (*op)<<"<chord is='"<<options->convert(*c)<<"' />";
      return;
    }
    
    const Tab* t=dynamic_cast<const Tab *>(item); 
    if (t) {
      (*op)<<"<!-- tab /-->";
      return;
    }
    
    assert(false);
  };

};

// registra il plugin
const bool XngPlug::dummy=XngPlug::startup();


