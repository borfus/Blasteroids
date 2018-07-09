#include <stdio.h>
#include <math.h>

// Local includes
#include "blast.h"

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define RED al_map_rgb(255, 0, 0)

// Draws the blast on the screen. Takes a blast address.
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

// Creates and returns a blast struct. Takes a scale.
Blast create_blast(int scale)
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
	blast.scale = scale;

	// Scale used to determine whether the player grabbed a powerup or not
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

// Moves the blast on screen based on its heading and speed values. Takes a blast address.
void move_blast(Blast *b)
{
	b->sx += sin(b->heading) * b->speed;
	b->sy -= cos(b->heading) * b->speed;
}