#include <iostream>
#include <libxml/parser.h>
#include <cassert>
#include <vector>
#include <fstream>

#include "textmedia.hh"
#include "psmedia.hh"
#include "pdfmedia.hh"
#include "print.hh"
#include "plugin.hh"
#include "util.hh"

#ifndef VERSION
#define VERSION "unknow song version"
#endif

#define XNG_VERSION "0.1"
#define XNG_DTD "http://www.math.unifi.it/paolini/src/song/xng-" XNG_VERSION ".dtd"


string usage(
	     "song: manu-fatto (2005)\n"
	     VERSION "\n"
	     "description: reads sng and xng song files "
	     "and print them in ps, pdf, xml or txt\n"
	     "usage: song [options or input-files]\n\n"
	     "options:\n");

#define OPT0(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)))
#define OPT1(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)) && i+1<argc )

enum {DEFAULT=0, TXT, PS, PDF, XML, LIST} lang;
enum {SNG,XNG} format=SNG;

vector<xmlNodePtr> song_list;

string extension(const string &filename) {
  string::size_type i;
  for (i=filename.size()-1;i>=0 && filename[i]!='.';--i);
  if (i>=0) return string(filename,i+1,string::npos);
  else return string();
};

int main(int argc, char *argv[]) {
  int width=79,height=250;
  int start_page=1;
  string output_file=string();
  
  for (int i=1;i<argc;++i) {
    OPT0("-ps","forces PostScript output") {lang=PS;continue;}
    OPT0("-pdf","forces PDF output") {lang=PDF;continue;}
    OPT0("-txt","forces txt output (default)") {lang=TXT;continue;}
    OPT0("-xng","forces xml output") {lang=XML;continue;}
    OPT0("-list","list titles") {lang=LIST;continue;}
    OPT0("-version","show current version") {cout<<VERSION<<"\n";exit(0);};
    OPT1("-o","<output-file> name of output file") {
      ++i;
      output_file=string(argv[i]);
      if (lang==DEFAULT) {
	string ext=extension(output_file);
	if (ext=="ps") lang=PS;
	if (ext=="pdf") lang=PDF;
	if (ext=="xng") lang=XML;
	if (ext=="txt") lang=TXT;
      }
      continue;
    };
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
  
  ostream *file=0;

  // print all songs in song_list
  try {
    Media *m=0;
    switch(lang) {
    case DEFAULT:
    case TXT: 
      if (output_file.size()) {
	file= new ofstream(output_file.c_str());
	m=new TextMedia(*file,width,height);
      }
      m=new TextMedia(cout,width,height);
      break;
    case PS:  
      {
	if (output_file.size()) {
	  file=new ofstream(output_file.c_str());
	  m=new PsMedia(*file,start_page);
	} else
	  m=new PsMedia(cout,start_page);
      }
      break;
    case PDF: 
      if (output_file==string())
	throw runtime_error("PDF: output to stdout not supported\n");
      m=new PdfMedia(output_file);
      break;
    case XML: 
      unsigned int i;
      for (i=0;i<song_list.size();++i) {
	xmlDtdPtr dtd;
	xmlDocPtr doc;
	doc=xmlNewDoc(BAD_CAST "1.0");
	dtd=xmlNewDtd(doc,BAD_CAST "song",BAD_CAST XNG_DTD,0);
	doc->extSubset=dtd;
	doc->children=(xmlNodePtr) dtd;
	xmlDocSetRootElement(doc,song_list[i]);
	xmlDocDump(stdout,doc);
      }
      break;
    case LIST:
      {
	unsigned int i;
	for (i=0;i<song_list.size();++i) {
	  xmlNodePtr p=song_list[i];
	  string title="no title",author="no author";
	  for (p=p->children;p && strcmp((const char *)p->name,"head");p=p->next);
	  if (p) {
	    for (p=p->children;p;p=p->next) {
	      if (!strcmp((const char *)p->name,"title") && p->children) {
		xmlChar *s=xmlNodeListGetString(NULL,p->children,1);
		title=utf8(s);
		xmlFree(s);
	      }
	      if (!strcmp((const char *)p->name,"author") && p->children) {
		xmlChar *s=xmlNodeListGetString(NULL,p->children,1);
		author=utf8(s);
		xmlFree(s);
	      }
	    }
	  }
	  cout<<title<<" ("<<author<<")\n";
	}
      }
      break;
    default: assert(false);
    }
    if (m) {
	PrintSongs(song_list,*m);
    }
    delete m;
  } catch (
	   //runtime_error &e
	   PdfException &e
	   ) {
    cerr<<"PDFLIB: "<<e.what()<<"\n";
    abort();
  }
  if (file) delete file;

  return 0;
}

