#include "webserve.h"

class CordaWeb: public WebEngine {
 public:
  virtual int answer(WebRequest &req) const;
};



