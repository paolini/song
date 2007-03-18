#include <cassert>

#include "cordaweb.h"
#include "song.h"
#include "plug.h"

using namespace std;

extern SongList song_list;
extern PlugoutOptions opt;

string path, name, ext;

void write_item(ostream &out, const PhraseItem *item) {
  {
    const Word *word=item->wordp();
    if (word) {
      out<<word->word;
    }
  }
}

void write_phrase(ostream &out, const PhraseList *phrase) {
  for (unsigned i=0;i<phrase->size();++i)
    write_item(out,(*phrase)[i]);
}

void write_song(ostream &out, unsigned n) {
  const Song *song=0;
  if (n>=song_list.size()) {
    out<<"No song n. "<<n<<"\n";
    return;
  }
  song=song_list[n];
  out<<"<h1>"<<song->head()->title<<"</h2>\n";
  unsigned j;
  for (j=0;j<song->head()->author.size();++j){
    if (j==0) out<<"<h2>";
    else out<<", ";
    string s=song->head()->author[j]->firstName;
    if (s.size()) s+=" ";
    s+=song->head()->author[j]->Name;
    out<<s;
  }
  if (j) out<<"</h2>\n";
  for (j=0;j<song->body()->size();++j) {
    const Stanza *stanza=(*song->body())[j];
    out<<"<p>\n";
    
    for (unsigned k=0;k<stanza->size();++k) {
      if (k) out<<"<br />\n";
      write_phrase(out,(*stanza)[k]);
    }
    out<<"</p>\n";
  }
}

void write_list(ostream &out) {
  unsigned i;
  for (i=0;i<song_list.size();++i) {
    const Song* p=song_list[i];
    assert(p->head()!=0);
    out<<"<a href='/song/"<<i<<".html'>"
       <<p->head()->title<<"</a> ";

    size_t j;
    for (j=0;j<p->head()->author.size();++j){
      if (j==0) out<<" (";
      else out<<", ";
      string s=p->head()->author[j]->firstName;
      if (s.size()) s+=" ";
      s+=p->head()->author[j]->Name;
      out<<s;
    }
    if (j) out<<")<br />\n";
  }
}


int CordaWeb::answer(WebRequest &req) const {
  cerr<<"REQUEST: "<<req.url<<"\n";
  req.out<<
    "Content-type: text/html; charset=UTF-8\n\n"
    "<html><body><h1>CordaWeb</h1>\n"
    "<p>Pagina richiesta: "<<req.url<<"</p>\n";

  // split URL
  {
    unsigned int i;
    int dash=-1;
    int dot=-1;
    string url=req.url;

    name=path=ext=string();
    
    for (i=0;i<url.size();++i) {
      if (url[i]=='/') dash=i;
      if (url[i]=='.') dot=i;
    }
    if (dash>=0) {
      path=url.substr(0,dash);
      url=string(url.substr(dash+1));
      dot-=dash+1;
    }
    if (dot>=0) {
      ext=url.substr(dot+1);
      url=url.substr(0,dot);
    }
    name=url;
  }
  req.out<<"PATH ["<<path<<"], NAME ["<<name<<"], EXT ["<<ext<<"]<br />\n";
  
  req.out<<"[<a href='/titles/'>full list</a>]<br />\n";
  if (path == "/titles")
    write_list(req.out);
  else if (path == "/song") {
    if (ext == "html") {
      write_song(req.out,atoi(name.c_str()));
    }
  }
  return 0;
};
