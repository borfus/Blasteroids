//#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <stdlib.h>
//#include <crtdbg.h>  
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "asteroid.h"
#include "spaceship.h"
#include "blasteroids.h"
#include "blast.h"
#include "gui.h"
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define TOTAL_POSSIBLE_BLASTS 30
#define TOTAL_POSSIBLE_ASTEROIDS 40
#define STARTING_ASTEROID_NUM 10
#define BLACK al_map_rgb(0, 0, 0)
#define RED_TEXT al_map_rgb(255, 17, 30)

void error(char *msg)
{
	printf("%s: %s\n", msg, strerror(errno));
	exit(1);
}

typedef struct
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool space;
} key;

key keys = { false, false, false, false, false };
ALLEGRO_DISPLAY *display;
bool closed = false;
bool shot = false;
bool restart = false;
unsigned int score = 0;

void* read_key_events(ALLEGRO_THREAD *thread, void *a)
{
	for (;;)
	{
		if (!closed)
		{
			ALLEGRO_EVENT_QUEUE *queue;
			queue = al_create_event_queue();
			al_register_event_source(queue, al_get_keyboard_event_source());
			al_register_event_source(queue, al_get_joystick_event_source());
			al_register_event_source(queue, al_get_display_event_source(display));
			ALLEGRO_EVENT event;
			al_wait_for_event(queue, &event);

			// Keyboard/Joystick events down
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
			{
				switch (event.keyboard.keycode)
				{
					case ALLEGRO_KEY_A:
					case ALLEGRO_KEY_LEFT:
					{
						keys.right = false;
						keys.left = true;
					} break;

					case ALLEGRO_KEY_D:
					case ALLEGRO_KEY_RIGHT:
					{
						keys.left = false;
						keys.right = true;
					} break;

					case ALLEGRO_KEY_W:
					case ALLEGRO_KEY_UP:
					{
						keys.up = true;
						keys.down = false;
					} break;

					case ALLEGRO_KEY_S:
					case ALLEGRO_KEY_DOWN:
					{
						keys.down = true;
						keys.up = false;
					} break;

					case ALLEGRO_KEY_J:
					case ALLEGRO_KEY_SPACE:
					case ALLEGRO_KEY_Z:
					{
						shot = true;
						//keys.space = true;
					} break;

					case ALLEGRO_KEY_ESCAPE:
					{
						closed = true;
					} break;

					case ALLEGRO_KEY_R:
					{
						restart = true;
					}

					default:
					{
					} break;
				}
							
				if (event.joystick.button == 1)
				{
					shot = true;
				}
				if (event.joystick.button == 10)
				{
					keys.right = true;
					keys.left = false;
				}
				if (event.joystick.button == 11)
				{
					keys.left = true;
					keys.right = false;
				}
				if (event.joystick.button == 12)
				{
					keys.down = true;
					keys.up = false;
				}
				if (event.joystick.button == 13)
				{
					keys.up = true;
					keys.down = false;
				}
			}

			// Keyboard/Joystick events up
			if (event.type == ALLEGRO_EVENT_KEY_UP || event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)
			{
				switch (event.keyboard.keycode)
				{
					case ALLEGRO_KEY_A:
					case ALLEGRO_KEY_LEFT:
					{
						keys.left = false;
					} break;
					
					case ALLEGRO_KEY_D:
					case ALLEGRO_KEY_RIGHT:
					{
						keys.right = false;
					} break;

					case ALLEGRO_KEY_W:
					case ALLEGRO_KEY_UP:
					{
						keys.up = false;
					} break;

					case ALLEGRO_KEY_S:
					case ALLEGRO_KEY_DOWN:
					{
						keys.down = false;
					} break;

					case ALLEGRO_KEY_J:
					case ALLEGRO_KEY_Z:
					case ALLEGRO_KEY_SPACE:
					{
						keys.space = false;
					} break;

					default:
					{
					} break;
				}

				if (event.joystick.button == 10)
				{
					keys.right = false;
				}
				if (event.joystick.button == 11)
				{
					keys.left = false;
				}
				if (event.joystick.button == 12)
				{
					keys.down = false;
				}
				if (event.joystick.button == 13)
				{
					keys.up = false;
				}
			}

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				closed = true;
			}

			al_destroy_event_queue(queue);
			if (closed)
			{
				break;
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	// Check for memory leaks in Visual Studio
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Initialize Allegro and set up display
	
	srand(time(NULL));

	if (!al_init())
	{
		error("Failed to initialize");
	}
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_joystick();
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!closed)
	{
		restart = false;
		score = 0;
		if (display)
		{
			Spaceship spaceship = create_spaceship();
			if (!(&spaceship))
			{
				error("Could not create spaceship");
			}

			Blast *blast[TOTAL_POSSIBLE_BLASTS];
			for (int i = 0; i < TOTAL_POSSIBLE_BLASTS; i++)
			{
				blast[i] = NULL;
			}

			Asteroid *asteroid[TOTAL_POSSIBLE_ASTEROIDS];
			for (int i = 0; i < TOTAL_POSSIBLE_ASTEROIDS; i++)
			{
				asteroid[i] = NULL;
			}

			// Keyboard events
			void *result;
			ALLEGRO_THREAD *key_thread = al_create_thread(read_key_events, &result);
			al_start_thread(key_thread);

			ALLEGRO_FONT *font100 = al_load_font("FiraCode-Medium.ttf", 100, 0);
			ALLEGRO_FONT *font30 = al_load_font("FiraCode-Medium.ttf", 30, 0);
			ALLEGRO_FONT *font18 = al_load_font("FiraCode-Medium.ttf", 18, 0);
			char score_text[50];
			char lives_text[10];

			// Main game loop
			bool not_hit = true;
			int last_hit;
			while (!restart && !closed)
			{
				al_clear_to_color(BLACK);
				
				sprintf(score_text, "Score: %i", score);
				draw_text(font30, score_text, 20, 20, al_map_rgb(135, 206, 250), 0);
				sprintf(lives_text, "Lives: ");
				draw_text(font18, lives_text, 20, 56, al_map_rgb(135, 206, 250), 0);
				if (spaceship.lives > 0)
				{
					draw_lives(100, 64, spaceship.lives, 0.75);
					draw_ship(&spaceship);
					move_ship(&spaceship);
				}
				else
				{
					draw_text(font100, "GAME OVER", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 150, RED_TEXT, ALLEGRO_ALIGN_CENTER);
					sprintf(score_text, "Final score: %i", score);
					draw_text(font30, score_text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, RED_TEXT, ALLEGRO_ALIGN_CENTER);
					draw_text(font18, "Press 'R' to restart or 'ESC' to quit.", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100, RED_TEXT, ALLEGRO_ALIGN_CENTER);
				}

				for (int i = 0; i < TOTAL_POSSIBLE_ASTEROIDS; i++)
				{
					// Only fill out certain number of Asteroids in array to leave room for more after destroying
					bool empty = false;
					for (int j = STARTING_ASTEROID_NUM; j < TOTAL_POSSIBLE_ASTEROIDS; j++)
					{
						if (asteroid[j])
						{
							empty = false;
							break;
						}
						empty = true;
					}
					if (!asteroid[i] && i < STARTING_ASTEROID_NUM && empty)
					{
						asteroid[i] = malloc(sizeof(Asteroid));
						*asteroid[i] = create_asteroid(&spaceship);
						if (!(&asteroid[i]))
						{
							error("Could not create asteroid");
						}
					}
					else if (asteroid[i])
					{
						draw_asteroid(asteroid[i]);
						move_asteroid(asteroid[i]);

						// Check asteroid and spaceship collision
						if (not_hit)
						{
							if (collide_ship(asteroid[i], &spaceship))
							{
								spaceship.color = RED;
								last_hit = i;
								spaceship.lives--;
								not_hit = false;
							}
							else
							{
								spaceship.color = GREEN;
							}
						}
						else
						{
							if (!collide_ship(asteroid[last_hit], &spaceship))
							{
								spaceship.color = GREEN;
								not_hit = true;
							}
						}

						float adjustment_calc = (20 * (asteroid[i]->scale));
						if (asteroid[i]->sy > SCREEN_HEIGHT + adjustment_calc)
						{
							asteroid[i]->sy = 0 - adjustment_calc;
						}
						else if (asteroid[i]->sy < 0 - adjustment_calc)
						{
							asteroid[i]->sy = SCREEN_HEIGHT + adjustment_calc;
						}
						else if (asteroid[i]->sx > SCREEN_WIDTH + adjustment_calc)
						{
							asteroid[i]->sx = 0 - adjustment_calc;
						}
						else if (asteroid[i]->sx < 0 - adjustment_calc)
						{
							asteroid[i]->sx = SCREEN_WIDTH + adjustment_calc;
						}

						// Check blast collision
						for (int j = 0; j < TOTAL_POSSIBLE_BLASTS; j++)
						{
							if (blast[j] && asteroid[i])
							{
								if (collide_blast(asteroid[i], blast[j]))
								{
									asteroid[i]->health -= blast[j]->damage;
									if (asteroid[i]->health <= 0)
									{
										if (asteroid[i]->scale == 1)
										{
											score += 100;
										}
										else if (asteroid[i]->scale == 2)
										{
											score += 400;
										}
										else if (asteroid[i]->scale == 3)
										{
											score += 1000;
										}

										if (asteroid[i]->scale == 1)
										{
											free(asteroid[i]);
											asteroid[i] = NULL;
										}
										else
										{
											for (int k = STARTING_ASTEROID_NUM; k < TOTAL_POSSIBLE_ASTEROIDS; k++)
											{
												if (!asteroid[k])
												{
													asteroid[k] = malloc(sizeof(Asteroid));
													*asteroid[k] = split_asteroid(asteroid[i]);
													break;
												}
											}
										}
									}
									free(blast[j]);
									blast[j] = NULL;
								}
							}
						}
					}
				}

				if (shot && spaceship.lives > 0)
				{
					// Finds next available blast pointer to store new blast in
					for (int i = 0; i < 30; i++)
					{
						if (!blast[i])
						{
							blast[i] = malloc(sizeof(Blast));
							*blast[i] = create_blast();
							if (!(&blast[i]))
							{
								error("Could not create blast");
							}
							blast[i]->heading = spaceship.heading;
							blast[i]->sx = spaceship.sx;
							blast[i]->sy = spaceship.sy;
							shot = false;
							break;
						}
					}
				}
				else
				{
					shot = false;
				}

				for (int i = 0; i < 30; i++)
				{
					if (blast[i] != NULL)
					{
						draw_blast(blast[i]);
						move_blast(blast[i]);

						if (blast[i]->sx <= 0 || blast[i]->sx >= SCREEN_WIDTH || blast[i]->sy <= 0 || blast[i]->sy >= SCREEN_HEIGHT)
						{
							free(blast[i]);
							blast[i] = NULL;
						}
					}
				}

				// Test for spaceship reaching boundary
				if (spaceship.sy >= SCREEN_HEIGHT)
				{
					spaceship.sy = 0;
				}
				else if (spaceship.sy <= 0)
				{
					spaceship.sy = SCREEN_HEIGHT;
				}
				else if (spaceship.sx >= SCREEN_WIDTH)
				{
					spaceship.sx = 0;
				}
				else if (spaceship.sx <= 0)
				{
					spaceship.sx = SCREEN_WIDTH;
				}

				if (keys.left)
				{
					rotate_ship(&spaceship, LEFT);
				}
				if (keys.right)
				{
					rotate_ship(&spaceship, RIGHT);
				}
				if (keys.up)
				{
					accelerate_ship(&spaceship);
				}
				else if (!keys.up && !keys.down)
				{
					drifting_ship(&spaceship);
				}
				if (keys.down)
				{
					decelerate_ship(&spaceship);
				}

				al_flip_display();
				al_rest(0.0167); // Around 60fps
			}
			// Free memory
			for (int i = 0; i < TOTAL_POSSIBLE_ASTEROIDS; i++)
			{
				free(asteroid[i]);
				if (i < TOTAL_POSSIBLE_BLASTS)
				{
					free(blast[i]);
				}
			}
			al_destroy_font(font100);
			al_destroy_font(font30);
			al_destroy_font(font18);
			if (closed)
			{
				al_destroy_thread(key_thread);
				al_uninstall_keyboard();
				al_uninstall_joystick();
				al_destroy_display(display);
				al_shutdown_primitives_addon();
				al_shutdown_ttf_addon();
				al_shutdown_font_addon();
			}
		}
		else
		{
			error("Could not create display");
		}
	}
	
	return 0;
}