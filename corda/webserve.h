#include <iostream>
#include <string>
#include <stdexcept>

class WebRequest {
private:
  WebRequest(std::ostream &_out,std::istream &_in): out(_out), in(_in) {};
public:
  std::ostream &out;
  std::istream &in;
  std::string url;
  std::string host;
  int port;

  friend class WebServer;
};

class WebEngine {
public:
  virtual int answer(WebRequest &) const =0;
  virtual ~WebEngine(){};
};

class WebServer {
private:
  int _port;
  const WebEngine & _engine;
  bool dont_fork;
  void serve(int sock);
public:
  WebServer(int port, const WebEngine &engine, bool _dont_fork=false):
    _port(port), _engine(engine), dont_fork(_dont_fork) {};
  void start();
};
