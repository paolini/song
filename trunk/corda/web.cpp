#include <sstream>
#include <sockets/HTTPSocket.h>
#include <sockets/ListenSocket.h>
#include <sockets/SocketHandler.h>

#include "song.h"
#include "htmlplugout.h"

using namespace std;

extern SongList song_list;
extern PlugoutOptions opt;
 

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

  HtmlPlug *writer=new HtmlPlug;
  if (writer==0) {
	cerr<<"Known format HTML not supported.\n";
	abort();
      } 
  
  Send("Content-type: text/html; encoding: UTF8\n\n");
#if 0
  Send("<HTML><body>\n"
       "<h1>WEB TEST</h1>\n"
       "GET " + GetUrl() + 
       "</HTML>\n\n");
#endif
  ostringstream buffer;
  writer->Write(buffer,song_list,opt);
  Send(buffer.str());
  delete writer;

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

#if 0
void ServerSocket::Run()
{
	// use Socket::MasterHandler because we're now in a detached socket
	// Handler() will return the local sockethandler driving the socket
	// MasterHandler() returns the original sockethandler
	ServerHandler& h = static_cast<ServerHandler&>(m_socket.MasterHandler());
	std::string url = m_protocol + "://" + m_host + m_url;
	std::string host;
	int port;
	std::string url_ut;
	std::string file;
	std::string ext;
	url_this(url, host, port, url_ut, file, ext);
	std::string mime_type = h.GetMimetype(ext);
	Database *db = h.GetDatabase();
	Query q(*db);
	std::string safe_host = db -> safestr(m_host);
	std::string safe_protocol = db -> safestr(m_protocol);
	std::string safe_method = db -> safestr(m_method);
	std::string safe_url = db -> safestr(m_url);
	char sql[1000];
	sprintf(sql,"select * from url where host='%s' and protocol='%s' and method='%s' and url='%s'",
		safe_host.c_str(),
		safe_protocol.c_str(),
		safe_method.c_str(),
		safe_url.c_str());
	db::Url xx(db,sql);
	if (!xx.num)
	{
		xx.host = m_host;
		xx.protocol = m_protocol;
		xx.method = m_method;
		xx.url = m_url;
		xx.mime_type = mime_type;
	}
	xx.access_count++;
	xx.save();

	// quick'n dirty
/*
HTTP/1.1 200 OK
Date: Wed, 07 Apr 2004 13:15:18 GMT
Server: Apache/1.3.26 (Unix)
Last-Modified: Mon, 22 Mar 2004 03:11:56 GMT
ETag: "1e70c6-27cb-405e597c"
Accept-Ranges: bytes
Content-Length: 10187
Connection: close
Content-Type: text/html

*/
	db::Regel regel(*db,xx.regel);
	if (regel.num)
	{
		switch (regel.typ)
		{
		case 1: // File
			{
				std::string filename = regel.path + m_url;
				FILE *fil = fopen(filename.c_str(),"rb");
				if (fil)
				{
					char buf[4096];
					size_t n;
					m_socket.Send("HTTP/1.0 200 OK\n");
					m_socket.Send("Server: " + h.GetString("server/identity") + "\n");
					if (mime_type.size())
					{
						std::string str = "Content-type: " + mime_type;
						m_socket.Send(str + "\n");
					}
					m_socket.Send("Connection: close\n");
					m_socket.Send("\n");
					n = fread(buf, 1, 4096, fil);
					while (n > 0)
					{
//printf("read %d bytes\n",n);
						m_socket.SendBuf(buf, n);
						//
						n = fread(buf, 1, 4096, fil);
					}
					fclose(fil);
					m_socket.SetCloseAndDelete();
				}
				else
				{
					m_socket.Send("HTTP/1.0 404 Not Found\n");
					m_socket.Send("Server: " + h.GetString("server/identity") + "\n");
					m_socket.Send("Content-type: text/html\n");
					m_socket.Send("\n<html><body><h1>404 Not Found</h1></html>\n");
					m_socket.SetCloseAndDelete();
				}
			}
			break;
		case 2: // Execute
			break;
		default:
			m_socket.Send("HTTP/1.0 404 Not Found\n");
			m_socket.Send("Server: " + h.GetString("server/identity") + "\n");
			m_socket.Send("Content-type: text/html\n");
			m_socket.Send("\n<html><body><h1>404 Not Found - Resource not available</h1></html>\n");
			m_socket.SetCloseAndDelete();
		}
	}
	else
	{
		m_socket.Send("HTTP/1.0 404 Not Found\n");
		m_socket.Send("Server: " + h.GetString("server/identity") + "\n");
		m_socket.Send("Content-type: text/html\n");
		m_socket.Send("\n<html><body><h1>404 Not Found - Resource not available</h1></html>\n");
		m_socket.SetCloseAndDelete();
	}
}


#endif
