#ifndef CHORDS_HEADER
#define CHORDS_HEADER

#define TUTTO_MAIUSCOLO 1
#define MAIUSCOLO 2
#define BASSO_TRA_PARENTESI 4
#define BASSO_TUTTO_MAIUSCOLO 8
#define BASSO_MAIUSCOLO 16
#define MINORE_MENO 32
#define ENGLISH 128

#define LASCIA 64

#define NORMALE (LASCIA|MAIUSCOLO|BASSO_TRA_PARENTESI|MINORE_MENO)

char *converti(char *s,int alzo, int mode);
/*converti converte la stringa s alzandola di
alzo in modalita' mode e alloca lo spazio per
la stringa risultato che poi torna*/

#endif
