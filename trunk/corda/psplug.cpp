#include <iostream>

#include "plug.h"
#include "psmedia.h"
#include "print.h"

using namespace std;

class PsPlugout: public Plugout {
private:
  static const bool dummy;
  static bool startup() {
    Plugout::Register("ps",&(PsPlugout::Create));
    return true;
  };
  static Plugout* Create() {return new PsPlugout();};

public:
  virtual void Write(std::ostream &out, const SongArray &list,
		     const PlugoutOptions &opt) {
    PsMedia m(out,opt.start_page);
    PrintSongs(list,m,&opt);
  };

  PsPlugout(): Plugout("Postscript","ps","Adobe Postscript document") {};
};

// registra il plugout

const bool PsPlugout::dummy=PsPlugout::startup();

