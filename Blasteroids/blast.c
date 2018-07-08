#include <stdio.h>
#include <math.h>

#include "blast.h"

#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define RED al_map_rgb(255, 0, 0)

void draw_blast(Blast *b)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_rotate_transform(&transform, b->heading);
	al_scale_transform(&transform, b->scale, b->scale);
	al_translate_transform(&transform, b->sx, b->sy);
	al_use_transform(&transform);
	al_draw_line(0, -2, 0, 2, b->color, 4.0f);
}

Blast create_blast(int scale)
{
	Blast blast;
	if (!&blast)
	{
		//printf("Could not create blast: %s\n", strerror(errno));
		exit(1);
	}
	blast.sx = 0;
	blast.sy = 0;
	blast.heading = 0.0;
	blast.speed = 15.0;
	blast.scale = scale;
	if (scale == 1)
	{
		blast.damage = 1;
	}
	else
	{
		blast.damage = 3;
	}
	
	blast.color = RED;
	return blast;
}

void move_blast(Blast *b)
{
	b->sx += sin(b->heading) * b->speed;
	b->sy -= cos(b->heading) * b->speed;
}