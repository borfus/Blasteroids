#ifndef _BLASTH_
#define _BLASTH_

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

// Blast struct
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

Blast create_blast(int scale);
void draw_blast(Blast *b);
void move_blast(Blast *b);
#endif