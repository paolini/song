#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include "plugin.hh"
using namespace std;

int Plugin::Read(string filename, vector<xmlNodePtr> &list) {
  ifstream in(filename.c_str());
  if (!in) 
    throw runtime_error("cannot open file "+filename);
  return Read(in,list);
}

map<string,Plugin::ctor *> &registry() {
  static map<string,Plugin::ctor *> reg;
  return reg;
}

void Plugin::Register(string ext, ctor f) {
  registry()[ext]=f;
  //  cerr<<"Plugin "<<ext<<" registered!\n";
}

Plugin *Plugin::Construct(string ext) {
  ctor *f;
  f=registry()[ext];
  if (f) return f();
  else return 0;
}
