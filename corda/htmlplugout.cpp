#include <cstdio>
#include <vector>
#include <iostream>
#include <cassert>
#include <map>

#include "song.h"
#include "plug.h"

#include "htmlplugout.h"

using namespace std;

bool HtmlPlug::startup() {
    Plugout::Register("html",&(HtmlPlug::Createout));
    return true;
  };

void HtmlPlug::Write(std::ostream &out, const SongArray &list,
		     const PlugoutOptions &opt) {
    op=&out;
    (*op)<<
      "<html encoding='utf8'>\n"
      "<body>";
      for (size_t i=0;i<list.size();++i) {
	assert (list[i]!=NULL);
	writeSong(list[i],&opt);
      }
  };
  
  void HtmlPlug::writeSong(const Song *song, const PlugoutOptions *options) {
    if (song->head())
      writeHead(song->head());
    if (song->body())
      writeBody(song->body(),options);
  };

  void HtmlPlug::writeHead(const Head *head) {
    (*op)<<"<h1>"<<head->title<<"</h1>\n";
    for (size_t i=0;i<head->author.size();++i) {
      if (i==0) (*op)<<"<h2>";
      else (*op)<<", ";
      writeAuthor(head->author[i]);
      if (i==head->author.size()-1) (*op)<<"</h2>\n";
    }
  };

  void HtmlPlug::writeAuthor(const Author *author) {
    (*op)<<author->firstName<<" "
	 <<author->Name;
  };

  void HtmlPlug::writeBody(const Body *body, const PlugoutOptions *options) {
    for (size_t i=0;i<body->size();++i)
      ids[(*body)[i]]=i;
    for (size_t i=0;i<body->size();++i)
      writeStanza((*body)[i],options);
  };

  void HtmlPlug::writeStanza(const Stanza *stanza, const PlugoutOptions *options) {
    (*op)<<"<p id='"<<ids[stanza]<<"'";
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
      writeVerse((*stanza)[i],options);
      (*op)<<"<br />\n";
    }
    (*op)<<"</p>\n";
  };

  void HtmlPlug::writeVerse(const PhraseList *phrase, const PlugoutOptions *options) {
    for (size_t i=0;i<phrase->size();++i)
      writeItem((*phrase)[i],options);
  };

  void HtmlPlug::writeItem(const PhraseItem *item, const PlugoutOptions *options) {
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


// registra il plugin
const bool HtmlPlug::dummy=HtmlPlug::startup();

