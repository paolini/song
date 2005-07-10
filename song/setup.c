#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LINE 256
char songdir[LINE]="C:\\SONG";
char gsview[LINE]="C:\\GSTOOLS\\GSVIEW\\GSVIEW32.EXE";

char *batchname[]={"songView.bat","song.bat","songAll.bat",0};
char *batchfile[]={
  "%SONGDIR%\\BIN\\sng2ps -output %TEMP% %1 %2 %3 %4 %5 %6 %7 %8 %9\n"
  "%GSVIEW% %TEMP%\n",
  "%SONGDIR%\\BIN\\song -config %CONFIG% %1 %2 %3 %4 %5 %6 %7 %8 %9\n",
  "%SONGDIR%\\BIN\\song -config %CONFIG% %SONGDIR%\\SONGS\\*.sng \n",0};

char *zipfile[]={"package.zip","songs.zip","stampe.zip",0};
char *zipdesc[]={"i file veramente necessari","tantissime canzoni","le pagine gia' stampate",0};

void writeSettings(FILE *fp) {
  fprintf(fp,
  "@echo off\n"
  "set TEMP=%s\\TEMP.PS\n"
  "set SONGDIR=%s\n"
  "set GSVIEW=%s\n"
  "set CONFIG=%s\\song.cfg\n",
  songdir,songdir,gsview,songdir);
}

char* ask(char *question, char *suggestion) {
  static char ans[LINE];
  char *p;
  printf("%s [%s]\n",question,suggestion);
  fgets(ans,LINE,stdin);
  while (p=strchr(ans,'\n')) *p=0;
  if (strlen(ans)==0) strcpy(ans,suggestion);
  return ans;
}

  char boh[LINE];
  char buf[2*LINE];
main() {
  int r;
  int i;
  FILE *fp;
  printf("Ciao! Vediamo se riusciamo ad installare 'sto song\n");
  printf("Prima di tutto devi installare gsview se non l'hai gia' fatto.\n");
  strcpy(boh,ask("Vuoi lanciare l'installazione di gsview?","si"));
  if (tolower(boh[0])=='s') {
    printf("faccio partire l'installazione!!\n");
    if (system("gsv26550")) {
      printf("Mi sa che l'installazione di GSVIEW non e' riuscita...\n");
      exit(1);
    }
  }
  strcpy(gsview,ask("Qual e' il path completo di gsview?",gsview));
  strcpy(songdir,ask("In che directory vuoi installare song?",songdir));
  if (mkdir(songdir,0)) {
    if (errno=EEXIST)
      printf("La directory esiste gia'... vabbe...\n");
    else {
      printf("Urka! Non riesco a creare la directory %s\n",songdir);
      perror("mkdir");
      exit(1);
    }
  }

  for (i=0;zipfile[i];i++) {
    sprintf(buf,"Vuoi installare %s (%s)?",zipdesc[i],zipfile[i]);
    strcpy(boh,ask(buf,"si"));
    if (tolower(boh[0]=='s')) {
      sprintf(buf,"unzip32 %s -d %s",zipfile[i],songdir);
      r=system(buf);
      if (r>2 || r<0) {
        printf("unzip e' uscito con un codice di errore %d\n",r);
        printf("Vabbe', facciamo finta di niente...\n");
      }
    }
  }
  printf("\nOra creo i file batch con le configurazioni giuste.\n");
  for (i=0;batchname[i];i++) {
    printf("Creo file %s\n",batchname[i]);
    sprintf(boh,"%s\\bin\\%s",songdir,batchname[i]);
    fp=fopen(boh,"wt");
    if (!fp) {
      printf("Blah, non riesco a scrivere il file %s...\n",boh);
      continue;
    }
    writeSettings(fp);
    fprintf(fp,"%s",batchfile[i]);
    fclose(fp);
  }
  printf("\n\nBene credo di aver installato tutto\n"
    "Nella directory %s\\SONGS puoi trovare un bel po' di file .sng\n"
    "Nella directory %s\\BIN puoi trovare tutti gli eseguibili.\n",songdir,
    songdir);
  printf("I file .BAT (quelli con l'ingranaggio nell'icona) \n"
    "che trovi nella directory %s\\BIN possono\n"
    "essere copiati in posti piu' comodi (ad esempio il desktop)\n",songdir);
  printf("Per stampare le canzoni apri la cartella %s\\SONGS, ",songdir);
  printf("quindi seleziona i file che vuoi stampare (usa il tasto ctrl per\n"
  "estendere la selezione) e sposta quindi i file selezionati sull'icona\n"
  "del file BAT songView\n");
  printf("Se invece vuoi visualizzare le canzoni a schermo spostale come \n"
  "prima sull'icona del file BAT song\n");
  printf("Se invece apri l'icona songAll verranno caricate e visualizzate\n"
  "a schermo tutte le canzoni disponibili\n");

  printf("\nBye!\n");
  }
    

