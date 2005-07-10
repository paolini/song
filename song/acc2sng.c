#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main()
{
  int c,d,e;
  FILE *fp,*out;
  fp=stdin;
  out=stdout;
  while ((c=fgetc(fp))!=EOF)
    {
      if (strchr("aeiouAEIOU",c))
	{
	  d=fgetc(fp);
	  if (strchr("'`",d))
	    {
	     e=fgetc(fp);
	     if (strchr(" \n",e))
	       fprintf(out,"\\%c%c%c",(char)d,(char)c,(char)e);
	     else
	       fprintf(out,"%c%c%c",(char)c,(char)d,(char)e);
	    }
	  else
	    fprintf(out,"%c%c",(char)c,(char)d);
	}
      else
	fputc(c,out);
    }
}
