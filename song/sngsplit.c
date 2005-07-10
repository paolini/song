#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


FILE *myopen(char *title, char *author) {
  char filename[4096];
  struct stat st;
  int rep=0;
  fprintf(stderr,"myopen(%s,%s)\n",title,author);
  sprintf(filename,"%s.sng",title);
  while(stat(filename,&st)==0) {
    fprintf(stderr,"%s duplicato...\n",filename);
    rep++;
    sprintf(filename,"%s-%d.sng",title,rep);
  }
  fprintf(stderr,"opening %s...\n",filename);
  return fopen(filename,"wt");
}

int main() {
  char line[4096];
  char title[4096];
  char author[4096];
  char buf[10][4096];
  int nbuf=0;
  FILE *out=0;

  author[0]=0;
  title[0]=0;

  while (fgets(line,sizeof(line),stdin)) {
    if (line[0]=='\\' && tolower(line[1])=='t') {
      char *p,*q;
      if (out) fclose(out);
      out=0;

      for (p=line+1;*p && isalpha(*p);p++);
      if (*p==0) strcpy(title,"unknown");
      else {
	int upper=1;
	q=title;
	while(*p && *p!='}') {
	  if (isalnum(*p)) {
	    *(q++)=upper?toupper(*p):tolower(*p);
	    upper=0;
	  }
	  else if (*p==' ') upper=1;
	  p++;
	}
	*q=0;
      }    
    } else if (line[0]=='\\' && tolower(line[1])=='a') {
      char *p,*q;
      for (p=line+1;*p && isalpha(*p);p++);
      for (;*p && !(isupper(*p) && isupper(*(p+1)));p++);
      if (*p==0) strcpy(author,"unknown");
      else {
	int upper=1; 
	q=author;
	while(*p && *p!='}' && (isalnum(*p) || ! isalpha(*p))) {
	  if (isalnum(*p)) {
	    *(q++)=upper?toupper(*p):tolower(*p);
	    upper=0;
	  } else if (*p==' ') upper=1;
	  p++;
	}
	*q=0;
      }    
    }
 
    if (out) {
      fprintf(out,"%s",line);
    } else {
      strcpy(buf[nbuf++],line);
      if (nbuf>5) {
	int i;
	if (*title==0) strcpy(title,"unknown");
	out=myopen(title,author);
	for (i=0;i<nbuf;i++)
	  fprintf(out,"%s",buf[i]);
	nbuf=0;
      }
    }
  }
  if (nbuf>0) {
    fprintf(stderr,"nonempty buffer!\n");
  }
  return 1;
}
