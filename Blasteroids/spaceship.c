#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>

// Local includes
#include "spaceship.h"
#include "blasteroids.h"

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.295779513)

// Draws a spaceship on the screen. Takes a spaceship address.
void draw_ship(Spaceship *s)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, s->heading);
	al_translate_transform(&transform, s->sx, s->sy);
	al_use_transform(&transform);
	al_draw_line(-8, 9, 0, -11, s->color, 2.0f);
	al_draw_line(0, -11, 8, 9, s->color, 2.0f);
	al_draw_line(-8, 9, -11, 17, s->color, 2.0f);
	al_draw_line(-11, 17, -2, 17, s->color, 2.0f);
	al_draw_line(-2, 17, -2, 9, s->color, 2.0f);
	al_draw_line(-2, 9, 2, 9, s->color, 2.0f);
	al_draw_line(2, 9, 2, 17, s->color, 2.0f);
	al_draw_line(2, 17, 11, 17, s->color, 2.0f);
	al_draw_line(11, 17, 8, 9, s->color, 2.0f);
}

// Creates and returns a spaceship struct. Always spawns in the middle of the screen.
Spaceship create_spaceship()
{
	Spaceship spaceship;
	if (!&spaceship)
	{
		printf("Could not create spaceship: %s\n", strerror(errno));
		exit(1);
	}
	spaceship.sx = SCREEN_WIDTH/2;
	spaceship.sy = SCREEN_HEIGHT/2;
	spaceship.heading = 0.0;
	spaceship.drift_heading = 0.0;
	spaceship.speed = 0.0;
	spaceship.gone = 0.0;
	spaceship.lives = 3;
	spaceship.drifting = true;
	spaceship.color = GREEN;
	return spaceship;
}

// Rotates the given spaceship based on the key being pressed. Takes a spaceship address and a keys enum.
void rotate_ship(Spaceship *s, enum keys key)
{
	if (key == LEFT)
	{
		s->heading -= SHIP_ROTATION_SPEED;
	}
	else
	{
		s->heading += SHIP_ROTATION_SPEED;
	}
}

// Accelerates the spaceship by adding the SHIP_ACCELERATION value to its current speed until it reaches
// its max possible speed with SHIP_MAX_SPEED. Takes a spaceship address.
void accelerate_ship(Spaceship *s)
{
	s->drifting = false;
	if (s->speed < SHIP_MAX_SPEED)
	{
		s->speed += SHIP_ACCELERATION;
	}
	if (s->speed > SHIP_MAX_SPEED)
	{
		s->speed = SHIP_MAX_SPEED;
	}
	s->drift_heading = s->heading;
}


// Reduces the spaceship's speed by subtracting SHIP_BRAKE_SPEED to it's current speed until it reaches 0;
// Takes a spaceship address
void decelerate_ship(Spaceship *s)
{
	if (s->speed > 0)
	{
		s->speed -= SHIP_BRAKE_SPEED;
	}
	if (s->speed < 0)
	{
		s->speed = 0;
	}
}

// Moves the spaceship based on its current_heading and speed values. The current_heading is decided based
// on whether the ship is drifting or not. The ship will drift when the player isn't pressing the accelerate button.
// Takes a spaceship address.
void move_ship(Spaceship *s)
{
	float current_heading;

	if (s->drifting)
	{
		current_heading = s->drift_heading;
	}
	else
	{
		current_heading = s->heading;
	}
	s->sx += sin(current_heading) * s->speed;
	s->sy -= cos(current_heading) * s->speed;
}

// Drifts the spaceship in the last direction it was heading in and slows it until it reaches 0 speed.
// Takes a spaceship address.
void drifting_ship(Spaceship *s)
{
	s->drifting = true;
	if (s->speed > 0)
	{
		s->speed -= SHIP_DRIFT_SPEED;
	}
	if (s->speed < 0)
	{
		s->speed = 0;
	}
}