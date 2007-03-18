#include <iostream>
#include <string>
#include <cstdio>

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>

#include "sockstream.h"

const int sockBuf::SIZE=16;

using namespace std;

int sockBuf::sync() {
  streamsize n=pptr() - pbase();
  if (n)
    if (write(_socket,pbase(),n)!=n)
      return EOF;
  return 0;
};

int sockBuf::overflow (int ch) {
  streamsize n= pptr() - pbase() ;
  if (n && sync ())
    return EOF;
  if (ch != EOF) {
    char cbuf[1];
    cbuf[0]=ch;
    if (write(_socket,cbuf,1)!=1)
      return EOF;
  }
  pbump (-n);
  return 0;
};

int sockBuf::underflow() {
  fd_set read_fd_set;
  FD_ZERO (&read_fd_set);
  FD_SET (_socket, &read_fd_set);
  if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
    perror("select");
    exit(1);
  }
  assert(gptr()==egptr());
  assert(eback()==gbuf);
  size_t m=read(_socket,gbuf,SIZE);
  if (m==0) return EOF;
  setg(gbuf,gbuf,gbuf+m);
  return 0; // return EOF??
};

sockBuf::sockBuf(int socket): _socket(socket) {
    gbuf=new char[SIZE];
    pbuf=new char[SIZE];
    setp(pbuf,pbuf+SIZE);
    setg(gbuf,gbuf,gbuf);
};

sockBuf::~sockBuf() {
  sync();
  delete[] gbuf;
  delete[] pbuf;
};
