#include <iostream.h>
class MailCouple
{
protected:
  int level; // un sottocampo e' di un livello maggiore del campo padre prec.
  char *name;
  char *value;
  MailCouple *next;
public:
  friend class Mail;
  MailCouple(istream &,int level=0);
  MailCouple(char *buf,int level);
  ~MailCouple(){if (name) delete name; if (value) delete value;};
  void Out(ostream&);
};

class Mail
{  
public:
  enum state {error, preamble, body, bodyread, submail, intosubmail, end};
  // preamble: sto per leggere il preambolo
  // body: sto per leggere il body
  // submail: sto per  leggere un submail
  // intosubmail: sto leggendo un submail
  // bodyread: ho letto il body o un submail
  // end: fine
private:
  state status; 
  MailCouple *first;
  istream *in;
  Mail *father;
  Mail *son;
protected:
  void AddCouple(MailCouple *p);
  MailCouple *FindCouple(char *name);
  int IsBoundary(char *line);
  int SkipBoundary(void);
  int CheckStatus(void);
public:  
  state Status(void){CheckStatus();return status;};
  int IsMultiPart(void);
  int Good(void){CheckStatus();return status!=end && status!=error;};
  Mail(Mail *father)
  {in=father->in;first=NULL;status=preamble;this->father=father;son=NULL;};
  Mail(istream *in)
  {this->in=in;first=NULL;status=preamble;father=son=NULL;};
  char *ValueOf(char *FiledName);
  int ReadPreamble(void);
  int ReadBody(ostream &out);
  int SkipBody(void);
  Mail *ReadSubMail(void);
  ~Mail();
  void Out(ostream&);
  ostream *Answer(char* to=NULL);
};
