// Local includes
#include "spaceship.h"

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

// Powerup struct
typedef struct
{
	float sx;
	float sy;
	ALLEGRO_COLOR color;
} Powerup;

void draw_powerup(Powerup *p);
Powerup create_powerup();
int collide_powerup(Powerup *p, Spaceship *s);