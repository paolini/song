#include <iostream>
#include <cassert>

#include "iso.h"

using namespace std;

char iso(const string &utf8, size_t &i) {
  const string &s=utf8;
  unsigned char c=s[i];
  if ((c&128)==0) return (s[i++]);
  
  if (c>>2 == 48 && ((unsigned char)s[i+1]>>6) == 2) { 
    c=s[i+1];
    // carattere unicode a 8 bit
    if (c>>6!=2) {
      std::cerr<<"Invalid UTF8 encoding\n";
      assert(false);
      i++;
      return '!';
    }
    c=(((s[i] & 3)<<6) | (s[i+1] & 63));
//    cerr<<"iso char "<<int(s[i])<<","<<int(s[i+1])<<" = ["<<c<<"]\n";
    i+=2;
    return c;
  }
  if (c>>5 == 4+2+0 ) {
    // UNICODE a 11 bit
    c=s[i+1];
    if (c>>6!=2) {
      cerr<<"Invalid utf8 encoding\n";
      assert(false);
      i++;
      return '!';
    }
    i+=2;
    cerr<<"non 16-bit unicode caracter encountered: \n";
    return '?';
  }
  // IMPLEMENTARE caratteri a piu' di 11 bit...
  cerr<<"char '"<<s[i]<<"'="<<int((unsigned char)s[i])<<" encountered\n";
  assert(false);
};

string iso(const string &utf8) {
  string ret;
  for (size_t i=0;i<utf8.size();) {
    ret+=iso(utf8,i);
  }
  return ret;
};

int add_utf8_char(unsigned char c, string &to) {
  if ( c & 128 ) { // 2 caratteri
    //cerr<<"char "<<(unsigned int)(c)<<" to utf8\n";
    to+=char((c>>6)|(128+64));
    to+=char((c&(63))|128);
    return 2;
  } else {
    to+=char(c);
    return 1;
  }
}

