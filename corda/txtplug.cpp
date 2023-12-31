#include <iostream>

#include "plug.h"
#include "textmedia.h"
#include "print.h"

using namespace std;

class TxtPlugout: public Plugout {
private:
  static const bool dummy;
  static bool startup() {
    Plugout::Register("txt",&(TxtPlugout::Create));
    return true;
  };
  static Plugout* Create() {return new TxtPlugout();};

public:
  virtual void Write(std::ostream &out, const SongArray &list,
		     const PlugoutOptions &opt) {
    //    std::cerr<<"TxtPlug::Write("<<list.size()<<")\n";
    TextMedia m(out,opt.width,opt.height);
    PrintSongs(list,m,&opt);
  };

  TxtPlugout(): Plugout("Text","txt","simple text") {
    //    std::cerr<<"TxtPlugout()="<<this<<"\n";
  };
};

// registra il plugout

const bool TxtPlugout::dummy=TxtPlugout::startup();

