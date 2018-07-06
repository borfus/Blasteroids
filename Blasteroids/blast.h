#ifndef _BLASTH_
#define _BLASTH_
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

typedef struct
{
	float sx;
	float sy;
	float heading;
	float speed;
	float damage;
	float scale;
	ALLEGRO_COLOR color;
} Blast;

Blast create_blast(void);
void draw_blast(Blast *b);
void move_blast(Blast *b);
#endif