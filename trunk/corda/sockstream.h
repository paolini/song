#include <iostream>
#include <string>

#include <netdb.h>
#include <sys/types.h>

class sockBuf: public std::streambuf {
  int _socket;
  static const int SIZE;
  char *gbuf;
  char *pbuf;
protected:
  int sync();
  int overflow (int ch);
  int underflow();
public:
  sockBuf(int socket);
  virtual ~sockBuf();
};

