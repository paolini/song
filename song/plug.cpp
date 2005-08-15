#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include "plug.h"
using namespace std;

int Plugin::Read(string filename, vector<xmlNodePtr> &list) {
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

void Plugout::Write(string filename, vector<xmlNodePtr> &list,
		   const PlugoutOptions &opt) {
  ofstream out(filename.c_str());
  if (!out) 
    throw runtime_error("cannot open file"+filename);
  return Write(out,list,opt);
};

map<string,Plugout::ctor *> &registry_out() {
  static map<string,Plugout::ctor *> reg;
  return reg;
};

void Plugout::Register(string ext, ctor f) {
  registry_out()[ext]=f;
  //  cerr<<"Plugout "<<ext<<" registered!\n";
}

Plugout *Plugout::Construct(string ext) {
  ctor *f;
  f=registry_out()[ext];
  if (f) return f();
  else return 0;
};
