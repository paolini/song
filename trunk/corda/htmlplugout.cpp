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
      "<html encoding='utf-8'>\n"
      "<body>";
      for (size_t i=0;i<list.size();++i) {
	assert (list[i]!=NULL);
	writeSong(out,list[i],&opt,i);
      }
  };
  
  void HtmlPlug::writeSong(ostream &out, const Song *song, const PlugoutOptions *options, int n) {
    op=&out;
    if (song->head())
      writeHead(song->head(),options,n);
    if (song->body() && !options->index)
      writeBody(song->body(),options);
  };

  void HtmlPlug::writeHead(const Head *head, const PlugoutOptions *options, int n) {
    if (options->index) { 
      if (options->links)
      (*op)<<"<a href='"<<options->base_url<<"/song/"<<n<<".html'>"
           <<head->title<<"</a>";
      else (*op)<<head->title;
    }
    else
      (*op)<<"<h1>"<<head->title<<"</h1>\n";
    for (size_t i=0;i<head->author.size();++i) {
      if (i==0) {
        if (options->index) (*op)<<" (";
        else (*op)<<"<h2>";
      }
      else (*op)<<", ";
      writeAuthor(head->author[i]);
      if (i==head->author.size()-1) {
        if (options->index) (*op)<<")";
        else (*op)<<"</h2>\n";
      }
    }
    if (options->index) (*op)<<"</br>\n";
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
    if (stanza->type==Stanza::REFRAIN)
      (*op)<<"<i>";
    for (size_t i=0;i<stanza->size();++i) {
      writeVerse((*stanza)[i],options);
    }
    if (stanza->type==Stanza::REFRAIN)
      (*op)<<"</i>";
    (*op)<<"</p>\n";
  };

  void HtmlPlug::writeVerse(const PhraseList *phrase, const PlugoutOptions *options) {
//  cerr<<"[writeverse]\n";
    if (phrase->hasChords()) {
      (*op)<<"<table cellpadding='0' cellspacing='0'><tr><td></td>";
      writePhrase(phrase,options,1);
      (*op)<<"</tr>\n<tr><td>";
      writePhrase(phrase,options,2);
      (*op)<<"</td></tr></table><br />\n";
    }
    else {
     writePhrase(phrase,options,0);
     (*op)<<"<br />\n";
    }
  };

  void HtmlPlug::writePhrase(const PhraseList *phrase, const PlugoutOptions *options, int chordmode=0) {
//  cerr<<"[writephrase]\n";
    for (size_t i=0;i<phrase->size();++i)
      writeItem((*phrase)[i],options,chordmode);
  };
// chordmode: 0 nochords, 1 chord line, 2 text line

  void HtmlPlug::writeItem(const PhraseItem *item, const PlugoutOptions *options, int chordmode=0) {
    const Word *w=dynamic_cast<const Word*>(item);
    if (w) {
      if (chordmode!=1) {
        size_t size=w->word.size();
        if (size && w->word[0]==' ') (*op)<<"&nbsp;";
        (*op)<<w->word;
        if (chordmode==2 && w->word[size-1]==' ')
          (*op)<<"&nbsp;";
      }
      return;
    } 
    
    const Modifier *m=dynamic_cast<const Modifier *>(item);
    if (m) {
      if (m->attribute==Modifier::STRUM) {
	(*op)<<"<!-- strum-->";
	writePhrase(m->child,options,chordmode);
	(*op)<<"<!-- /strum-->";
      }
      else if (m->attribute==Modifier::NOTES) {
	(*op)<<"[";
	writePhrase(m->child,options,chordmode);
	(*op)<<"]";
      }
      else assert(false);
      return;
    }

    const Chord* c=dynamic_cast<const Chord *>(item);
    if (c) {
//      (*op)<<"<chord is='"<<options->convert(*c)<<"' />";
       if (chordmode==1)
         (*op)<<"<td>"<<options->convert(*c)<<"</td>";
       if (chordmode==2)
         (*op)<<"</td><td>";
      return;
    }
    
    const Tab* t=dynamic_cast<const Tab *>(item); 
    if (t) {
      if (chordmode!=1)
        (*op)<<"<!-- tab /-->";
      return;
    }
    
    assert(false);
  };


// registra il plugin
const bool HtmlPlug::dummy=HtmlPlug::startup();

