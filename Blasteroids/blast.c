#include <stdio.h>
#include "blast.h"
#include <math.h>
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define RED al_map_rgb(255, 0, 0)

void draw_blast(Blast *b)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, b->heading);
	al_translate_transform(&transform, b->sx, b->sy);
	al_use_transform(&transform);
	al_draw_line(0, -2, 0, 2, b->color, 4.0f);
}

Blast create_blast()
{
	Blast blast;
	if (!&blast)
	{
		printf("Could not create blast: %s\n", strerror(errno));
		exit(1);
	}
	blast.sx = 0;
	blast.sy = 0;
	blast.heading = 0.0;
	blast.speed = 15.0;
	blast.damage = 1;
	blast.scale = 1;
	blast.color = RED;
	return blast;
}

void move_blast(Blast *b)
{
	b->sx += sin(b->heading) * b->speed;
	b->sy -= cos(b->heading) * b->speed;
}