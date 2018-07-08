#include "spaceship.h"

#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

typedef struct
{
	float sx;
	float sy;
	ALLEGRO_COLOR color;
} Powerup;

void draw_powerup(Powerup *p);
Powerup create_powerup();
int collide_powerup(Powerup *p, Spaceship *s);