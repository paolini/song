#include <cctype>
#include <string>
#include <iostream>
#include <cassert>

#include "chord.h"

using namespace std;

static char *it_chords[7]={"La","Si","Do","Re","Mi","Fa","Sol"};
static int tones[7]={0,2,3,5,7,8,10};

int note_cmp(const char *s,const char *q) {
  while (*s && *q && tolower(*s)==tolower(*q)) s++,q++;
  return *q==0;
};

string chord::convert(string s) {
  assert(mode!=OFF);
  
  int note;
  int aug;
  int i;
  int pos=0;
  string ret;

  //  cerr<<"[convert "<<s<<"->";

  while (pos<s.size()) {
    aug=0;
    
    while(pos<s.size() && isspace(s[pos])) ret+=s[pos++];
    if (pos==s.size()) return ret;
    for (i=0;i<7 && !note_cmp(s.c_str()+pos,it_chords[i]);++i);
    if (i<7) { // accordi italiani
      pos+=strlen(it_chords[i]);
    } else { // accordi inglesi
      i=tolower(s[0])-'a';
      if (i<0 || i>6) {
	cerr<<"[unrecognized chord "<<(s.c_str()+pos)<<"]\n";
	return s;
      }
      pos++;
    }
    
    note=tones[i];

    while (s.size()>pos && s[pos]=='#' || s[pos]=='b') {
      if (s[pos]=='#') aug++;
      else aug--;
      pos++;
    }
    
    
    if (mode==EN) {
      assert(tone==0); //tone not implemented
      ret+='A'+i; // aggiungere tone...
    } else {
      string ss;
      ss=it_chords[i];
      if (mode==it) {
	ss[0]=tolower(ss[0]);
      } else if (mode==IT) {
	for (int j=1;j<s.size();++j)
	  ss[j]=toupper(ss[j]);
      }
      ret+=ss;
    }
    for (;aug>0;aug--) ret+='#';
    for (;aug<0;aug++) ret+='b';
    while (pos<s.size() && !isspace(s[pos])) ret+=s[pos++];
  }
  //  cerr<<ret<<"]\n";
  return ret;

};

void chord::convert(xmlNodePtr p) {
  if (mode==OFF) return;
  if (p) {
    //    if (!strcmp((char *) p->name,"head")) return;
    if (!strcmp((char *) p->name,"c")) {
      p=p->children;
      if (!p) return;
      if (!p->content) return;
      string s=convert(string((char *) p->content));
      xmlFree(p->content);
      p->content=(xmlChar *) strdup(s.c_str());
    } else {
      for (p=p->children;p;p=p->next)
	convert(p);
    }
  }
};
