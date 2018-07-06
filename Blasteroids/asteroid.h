#include "blast.h"
#include "spaceship.h"
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

typedef struct
{
	float sx;
	float sy;
	float rot_heading;
	float heading;
	float speed;
	float health;
	float rot_velocity;
	float scale;
	ALLEGRO_COLOR color;
} Asteroid;

void draw_asteroid(Asteroid *a);
Asteroid create_asteroid(Spaceship *s);
void move_asteroid(Asteroid *a);
int collide_blast(Asteroid *a, Blast *b);
int collide_ship(Asteroid *a, Spaceship *s);
Asteroid split_asteroid(Asteroid *a);