
static char* conv_tab={
  "a`\340",
  "e`\350",
  "e'\351",
  "i`\354",
  "o`\362",
  "u`\371",
  "A`\300",
  "E`\310",
  "E'\311",
  "I`\314",
  "O`\322",
  "U`\331",
  0};

string convert(const struct line *p) {
  string res;
  for (int i=0;p.s[i];++i) {
    if (p.ctrl[i]==' ')
      res+=p.s[i];
    else {
      for (int j=0;
	   conv_tab[j] && 
	     !(conv_tab[j][0]==p.s[i] && conv_tab[j][1]==p.ctrl[i]);
	   ++j);
      if (conv_tab[j]) 
	res+=conv_tab[j][2];
      else {
	throw runtime_error("Carattere speciale \\"+p.ctrl[i]+p.s[i]+
			    " non supportato");
      }
    }
  }
  return res;
};
