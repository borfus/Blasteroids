#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include "spaceship.h"
#include "blasteroids.h"
#include <Windows.h>
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define DEG_TO_RAD(x) ((x)*0.0174532925)
#define RAD_TO_DEG(x) ((x)*57.295779513)

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

void rotate_ship(Spaceship *s, enum keys key)
{
	if (key == LEFT)
	{
		//OutputDebugString("Going Left\n");
		s->heading -= SHIP_ROTATION_SPEED;
	}
	else
	{
		//OutputDebugString("Going Right\n");
		s->heading += SHIP_ROTATION_SPEED;
	}
}

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