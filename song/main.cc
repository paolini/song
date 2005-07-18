#include <iostream>
#include <libxml/parser.h>
#include <cassert>
#include <vector>

#include "textmedia.hh"
#include "psmedia.hh"
#include "print.hh"
#include "plugin.hh"

string usage(
	     "song: manu-fatto (2005)\n"
	     "description: reads sng and xng song files "
	     "and print them in ps or txt\n"
	     "usage: song [options or input-files]\n\n"
	     "options:\n");

#define OPT0(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)))
#define OPT1(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)) && i+1<argc )

enum {TXT=0, PS, XML} lang;
enum {SNG,XNG} format=SNG;

vector<xmlNodePtr> song_list;

int main(int argc, char *argv[]) {
  int width=79,height=250;
  int start_page=1;
  for (int i=1;i<argc;++i) {
    OPT0("-ps","writes PostScript output") {lang=PS;continue;}
    OPT0("-txt","writes txt output (default)") {lang=TXT;continue;}
    OPT0("-xng","writes xml output") {lang=XML;continue;}
    OPT1("-width","<x> width of page media (TXT only)") {
      ++i;
      int n=atoi(argv[i]);
      if (n>0) width=n;
      continue;
    }
    OPT1("-height","<y> height of page media (TXT only)") {
      ++i;
      int n=atoi(argv[i]);
      if (n>0) height=n;
      continue;
    }
    OPT0("-help","print this help lines") {
      cout<<usage;
      exit(0);
    }
    OPT1("-pag","<page> starting page number") {
      ++i;
      start_page=atoi(argv[i]);
      continue;
    }
    OPT0("-sng","default to sng format (default)") {
      format=SNG;
      continue;
    }
    OPT0("-xng","default to xng format") {
      format=XNG;
      continue;
    }
    OPT1("-debug","<what> only for testing") {
      ++i;
      if (!strcmp(argv[i],"stanza")) stanza_debug=true;
      else if (!strcmp(argv[i],"verse")) verse_debug=true;
      else if (!strcmp(argv[i],"song")) song_debug=true;
      else if (!strcmp(argv[i],"body")) body_debug=true;
      else {
	cerr<<"opzione sconosciuta "<<argv[i]<<"\n";
	cout<<usage;
	abort();
      }
      continue;
    } 
    if (argv[i][0]=='-') {
      cerr<<"opzione sconosciuta "<<argv[i]<<"\n";
      cout<<usage;
      abort();
    }

    // assume che argv[i] sia il nome di un file da leggere

    char *name=argv[i];
    try {
      string ext="sng";

      if (format==XNG || strstr(name,".xng")) {
	ext="xng";
      } else if (format==SNG || strstr(name,".sng")) {
	ext="sng";
      } 
      
      Plugin* reader=Plugin::Construct(ext);
      if (!reader) {
	cerr<<"unknon format "<<ext<<" for file "<<name<<"\n";
	abort();
      }
      reader->Read(name,song_list);
      if (false) {
	xmlDocPtr doc=xmlParseFile(argv[i]);
	if (!doc) {
	  cerr<<"non riesco a leggere il file "<<argv[i]<<"\n";
	  abort();
	}
	for (xmlNodePtr p=doc->children;p;p=p->next) {
	  if (strcmp((char*)(p->name),"song")!=0) {
	    cerr<<"ignore unknown <"<<(p->name)<<" item\n";
	    continue;
	  } 
	  song_list.push_back(p);
	  // discard doc !?!?
	}
      } 
    } catch (runtime_error &e) {
      cerr<<"Error reading file "<<name<<"\n"<<e.what();
      abort();
    }
  }
  
  // print all songs in song_list
  {
    Media *m=0;
    switch(lang) {
    case TXT: m=new TextMedia(cout,width,height);break;
    case PS:  m=new PsMedia(cout,start_page);break;
    case XML: 
      unsigned int i;
      for (i=0;i<song_list.size();++i) {
	xmlDocPtr doc=xmlNewDoc(BAD_CAST "1.0");
	xmlDocSetRootElement(doc,song_list[i]);
	xmlDocDump(stdout,doc);
      }
	
      break;
    default: assert(false);
    }
    if (m)
      PrintSongs(song_list,*m);
    delete m;
  }
  
}

