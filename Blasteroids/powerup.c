// Local includes
#include "powerup.h"
#include "blasteroids.h"
#include "spaceship.h"

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define YELLOW al_map_rgb(255, 255, 0)

// Draws the powerup on screen. Takes a powerup address.
void draw_powerup(Powerup *p)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_translate_transform(&transform, p->sx, p->sy);
	al_use_transform(&transform);
	al_draw_line(-10, 20, -5, 0, p->color, 2.0f);
	al_draw_line(-5, 0, -13, 0, p->color, 2.0f);
	al_draw_line(-13, 0, -8, -20, p->color, 2.0f);
	al_draw_line(-8, -20, 8, -20, p->color, 2.0f);
	al_draw_line(8, -20, 0, -5, p->color, 2.0f);
	al_draw_line(0, -5, 10, -5, p->color, 2.0f);
	al_draw_line(10, -5, -10, 20, p->color, 2.0f);
}

// Creates and returns a powerup struct with random x and y values
Powerup create_powerup()
{
	Powerup p;
	p.sx = (rand() % (SCREEN_WIDTH - 15)) + 16;
	p.sy = (rand() % (SCREEN_HEIGHT - 20)) + 21;
	p.color = YELLOW;
	return p;
}

// Checks for powerup and spaceship collision. Takes a powerup and spaceship address.
int collide_powerup(Powerup *p, Spaceship *s)
{
	// Compensation values to account for the size of the structs
	int spaceship_comp = 11;
	int powerup_comp_x = 11;
	int powerup_comp_y = 20;
	if (((s->sx + spaceship_comp) >= (p->sx - powerup_comp_x) && (s->sx - spaceship_comp) <= (p->sx + powerup_comp_x)) &&
		((s->sy + spaceship_comp) >= (p->sy - powerup_comp_y) && (s->sy - spaceship_comp) <= (p->sy + powerup_comp_y)))
	{
		return 1;
	}
	return 0;
}