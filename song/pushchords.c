#include<stdio.h>
#include<string.h>
#include<ctype.h>

char buf1[256];
char buf2[256];
FILE *fp,*out;

void pushchords(char *c,char *s,FILE *out)
{
  int mode=0;
  int wait=0;
  int i,j;
  for(i=0,j=0;s[i] || c[j];)
    {
      if (s[i]=='\n') i++;
      if (c[j]=='\n') j++;
      if (isspace(c[j]))
	{
	  if (mode==0)
	    {
	      if (s[i])
		putc(s[i++],out);
	      if (wait>0)
		wait--;
	      else
		j++;
	    }
	  else
	    {
	      putc(']',out);
	      mode=0;
	    }
	}
      else if (c[j]=='\0')
	{
	  if (mode==1)
	    {
	    putc(']',out);
	    mode=0;
	    }
	  else if (s[i])
	    putc(s[i++],out);
	}
      else if (mode==1) 
	{
	  putc(c[j++],out);
	  wait++;
	}
      else
	{
	  putc('[',out);
	  mode=1;
	}
    }
  if (mode==1)
    putc(']',out);
  putc('\n',out);
}
/*0: punteggiatura, 1: spazi, 2:lettere minuscole, 3:letteremaiuscole, 4:totale
*/
char *categ[]={".';!?'\""," \t\n"};
char *notes[]={"do","re","mi","fa","sol","la","si",NULL};
int count[10];

void set_count(char *s)
{
  count[0]=count[1]=count[2]=count[3]=count[4]=0;
for (;*s;s++)
  {
    if (strchr(categ[0],*s))
      count[0]++;
    if (strchr(categ[1],*s))
      count[1]++;
    if (islower(*s))
      count[2]++;
    if (isupper(*s))
      count[3]++;
    count[4]++;
  }
}
int is_note(char *s)
{
  int i,j;
  for (i=0;notes[i]!=NULL;i++)
    {
      for (j=0;notes[i][j]!='\0' && tolower(notes[i][j])==tolower(s[j]);j++);
      if (notes[i][j]=='\0') return 1;
    }
  return 0;
}

int is_chords(char *t)
{
  char *s;
  for (s=t;s!=NULL && *s!='\0';)
    {
      if (*s!=' ')
	{
	  if (!is_note(s)) return 0;
	  s=strchr(s,' ');
	}
      else
	while (*s==' ' || *s=='\n') s++;
    }
  set_count(t);
  return (count[2]+count[3]>1);
}
int is_verse(char *s)
{
  if (is_chords(s)) return 0;
  set_count(s);
  return (count[2]+count[3]>1);
}

main(int argc,char *argv[])
{
  int i;
  fp=stdin;
  out=stdout;
  for (i=1;i<argc;i++)
    {
      if (i==1)
	{
	  fp=fopen(argv[i],"rt");
	  if (fp==NULL)
	    {
	      fprintf(stderr,"Non riesco ad aprire il file %s\n",argv[i]);
	      exit(1);
	    }
	}
      if (i==2)
	{
	  out=fopen(argv[2],"wt");
	  if (out==NULL)
	    {
	      fprintf(stderr,"Non riesco ad aprire il file %s\n",argv[i]);
	    }
	}
    }
  fgets(buf1,255,fp);
  while (!feof(fp))
    {
      if (is_chords(buf1))
	{
	  fgets(buf2,255,fp);
	  if (is_verse(buf2))
	    {
	      pushchords(buf1,buf2,out);
	      fgets(buf1,255,fp);
	    }
	  else
	    {
	      fputs(buf1,out);
	      strcpy(buf1,buf2);
	    }
	}
      else
	{
	  fputs(buf1,out);
	  fgets(buf1,255,fp);
	}
    }
}
