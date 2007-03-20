
#include <iostream>
#include <libharu.h>

#include "song.h"
#include "plug.h"
#include "pdfmedia.h"
#include "print.h"


using namespace std;

class PdfPlugout: public Plugout {
private:
  static const bool dummy;
  static bool startup() {
    Plugout::Register("pdf",&(PdfPlugout::Create));
    return true;
  };
  static Plugout* Create() {return new PdfPlugout();};

public:
  
  virtual void Write(string filename, const SongArray &list,
		     const PlugoutOptions &opt) {
    try {
      PdfMedia m(filename,opt.start_page);
      PrintSongs(list,m,&opt);
    } catch (PdfException &e) {
      throw runtime_error((string("PDF (libharu): ")+e.what()).c_str());
    }
  };

  PdfPlugout(): Plugout("PDF","pdf","Adobe PDF document") {};
};

// registra il plugout

const bool PdfPlugout::dummy=PdfPlugout::startup();

