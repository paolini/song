#ifndef STRING_MODULE
#define STRING_MODULE
struct string
{
  int nalloc; /*numero di caratteri allocati compreso il '\0'*/
  char *s; /*puntatore al primo carattere*/
};

typedef struct string STRING;

void set_block_size(int new_block_size);
STRING new_string(char *p); 
/* 
p deve puntare ad una stringa null-terminata allocata con malloc oppure NULL
*/

STRING delete_string(STRING);
STRING string_alloc(STRING,int n);
STRING string_realloc(STRING);
STRING string_cat(STRING,char *s);
STRING string_add_char(STRING,char c);
STRING string_cpy(STRING,char *s);
STRING string_insert(STRING,char *where,char c);
STRING string_merge(STRING,STRING);
#endif
