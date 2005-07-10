/*FUNZIONI PER L'UTILIZZO DEL MOUSE*/

int there_is_mouse(void);
int mouse_xy(int *x,int *y);
int mouse_x(void);
int mouse_y(void);
void mouse_on(void);
void mouse_off(void);
void mouse_to(int x,int y);
void mouse_rangex(int min,int max);
void mouse_rangey(int min,int max);

void mouse_reset(void);
int mouse_pressed(int pulsante);
int mouse_getinfopressed(int *conteggio,int pulsante);
int mouse_getinfounpressed(int *conteggio,int pulsante);

