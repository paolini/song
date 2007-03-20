#include <sstream>
#include <sockets/HTTPSocket.h>
#include <sockets/ListenSocket.h>
#include <sockets/SocketHandler.h>

#include "song.h"
#include "htmlplugout.h"

using namespace std;

extern SongList song_list;
extern PlugoutOptions opt;
 

void decompose(const string &path, string &dir, string &name, string &ext) {
  int i,slash=0,dot=path.size()-1;
//  cout<<"decompose ["<<path<<"]\n";
  for (i=0;i<path.size();i++) {
     if (path[i]=='/') slash=i;
     else if (path[i]=='.') dot=i;                         
  }
  if (dot<slash) dot=path.size()-1;
  dir=path.substr(0,slash);
//  cout<<"dir=["<<dir<<"]\n";
  name=path.substr(slash+1,dot-slash-1);
//  cout<<"name=["<<name<<"]\n";
  ext=path.substr(dot+1);
//  cout<<"ext=["<<ext<<"]\n";
}

class ServerSocket : public HTTPSocket
{
public:
	ServerSocket(ISocketHandler& );
	~ServerSocket();

	void Init();
	void OnAccept();

	void OnFirst();
	void OnHeader(const std::string& ,const std::string& );
	void OnHeaderComplete();
	void OnData(const char *,size_t);
	void OnDetached();

};


int start_web(int port=80)
{
          
  SocketHandler h;
//ServerHandler h("config.xml");
  ListenSocket<ServerSocket> ll_open(h);

  printf("Starting....\n");

  if (ll_open.Bind("0.0.0.0",port,20)) {
    printf("bind failed\n");
    return -1;
  }
  printf("Bind done...\n");
  h.Add(&ll_open);
  bool quit = false;
  printf("Loop start...\n");
  while (!quit) {
   h.Select(1,0);
  }
}






ServerSocket::ServerSocket(ISocketHandler& h)
:HTTPSocket(h)
{
}


ServerSocket::~ServerSocket()
{
}


void ServerSocket::OnFirst()
{
	printf("IsRequest: %s\n",IsRequest() ? "YES" : "NO");
	if (IsRequest())
	{
		printf(" Method: %s\n",GetMethod().c_str());
		printf(" URL: %s\n",GetUrl().c_str());
		printf(" Http version: %s\n",GetHttpVersion().c_str());
	}

	printf("IsResponse: %s\n",IsResponse() ? "YES" : "NO");
	if (IsResponse())
	{
		printf(" Http version: %s\n",GetHttpVersion().c_str());
		printf(" Status: %s\n",GetStatus().c_str());
		printf(" Status text: %s\n",GetStatusText().c_str());
	}

}


void ServerSocket::OnHeader(const std::string& key,const std::string& value)
{
  printf("OnHeader(): %s: %s\n",key.c_str(),value.c_str());
	if (!strcasecmp(key.c_str(),"host"))
	{
	}
	else
	if (!strcasecmp(key.c_str(),"user-agent"))
	{
	}
	else
	if (!strcasecmp(key.c_str(),"referer"))
	{
	}
}


void ServerSocket::OnData(const char *,size_t)
{
  printf("ONDATA\n");
}


void ServerSocket::OnHeaderComplete()
{
  printf("ONHEADERCOMPLETE\n");
  Send("HTTP/1.0 200 OK\n");

  string dir, name, ext;
  decompose(GetUrl(),dir,name,ext);
  ostringstream buffer;
  
//  Send("<HTML><BODY>\n"
//  "DIR=["+dir+"] NAME=["+name+"] EXT=["+ext+"]<br />\n");
  opt.links=true;
  if (dir == "") { // INDEX
    HtmlPlug html;
  
    opt.index=true;
    Send("Content-type: text/html; charset: utf-8\n\n");
    html.Write(buffer,song_list,opt);
  } else if (dir == "/song") {
    opt.index=false;
    int n=atoi(name.c_str());
    if (n<0 || n>=song_list.size())
      throw runtime_error("song number out of range");
    if (ext=="html") {
      HtmlPlug html;
      Send("Content-type: text/html; charset: utf-8\n\n");
      html.writeSong(buffer,song_list[n],&opt,n);
    } else if (ext=="pdf") {
    }
  }
  Send(buffer.str());

  SetCloseAndDelete();
  if (!Detach()){
    printf("Detach() failed\n");
    SetCloseAndDelete();
  }
}


void ServerSocket::OnDetached()
{
  printf("ONDETACHED\n");
//	Deliver d(*this,GetServer(),"http",GetMethod(),GetUrl());
//	d.Run();
}


void ServerSocket::Init() 
{
//	ServerHandler& ref = static_cast<ServerHandler&>(Handler());
//	if (GetParent() -> GetPort() == ref.GetInt("secure/port"))
//	{
//		EnableSSL();
//	}
}


void ServerSocket::OnAccept() 
{
	printf("Accept!\n");
	HTTPSocket::OnAccept();
}

