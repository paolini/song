#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include "plug.h"
using namespace std;

Plug::Plug(string the_name, string the_ext, string the_description):
    name(the_name),
    ext(the_ext),
    description(the_description) {
  //      cerr<<"Plug("<<the_name<<"="<<name<<") "<<this<<"\n";
    };

Plugin::Plugin(string name,string ext,string descr): Plug(name,ext,descr)
 {
   // cerr<<"Plugin("<<name<<"="<<this->name<<") "<<this<<"\n";
 }

int Plugin::Read(string filename, SongList &list) {
  ifstream in(filename.c_str());
  if (!in) 
    throw runtime_error("cannot open file "+filename);
  return Read(in,list);
}

map<string,Plugin::ctor *> &registry_in() {
  static map<string,Plugin::ctor *> reg;
  return reg;
};

void Plugin::Register(string ext, ctor f) {
  registry_in()[ext]=f;
  //  cerr<<"Plugin "<<ext<<" registered!\n";
}

Plugin *Plugin::Construct(string ext) {
  ctor *f;
  f=registry_in()[ext];
  if (f) return f();
  else return 0;
}

void Plugout::Write(string filename, const SongArray &list,
		   const PlugoutOptions &opt) {
  ofstream out(filename.c_str());
  if (!out) 
    throw runtime_error("cannot open file"+filename);
  //  std::cerr<<"Plugout::Write("<<filename<<","<<list.size()<<")="
  //	   <<this<<"\n";
  Write(out,list,opt);
  //  std::cerr<<"Plugout::Write done\n";
};

map<string,Plugout::ctor *> &registry_out() {
  static map<string,Plugout::ctor *> reg;
  return reg;
};

void Plugout::Register(string ext, ctor f) {
  //  std::cerr<<"Plugout::Register("<<ext<<")\n";
  registry_out()[ext]=f;
  //  cerr<<"Plugout "<<ext<<" registered!\n";
}

Plugout *Plugout::Construct(string ext) {
  ctor *f;
  f=registry_out()[ext];
  if (f) return f();
  else return 0;
};

string PlugoutOptions::convert(const Chord &chord) const {
  string r;
  r+=convert(chord.base);
  string mod=chord.modifier;
  if (minor_mode==MINOR_DASH) {
    for (size_t i=0;i<mod.size();++i) {
      if (mod[i]=='m') mod[i]='-';
      else if (mod[i]=='M') mod[i]='+';
    }
  } else if (minor_mode==MINOR_M) {
    for (size_t i=0;i<mod.size();++i) {
      if (mod[i]=='-') mod[i]='m';
      else if (mod[i]=='+') mod[i]='M';
    }
  }
  r+=mod;
  if (chord.bass.note!=-1) { // c'e' il basso
    switch(bass_mode) {
    case BASS_OFF: 
      break;
    case BASS_SLASH:
      r+='/';
      r+=convert(chord.bass);
      break;
    case BASS_BRACE:
      r+='(';
      r+=convert(chord.bass);
      r+=')';
      break;
    }
  }
  return r;
};

string PlugoutOptions::convert(const Note &note) const {
  string r;
  if (note.note<0) return r;
  switch(chord_mode) {
  case CHORD_OFF: 
    break;
  case CHORD_It:
    r+=Note::it_names[note.note];
    break;
  case CHORD_IT:
    r+=Note::it_names[note.note];
    for (size_t i=1;i<r.size();++i) r[i]=toupper(r[i]);
    break;
  case CHORD_EN:
    r+='A'+note.note;
    break;
  }
  int a=note.aug;
  while (a>0) {
    r+='#';
    a--;
    goto fine;
  }
  while (a<0) {
    r+='b';
    a++;
  }
 fine:
  return r;
};
