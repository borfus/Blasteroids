#ifndef _SPACESHIPH_
#define _SPACESHIPH_
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

#define SHIP_MAX_SPEED 5.0
#define SHIP_ACCELERATION 0.1
#define SHIP_BRAKE_SPEED 0.1
#define SHIP_DRIFT_SPEED 0.025
#define SHIP_ROTATION_SPEED 0.05

#define GREEN al_map_rgb(0, 255, 0)
#define RED al_map_rgb(255, 0, 0)

typedef struct
{
	double sx;
	double sy;
	float heading;
	float drift_heading;
	float speed;
	bool drifting;
	int gone;
	int lives;
	ALLEGRO_COLOR color;
} Spaceship;

void draw_ship(Spaceship *s);
Spaceship create_spaceship(void);
void rotate_ship(Spaceship *s, enum keys key);
void accelerate_ship(Spaceship *s);
void decelerate_ship(Spaceship *s);
void move_ship(Spaceship *s);
void drifting_ship(Spaceship *s);
#endif