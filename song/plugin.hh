#ifndef _PLUGIN_HH_
#define _PLUGIN_HH_

#include <vector>
#include <stdexcept>
#include <libxml/tree.h>
#include <string>

// instanzia questa classe 
class Plugin {
public:
  typedef Plugin* ctor(void);
protected:
  static void Register(std::string ext,ctor constructor);
public:
  // Costruisce un reader per il formato ext
  static Plugin* Construct(std::string ext);

  const std::string name; // name of format
  const std::string ext; // default file extension
  const std::string description; // brief description
  
  virtual int Read(std::istream &in, std::vector<xmlNodePtr> &list) {
    throw std::logic_error(std::string(name)+" not implemented");
  };
  
  virtual int Read(std::string filename, std::vector<xmlNodePtr> &list);
  
  Plugin(std::string the_name, 
	 std::string the_ext, 
	 std::string the_description):
    name(the_name),
    ext(the_ext),
    description(the_description) {};
  virtual ~Plugin(){};
};

#endif
