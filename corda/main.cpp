#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <cstring>

//#include "print.h"

#include "debug.h"
#include "plug.h"
// #include <libxml/parser.h>
#include "song.h"
//#include "chord.h"

//#include "cordaweb.h"

using namespace std;

string usage(
	     "song: manu-fatto (2005) VER. "+SongVersion()+"\n"
	     "description: reads sng and xng song files "
	     "and print them in ps, pdf, xml or txt\n"
	     "usage: song [options or input-files]\n\n"
	     "options:\n");

#define OPT0(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)))
#define OPT1(x,y) usage+=" " x " "  y "\n"; if (!strcmp(argv[i],(x)) && i+1<argc )

string lang=string();
char* supported_lang[]={"txt","lst","ps","pdf","xng","html",0};

string format="";

SongList song_list;
PlugoutOptions opt;
        
int listenToPort=0; // se diverso da 0 inizia un server alla porta specificata

string extension(const string &filename) {
  string::size_type i;
  for (i=filename.size();i>=1 && filename[i-1]!='.';--i);
  if (i>=1) return string(filename,i,string::npos);
  else return string();
};

extern int start_web(int port);

int main(int argc, char *argv[]) {
  try {
    //int width=79,height=250;
    //int start_page=1;
    string output_file=string();
    //    chord chord_mode;
    
    for (int i=1;i<argc;++i) {
      OPT0("-ps","forces PostScript output") {lang="ps";continue;}
      OPT0("-pdf","forces PDF output") {lang="pdf";continue;}
      OPT0("-txt","forces txt output (default)") {lang="txt";continue;}
      OPT0("-xng","forces xml output") {lang="xng";continue;}
      OPT0("-html","forces html output") {lang="html";continue;}
      OPT0("-list","list titles") {lang="lst";continue;}
      OPT0("-version","show current version") {
	cout<<SongVersion()<<"\n";exit(0);};
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
      OPT1("-pag","<page> starting page number") {
	++i;
	opt.start_page=atoi(argv[i]);
	continue;
      }
      OPT1("-chord","[OFF,IT,It,EN] chord mode (DO, Do, C)") {
	++i;
	if (!strcasecmp(argv[i],"OFF")) 
	  opt.chord_mode=PlugoutOptions::CHORD_OFF;
	else if (!strcmp(argv[i],"IT")) 
	  opt.chord_mode=PlugoutOptions::CHORD_IT;
	else if (!strcmp(argv[i],"It")) 
	  opt.chord_mode=PlugoutOptions::CHORD_It;
	else if (!strcasecmp(argv[i],"EN")) 
	  opt.chord_mode=PlugoutOptions::CHORD_EN;
	else
	  cerr<<"Unknown chord mode '"<<argv[i]<<"'\n";
	continue;
      }
      OPT1("-bass","[OFF,SLASH,BRACE] bass mode (C, C/G, C(G))") {
	++i;
	if (!strcasecmp(argv[i],"OFF"))
	  opt.bass_mode=PlugoutOptions::BASS_OFF;
	else if (!strcasecmp(argv[i],"SLASH"))
	  opt.bass_mode=PlugoutOptions::BASS_SLASH;
	else if (!strcasecmp(argv[i],"BRACE"))
	  opt.bass_mode=PlugoutOptions::BASS_BRACE;
	else
	  cerr<<"Unknown bass mode '"<<argv[i]<<"'\n";
	continue;
      }
      OPT1("-minor","[DASH,M] minor chord mode (C-, Cm)") {
	++i;
	if (!strcasecmp(argv[i],"DASH"))
	  opt.minor_mode=PlugoutOptions::MINOR_DASH;
	else if (!strcasecmp(argv[i],"M"))
	  opt.minor_mode=PlugoutOptions::MINOR_M;
	else
	  cerr<<"Unknown minor mode '"<<argv[i]<<"'\n";
	continue;
      }
      OPT0("-sng","default to sng format (default)") {
	format="sng";
	continue;
      }
      OPT0("-xng","default to xng format") {
	format="xng";
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
      OPT1("-port","<number> start web server") {
	++i;
	listenToPort=atoi(argv[i]);
	continue;
      }
      // -help dev'essere l'ultima opzione, altrimenti
      // le opzioni successive non modificano 'usage'
      OPT0("-help","print this help lines") {
	cout<<usage;
	exit(0);
      }
      if (argv[i][0]=='-') {
	cerr<<"opzione sconosciuta "<<argv[i]<<"\n";
	cout<<usage;
	abort();
      }
      
      // assume che argv[i] sia il nome di un file da leggere
      
      string name=argv[i];
      
      // legge il file "name"
      string ext=extension(name);
      if (format!="") ext=format;

      Plugin* reader=Plugin::Construct(ext);
      if (!reader) {
	cerr<<"unknown format "<<ext<<" for file "<<name<<"\n";
	abort();
      }
      //	cerr<<"Reading "<<name<<" with plugin "<<reader<<" ["<<reader->name<<"]\n";
      try {
	reader->Read(name,song_list);
      } catch (runtime_error &e) {
	cerr<<"Error reading file "<<name<<" with plugin "
	    <<reader->name<<"\n"<<e.what();
	cerr<<"!! file skipped....\n";
      }
    }
    
    // convert chords
    /*    
    for (int i=0;i<song_list.size();++i) {
      chord_mode.convert(song_list[i]);
    };
    */

    // print all songs in song_list

    if (listenToPort) {
      cerr<<"Starting web server, listening port "<<listenToPort<<"\n";
      opt.base_url="";
      try {
//	CordaWeb engine;
//	WebServer server (listenToPort,engine);
//	server.start(); // non torna piu`,,,
        start_web(listenToPort);
      } catch (runtime_error e) {
	cerr<<"ERROR: "<<e.what()<<"\n";
	abort();
      }
    }
    
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

