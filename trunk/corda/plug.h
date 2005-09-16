#ifndef _PLUGIN_HH_
#define _PLUGIN_HH_

#include <vector>
#include <stdexcept>
#include <string>


//#include <libxml/tree.h>
#include "song.h"

// classe base per Plugin e Plugout. Descrive un formato di input e/o output
class Plug {
 public:
  typedef std::string string;
 public:
  const string name; // name of format
  const string ext; // default file extension
  const string description; // brief description  
  Plug(string the_name, string the_ext, string the_description);
  virtual ~Plug(){};
};

// instanzia questa classe per avere un nuovo formato di input
class Plugin: public Plug {
public:
  typedef Plugin* ctor(void);
protected:
  static void Register(string ext,ctor constructor);
public:
  // Costruisce un reader per il formato ext
  static Plugin* Construct(string ext);

  virtual int Read(std::istream &in,  SongList &list) {
    throw std::logic_error("reading "+string(name)+
			   " from stdin not implemented");
  };
  
  virtual int Read(string filename, SongList &list);

  Plugin(string name,string ext,string descr);
  
  virtual ~Plugin(){};
};

class PlugoutOptions {
 public:
  int width, height;
  int start_page;
  PlugoutOptions() 
    {
      width=79;
      height=250;
      start_page=1;
    };
};

class Plugout: public Plug {
 public:
  typedef Plugout *ctor(void);
 protected:
  // ogni classe derivata deve registrare il proprio costruttore
  static void Register(string ext,ctor constructor);
 public:
  // costruisce un writer per il formato ext
  static Plugout* Construct(string ext);

  virtual void Write(std::ostream &out, const SongList &list, 
		     const PlugoutOptions &opt) {
    throw std::logic_error("writing "+string(name)+
			   "to stdout not implemented");
  };

  virtual void Write(string filename, const SongList &list,
		     const PlugoutOptions &opt);

  Plugout(string name,string ext, string descr): Plug(name,ext,descr) {};

  virtual ~Plugout(){};
};

#endif
