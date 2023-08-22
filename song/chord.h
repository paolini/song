#include <string>
#include <libxml/parser.h>

class chord {
 private:
  std::string convert(std::string s);
  enum {OFF=0,It,IT,it,EN} mode;
  int tone;
 public:
  void convert(xmlNodePtr song);
  chord() {
    mode=It;
    tone=0;
  };
};
