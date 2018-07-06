#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "asteroid.h"
#include "blasteroids.h"
#include "spaceship.h"
#include "blast.h"
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define SCREEN_CENTER_HEIGHT (SCREEN_HEIGHT/2)
#define SCREEN_CENTER_WIDTH (SCREEN_WIDTH/2)

#define WHITE al_map_rgb(255, 255, 255)

void draw_asteroid(Asteroid *a)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_scale_transform(&transform, a->scale, a->scale);
	al_rotate_transform(&transform, a->rot_heading);
	al_translate_transform(&transform, a->sx, a->sy);
	al_use_transform(&transform);
	al_draw_line(-15, 15, -20, 5, a->color, 2.0f);
	al_draw_line(-20, 5, -20, -10, a->color, 2.0f);
	al_draw_line(-20, -10, -10, -15, a->color, 2.0f);
	al_draw_line(-10, -15, -10, -20, a->color, 2.0f);
	al_draw_line(-10, -20, 5, -20, a->color, 2.0f);
	al_draw_line(5, -20, 20, -10, a->color, 2.0f);
	al_draw_line(20, -10, 20, -5, a->color, 2.0f);
	al_draw_line(20, -5, 15, 0, a->color, 2.0f);
	al_draw_line(15, 0, 20, 10, a->color, 2.0f);
	al_draw_line(20, 10, 10, 20, a->color, 2.0f);
	al_draw_line(10, 20, 0, 15, a->color, 2.0f);
	al_draw_line(0, 15, -15, 15, a->color, 2.0f);
}

Asteroid create_asteroid(Spaceship *s)
{
	int x_pos = s->sx;
	int y_pos = s->sy;
	int distance = 60;

	Asteroid asteroid;
	if (!&asteroid)
	{
		printf("Could not create asteroid: %s\n", strerror(errno));
		exit(1);
	}

	if (rand() % 2)
	{
		asteroid.sx = rand() % (x_pos - distance);
		if (rand() % 2)
		{
			asteroid.sy = rand() % (y_pos - distance);
		}
		else
		{
			asteroid.sy = (rand() % y_pos) + (y_pos + distance);
		}
	}
	else
	{
		asteroid.sx = (rand() % x_pos) + (x_pos + distance);
		if (rand() % 2)
		{
			asteroid.sy = rand() % (y_pos - distance);
		}
		else
		{
			asteroid.sy = (rand() % y_pos) + (y_pos + distance);
		}
	}
	asteroid.heading = (float)(rand() % 629)/100; // 6.28 roughly translates into 360 degrees
	asteroid.rot_heading = 0;
	asteroid.scale = (rand() % 3) + 1;
	if (asteroid.scale == 1)
	{
		asteroid.speed = 2.0;
		asteroid.health = 1;
		if (rand() % 2)
		{
			asteroid.rot_velocity = 0.1;
		}
		else
		{
			asteroid.rot_velocity = -0.1;
		}
	}
	else if (asteroid.scale == 2)
	{
		asteroid.speed = 1.0;
		asteroid.health = 3;
		if (rand() % 2)
		{
			asteroid.rot_velocity = 0.05;
		}
		else
		{
			asteroid.rot_velocity = -0.05;
		}
	}
	else if (asteroid.scale == 3)
	{
		asteroid.speed = 0.1;
		asteroid.health = 6;
		if (rand() % 2)
		{
			asteroid.rot_velocity = 0.02;
		}
		else
		{
			asteroid.rot_velocity = -0.02;
		}
	}
	asteroid.color = WHITE;
	return asteroid;
}

void move_asteroid(Asteroid *a)
{
	a->sx += sin(a->heading) * a->speed;
	a->sy -= cos(a->heading) * a->speed;
	a->rot_heading += a->rot_velocity;
}

int collide_blast(Asteroid *a, Blast *b)
{
	int asteroid_comp = 18 * a->scale;
	int blast_comp = 2;
	if (((b->sx + blast_comp) >= (a->sx - asteroid_comp) && (b->sx - blast_comp) <= (a->sx + asteroid_comp)) &&
		((b->sy + blast_comp) >= (a->sy - asteroid_comp) && (b->sy - blast_comp) <= (a->sy + asteroid_comp)))
	{
		return 1;
	}
	return 0;
}

int collide_ship(Asteroid *a, Spaceship *s)
{
	if (a)
	{
		int asteroid_comp = 18 * a->scale;
		int spaceship_comp = 11;
		if (((s->sx + spaceship_comp) >= (a->sx - asteroid_comp) && (s->sx - spaceship_comp) <= (a->sx + asteroid_comp)) &&
			((s->sy + spaceship_comp) >= (a->sy - asteroid_comp) && (s->sy - spaceship_comp) <= (a->sy + asteroid_comp)))
		{
			return 1;
		}
	}
	return 0;
}

void normalize_stats(Asteroid *a, float current_scale)
{
	if (current_scale == 2)
	{
		a->scale = 1;
		a->speed = 2.0;
		a->health = 1;
		if (rand() % 2)
		{
			a->rot_velocity = 0.1;
		}
		else
		{
			a->rot_velocity = -0.1;
		}
	}
	else if (current_scale == 3)
	{
		a->scale = 2;
		a->speed = 1.0;
		a->health = 3;
		if (rand() % 2)
		{
			a->rot_velocity = 0.05;
		}
		else
		{
			a->rot_velocity = -0.05;
		}
	}
}

Asteroid split_asteroid(Asteroid *a)
{
	float current_scale = a->scale;
	normalize_stats(a, current_scale);
	Asteroid new_a;
	new_a.scale = a->scale;
	new_a.sx = a->sx;
	new_a.sy = a->sy;
	new_a.rot_heading = 0 - (a->rot_heading);
	new_a.heading = (a->heading - 3.14);
	new_a.rot_velocity = a->rot_velocity;
	new_a.health = a->health;
	new_a.speed = a->speed;
	new_a.color = a->color;
	return new_a;
}