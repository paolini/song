#include <stdio.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "webserve.h"
#include "sockstream.h"

using namespace std;

static int make_socket (uint16_t port) {
  int sock;
  struct sockaddr_in name;
  
  /* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)  {
    throw runtime_error(string("socket: ")+strerror(errno));
    //  perror ("socket");
    //  exit (EXIT_FAILURE);
    }
  
  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
    throw runtime_error(string("bind: ")+strerror(errno));
  }
  
  return sock;
}

void WebServer::serve(int sock) {
  struct sockaddr_in clientname;
  size_t size = sizeof (clientname);
  int filedes;

  filedes = accept (sock,
		    (struct sockaddr *) &clientname,
		    &size);
  if (filedes < 0) {
      perror ("accept");
      exit (EXIT_FAILURE);
  }
  
  {
    sockBuf myBuf(filedes);
    ostream myout(&myBuf);
    istream in(&myBuf);
    WebRequest Req(myout,in);

    Req.host=string(inet_ntoa(clientname.sin_addr));
    Req.port=ntohs(clientname.sin_port);
 
    /*    string word;
    if (in>>word && word=="GET" && in>>Req.url) {
      cerr<<"GET "<<Req.url<<"\n";
      in.ignore(4096,'\n');
    */
    Req.url=string();

    string line;
    getline(in,line);
    if (line.substr(0,4)==string("GET ")) {
      unsigned int end;
      for (end=4;end<line.size() && !isblank(line[end]);end++);
      Req.url=line.substr(4,end-4);
      while (getline(in,line) && line.size() && line[0]!='\n' && line[0]!='\r') {
	cerr<<"HEADER: "<<line<<"\n";
      }
      Req.out<<"HTTP/1.0 200 OK\n";
      _engine.answer(Req);
    } else 
      cerr<<"Bad http request ["<<line<<"] from "<<Req.host<<"\n";
  }
  close(filedes);
}

void WebServer::start() {
  int sock;
  
  /* Create the socket and set it up to accept connections. */
  sock = make_socket (_port);
  if (listen (sock, 1) < 0) {
    perror ("listen");
    exit (EXIT_FAILURE);
  }
  
  /* Initialize the set of active sockets. */
  fd_set read_fd_set;
  FD_ZERO (&read_fd_set);
  FD_SET (sock, &read_fd_set);
  
  while (1) {
      /* Block until input arrives on one or more active sockets. */
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
	{
	  perror ("select");
	  exit (EXIT_FAILURE);
	}
      
      /* Connection request on original socket. */
      if (dont_fork || fork()==0) {
	serve(sock);
	if (!dont_fork) exit(0);
      }
    }
}

