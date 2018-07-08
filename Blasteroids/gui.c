#include "spaceship.h"

#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

void draw_text(ALLEGRO_FONT *font, char *text, int x, int y, ALLEGRO_COLOR color, int flag)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_translate_transform(&transform, 0, 0);
	al_use_transform(&transform);
	al_draw_text(font, color, x, y, flag, text);
}

void draw_lives(int x, int y, int lives, float scale)
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_scale_transform(&transform, scale, scale);
	al_translate_transform(&transform, x, y);
	al_use_transform(&transform);
	int x_offset = 28;
	for (int i = 0; i < lives; i++)
	{
		al_draw_line(-8 + (x_offset * i), 9, 0 + (x_offset * i), -11, GREEN, 3.0f);
		al_draw_line(0 + (x_offset * i), -11, 8 + (x_offset * i), 9, GREEN, 3.0f);
		al_draw_line(-8 + (x_offset * i), 9, -11 + (x_offset * i), 17, GREEN, 3.0f);
		al_draw_line(-11 + (x_offset * i), 17, -2 + (x_offset * i), 17, GREEN, 3.0f);
		al_draw_line(-2 + (x_offset * i), 17, -2 + (x_offset * i), 9, GREEN, 3.0f);
		al_draw_line(-2 + (x_offset * i), 9, 2 + (x_offset * i), 9, GREEN, 3.0f);
		al_draw_line(2 + (x_offset * i), 9, 2 + (x_offset * i), 17, GREEN, 3.0f);
		al_draw_line(2 + (x_offset * i), 17, 11 + (x_offset * i), 17, GREEN, 3.0f);
		al_draw_line(11 + (x_offset * i), 17, 8 + (x_offset * i), 9, GREEN, 3.0f);
	}
}