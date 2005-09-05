#include <string>

using namespace std;

// converte la stringa UTF-8 da s a end in una string UNICODE-1

string utf8(const unsigned char *s, const unsigned char *end=0) {
  string ret;
  for (;*s && (end==0 || s<end);s++) {
      char c;
      if (*s<0x80) c=*s;
      else if ((*s>>2) == 48 && (*(s+1)>>6)==2) {
	c= ((*s & 3)<<6) | (*(s+1) & 63);
	s++;
      } else {
	c='?';
	s++;
	while((*s>>6)==2) s++;
      }
      ret+=c;
  }
  return ret;
};

