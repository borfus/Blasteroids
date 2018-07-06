#include "spaceship.h"
#define ALLEGRO_STATICLINK
#include <allegro5/allegro_font.h>

void draw_text(ALLEGRO_FONT *font, char *text, int x, int y, ALLEGRO_COLOR color, int flag);
void draw_lives(int x, int y, int lives, float scale);