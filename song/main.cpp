#include <iostream>
#include <libxml/parser.h>
#include <cassert>
#include <vector>
#include <string>

//#include "print.h"
#include "debug.h"
#include "plug.h"
#include "chord.h"


#ifndef VERSION
#define VERSION "unknow song version"
#endif

using namespace std;

string usage(
	     "song: manu-fatto (2005) VER. "
	     VERSION "\n"
	     "description: reads sng and xng song files "
	     "and print them in ps, pdf, xml or txt\n"
	     "usage: song [options or input-files]\n\n"
	     "options:\n");

#define OPT0(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)))
#define OPT1(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)) && i+1<argc )

string lang=string();
char* supported_lang[]={"txt","lst","ps","pdf","xng",0};

enum {SNG,XNG} format=SNG;

vector<xmlNodePtr> song_list;

string extension(const string &filename) {
  string::size_type i;
  for (i=filename.size()-1;i>=0 && filename[i]!='.';--i);
  if (i>=0) return string(filename,i+1,string::npos);
  else return string();
};

int main(int argc, char *argv[]) {
  try {
    //int width=79,height=250;
    //int start_page=1;
    string output_file=string();
    chord chord_mode;
    
    PlugoutOptions opt;
        
    for (int i=1;i<argc;++i) {
      OPT0("-ps","forces PostScript output") {lang="ps";continue;}
      OPT0("-pdf","forces PDF output") {lang="pdf";continue;}
      OPT0("-txt","forces txt output (default)") {lang="txt";continue;}
      OPT0("-xng","forces xml output") {lang="xng";continue;}
      OPT0("-list","list titles") {lang="lst";continue;}
      OPT0("-version","show current version") {cout<<VERSION<<"\n";exit(0);};
      OPT1("-o","<output-file> name of output file") {
	++i;
	output_file=string(argv[i]);
	if (lang==string()) {
	  lang=extension(output_file);
	}
	continue;
      };
      OPT1("-width","<x> width of page media (TXT only)") {
	++i;
	int n=atoi(argv[i]);
	if (n>0) opt.width=n;
	continue;
      }
      OPT1("-height","<y> height of page media (TXT only)") {
	++i;
	int n=atoi(argv[i]);
	if (n>0) opt.height=n;
	continue;
      }
      OPT0("-help","print this help lines") {cout<<usage;exit(0);}
      OPT1("-pag","<page> starting page number") {
	++i;
	opt.start_page=atoi(argv[i]);
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
	else if (!strcmp(argv[i],"layout")) layout_debug=true;
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

      // legge il file "name"
      try {
	string ext="sng";
	
	if (format==XNG || strstr(name,".xng")) {
	  ext="xng";
	} else if (format==SNG || strstr(name,".sng")) {
	  ext="sng";
	} 
	
	Plugin* reader=Plugin::Construct(ext);
	if (!reader) {
	  cerr<<"unknown format "<<ext<<" for file "<<name<<"\n";
	  abort();
	}
	reader->Read(name,song_list);
      } catch (runtime_error &e) {
	cerr<<"Error reading file "<<name<<"\n"<<e.what();
	cerr<<"!! file skipped....\n";
      }
    }
    
    // convert chords
    
    for (int i=0;i<song_list.size();++i) {
      chord_mode.convert(song_list[i]);
    };
    
    // print all songs in song_list
    
    {
      if (lang==string()) lang="txt";
      {
	int i;
	for (i=0;supported_lang[i] && supported_lang[i]!=lang;++i);
	if (supported_lang[i]==0) {
	  cerr<<"Format "<<lang<<" unknown (file "<<output_file<<").\n";
	  abort();
	}
      }
      Plugout *writer=Plugout::Construct(lang);
      if (!writer) {
	cerr<<"Known format "<<lang<<" not supported.\n";
	abort();
      } 
      if (output_file==string()) 
	writer->Write(cout,song_list,opt);
      else
	writer->Write(output_file,song_list,opt);
      delete writer;
    }
    
  } catch (runtime_error &e) {
    cerr<<"ERROR CAUGHT: "<<e.what()<<"\n";
    abort();
  }
  return 0;
}

