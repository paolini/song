#ifndef _PLUGIN_HH_
#define _PLUGIN_HH_

#include <vector>
#include <stdexcept>
#include <string>


//#include <libxml/tree.h>
#include "song.h"

class PlugError: public std::runtime_error {
 public:
  unsigned int line;
  unsigned int col;
  std::string filename;
  PlugError(const std::string &mesg=std::string(),
	    int _line=0, int _col=0, 
	    const std::string &_filename=std::string()): 
    runtime_error(mesg),
    line(_line),
    col(_col),
    filename(_filename)
    {};
  virtual ~PlugError() throw () {};
};

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

  virtual int Read(std::istream &in,  SongList &list) 
    throw(PlugError)
    {
      throw PlugError("reading "+string(name)+
		      " from stdin not implemented");
    };
  
  virtual int Read(string filename, SongList &list);

  Plugin(string name,string ext,string descr);
  
  virtual ~Plugin(){};
};

class PlugoutOptions {
 public:
  enum {CHORD_OFF=0, CHORD_It, CHORD_IT, CHORD_EN} chord_mode;
  enum {BASS_OFF=0, BASS_SLASH, BASS_BRACE} bass_mode;
  enum {MINOR_DASH=0, MINOR_M} minor_mode;
  int width, height;
  int start_page;
  PlugoutOptions() {
    width=79;
    height=250;
    start_page=1;
    chord_mode=CHORD_EN;
    bass_mode=BASS_SLASH;
    minor_mode=MINOR_M;
  };
  std::string convert(const Chord &chord) const;
  std::string convert(const Note &note) const;
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

  virtual void Write(std::ostream &out, const SongArray &list, 
		     const PlugoutOptions &opt) {
    throw std::runtime_error("writing "+string(name)+
			   " to stdout not implemented");
  };

  virtual void Write(string filename, const SongArray &list,
		     const PlugoutOptions &opt);

  Plugout(string name,string ext, string descr): Plug(name,ext,descr) {};

  virtual ~Plugout(){};
};

#endif
