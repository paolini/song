/* 
 * Questo file contiene funzioni utili per l'interpretazione delle query
 * nei programmi cgi-bin.
 *
 * Ecco cosa rende disponibile:
 * 
 * struct couple
 *   una lista di coppie variabile=valore 
 *
 * int couple_out_of_memory
 *   questa variabile viente posta =1 in caso non ci sia memoria sufficiente.
 *
 * struct couple *read_couple(void)
 *   legge la query con il metodo GET o POST indifferentemente, e torna una
 *   lista di coppie variabile=valore 
 *
 * char *find(struct couple *list,char *var)
 *   torna il valore della variabile var nella lista list.
 * 
 * int list_length(struct couple *list)
 *   torna il numero di coppie contenute nella lista list.
 *
 * void free_couple(struct couple *list)
 *   libera la memoria occupata dalla lista list.
 *
 * void out_html(void), void out_xbm(void), void out_gif(void)
 *   scrivono l'intestazione dei vari formati
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

struct couple
{
  char *name;
  char *value;
  struct couple *rest;
};
extern int couple_out_of_memory;


void free_couple(struct couple *p);

struct couple *delete_couple(struct couple *list,struct couple *p);

struct couple *read_stdin(int len);
struct couple *read_couple();

char *find(struct couple *p,char *name);

struct couple *find_couple(struct couple *list,char *name);

int list_length(struct couple *p);



void set_html_output(FILE* output);
FILE *get_html_output(void);

void out_html(void);

void out_xbm(void);

void out_gif(void);

void out_ps(void);

STRING url(struct couple *list);

void open_html(char *title,char *base_url);

void close_html(void);

