#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAXLINE 80

char *SUBJ[]={"##system","#debug","#hello","#bookmark",""};

struct mail
{
  char date[MAXLINE];
  char from[MAXLINE];
  char to[MAXLINE];
  char subject[MAXLINE];
  char mimeversion[MAXLINE];
  char contenttype[MAXLINE];
  char boundary[MAXLINE];
};

int is_empty(char *s)
{
  while (isspace(*s)) s++;
  return (*s=='\0');
}

char *begins(char *s,char *p)
{
  while (*s==*p) {s++;p++;}
  if (*p=='\0') return s;
  else return NULL;
}

int field_write(char *field,char *s, char *p)
{
s=begins(s,p);
if (s==NULL) 
    return 0;
strcpy(field,s);
if (strchr(field,'\n'))
  *strchr(field,'\n')='\0';
return 1;
}


void read_intro(struct mail *p,FILE *fp)
{
  char *p;
  char s[MAXLINE];
  do
    {
      fgets(s,MAXLINE-1,fp);
      field_write(p->date,s,"Date: ");
      field_write(p->from,s,"From: ");
      field_write(p->to,s,"To: ");
      field_write(p->subject,s,"Subject: ");
      field_write(p->mimeversion,s,"Mime-Version: ");
      field_write(p->contenttype,s,"Content-Type: "); 
    }
  while (!is_empty(s) && !feof(fp));
  if (p->contenttype
}
int is_in(char **list,char *s)
{
  int i;
  for (i=0;*(list[i])!='\0';i++)
    if (!strcmp(list[i],s)) return i;
  return -1;
}

#define BUFFLEN 80
void cat(FILE *fp, FILE *fout)
{
  char *buff[BUFFLEN];
  int n;
  do
    {
      n=fread(buff,sizeof(char),BUFFLEN,fp);
      fwrite(buff,sizeof(char),n,fout);
    } while (n==BUFFLEN);
}

/* torna un FILE* su cui si puo' scrivere per rispondere al mail!*/
FILE *answer(struct mail *m,char *subject)
{
  FILE *out;
  char buf[80];
  sprintf(buf,"sendmail \"%.40s\"",m->from); 
  out=popen(buf,"w");
  if (out)
    {
      if (subject)
	fprintf(out,"Subject: %s\n",subject);
      else
	fprintf(out,"Subject: RE: %s\n",m->subject);
      fprintf(out,"\n");
    }
  else
    {
      fprintf(stderr,"Non riesco ad aprire la pipe (%s)\n",buf);
    }
  return out;
}

int esegui(struct mail *m)
{
  FILE *fp=NULL;
  FILE *out;
  char *tmp;
  char buf[80];
  tmp=tmpnam(NULL);
  if (tmp) fp=fopen(tmp,"w");
  if (fp)
    {
      cat(stdin,fp);
      fclose (fp);
      sprintf(buf,"(echo 'Subject: system output';echo \"\";csh %s) |sendmail \"%s\"",tmp,m->from);
      return system(buf);
    }
  else
    {
      out=answer(m,"system: Errore");
      fprintf(out,"non riesco ad aprire il file temporaneo\n");
      fclose(out);
      return 0;
    }
}

void add_bookmark(void)
{
  FILE *fp;
  char buf[160];
  char *b,*c="new bookmark";

  fp=fopen("/home/math/paolini/.netscape/bookmarks.html","a");
  if (fp)
    {
      fgets(buf,159,stdin);
      if (strchr(buf,'\n'))
	*strchr(buf,'\n')='\0';
      for (b=buf;isspace(*b);b++);
      for (c=b;!isspace(*c) && c!='\0';c++);
      if (*c!='\0')
	{
	  *c='\0';
	  for (c=c+1;isspace(*c);c++); 
	}
      fprintf(fp,"<A HREF=\"%s\">%s</A><p>\n",b,c);
      fclose(fp);
    }
}


int main(void)
{
  FILE *fp;
  int n;
  struct  mail m;

  read_intro(&m,stdin);
  if (!strcmp(m.subject,"##system"))
    esegui(&m);
  else if (!strcmp(m.subject,"#debug"))
    system("echo mailfilter debug |write paolini");
  else if (!strcmp(m.subject,"#hello"))
    {
      fp=answer(&m,NULL);
      fprintf(fp,"Hello from mailfilter program by Emanuele Paolini\n");
      pclose(fp);
    }
  else if (!strcmp(m.subject,"#bookmark"))
    add_bookmark();
  return 0;
}
