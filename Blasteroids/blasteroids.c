//#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <stdlib.h>
//#include <crtdbg.h>  // Required under <stdlib.h> based on MS documentation
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>

// Local includes
#include "asteroid.h"
#include "spaceship.h"
#include "powerup.h"
#include "blasteroids.h"
#include "blast.h"
#include "gui.h"

// Allegro includes
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define TOTAL_POSSIBLE_BLASTS 30
#define TOTAL_POSSIBLE_ASTEROIDS 40
#define STARTING_ASTEROID_NUM 10
#define BLACK al_map_rgb(0, 0, 0)
#define RED_TEXT al_map_rgb(255, 17, 30)
#define PURPLE al_map_rgb(255, 0, 255)

// Error function used to catch various errors that might occur.
void error(char *msg)
{
	printf("%s: %s\n", msg, strerror(errno));
	exit(1);
}

// Key struct
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

// Thread function that is used to catch inputs from the player. Because it uses an Allegro thread, it must
// always return a void pointer and accept an ALLEGRO_THREAD and void pointer as parameters.
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

			// Keyboard/Joystick events 'pressed' down
			if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
			{
				switch (event.keyboard.keycode)
				{
					// Rotate left
					case ALLEGRO_KEY_A:
					case ALLEGRO_KEY_LEFT:
					{
						keys.right = false;
						keys.left = true;
					} break;

					// Rotate right
					case ALLEGRO_KEY_D:
					case ALLEGRO_KEY_RIGHT:
					{
						keys.left = false;
						keys.right = true;
					} break;

					// Accelerate
					case ALLEGRO_KEY_W:
					case ALLEGRO_KEY_UP:
					{
						keys.up = true;
						keys.down = false;
					} break;

					// Decelerate
					case ALLEGRO_KEY_S:
					case ALLEGRO_KEY_DOWN:
					{
						keys.down = true;
						keys.up = false;
					} break;

					// Shoot blast
					case ALLEGRO_KEY_J:
					case ALLEGRO_KEY_SPACE:
					case ALLEGRO_KEY_Z:
					{
						shot = true;
					} break;

					// Closes the game
					case ALLEGRO_KEY_ESCAPE:
					{
						closed = true;
					} break;

					// Restarts the game
					case ALLEGRO_KEY_R:
					{
						restart = true;
					}

					default:
					{
					} break;
				}
				
				// Shoot blast
				if (event.joystick.button == 1)
				{
					shot = true;
				}
				// Rotate right
				if (event.joystick.button == 10)
				{
					keys.right = true;
					keys.left = false;
				}
				// Rotate left
				if (event.joystick.button == 11)
				{
					keys.left = true;
					keys.right = false;
				}
				// Decelerate
				if (event.joystick.button == 12)
				{
					keys.down = true;
					keys.up = false;
				}
				// Accelerate
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

			// Closes game if "X" button on window clicked
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
	
	// Random number seed setup
	srand(time(NULL));

	// Initialize Allegro and set up display, addons, and install various tools
	if (!al_init())
	{
		error("Failed to initialize");
	}
	al_install_audio();
	al_init_acodec_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_joystick();
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	al_reserve_samples(100);

	// Sound files used in Blasteroids
	ALLEGRO_SAMPLE *sample_shoot = al_load_sample("Shoot.ogg");
	ALLEGRO_SAMPLE *sample_ship_hit = al_load_sample("Spaceship Hit.ogg");
	ALLEGRO_SAMPLE *sample_powerup = al_load_sample("Powerup.ogg");
	ALLEGRO_SAMPLE *sample_powerup_shoot = al_load_sample("Powerup Shoot.ogg");
	ALLEGRO_SAMPLE *sample_asteroid_hit = al_load_sample("Hit Asteroid.ogg");
	ALLEGRO_SAMPLE *sample_asteroid_destroy_1 = al_load_sample("Asteroid Destroy 1.ogg");
	ALLEGRO_SAMPLE *sample_asteroid_destroy_2 = al_load_sample("Asteroid Destroy 2.ogg");
	ALLEGRO_SAMPLE *sample_asteroid_destroy_3 = al_load_sample("Asteroid Destroy 3.ogg");

	// Keyboard events thread setup and start
	void *result;
	ALLEGRO_THREAD *key_thread = al_create_thread(read_key_events, &result);
	al_start_thread(key_thread);

	// Font setup with various settings
	ALLEGRO_FONT *font100 = al_load_font("font.ttf", 100, 0);
	ALLEGRO_FONT *font30 = al_load_font("font.ttf", 30, 0);
	ALLEGRO_FONT *font18 = al_load_font("font.ttf", 18, 0);

	// Main game loop start. Outer loop that determines whether game exits completely or not.
	while (!closed)
	{
		restart = false;
		score = 0;
		if (display)
		{
			// Create spaceship, blast, asteroid, and powerup structs/pointer arrays
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

			Powerup *powerup = NULL;

			char score_text[50];
			char lives_text[10];
			
			bool not_hit = true;
			int last_hit;
			int ptimer = 0;
			bool pspawned = false;
			bool powerup_enabled = false;
			int petimer = 0;
			// Main game loop determines whether the user has restarted the game or not.
			while (!restart && !closed)
			{
				al_clear_to_color(BLACK);
				
				// Draw score and lives
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
				// Draw game over screen
				else
				{
					draw_text(font100, "GAME OVER", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 150, RED_TEXT, ALLEGRO_ALIGN_CENTER);
					sprintf(score_text, "Final score: %i", score);
					draw_text(font30, score_text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, RED_TEXT, ALLEGRO_ALIGN_CENTER);
					draw_text(font18, "Press 'R' to restart or 'ESC' to quit.", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100, RED_TEXT, ALLEGRO_ALIGN_CENTER);
				}

				// Spawn powerup after 30 seconds
				int spawn_time = 5;
				int spawn_delay = 7;
				int powerup_buff_time = 10;
				if (ptimer / 60 >= spawn_time && !pspawned)
				{
					ptimer = 0;
					if (powerup)
					{
						free(powerup);
						powerup = NULL;
					}
					powerup = malloc(sizeof(Powerup));
					*powerup = create_powerup();
					pspawned = true;
				}
				// Allows user 7 seconds to grab the powerup before it disappears
				else if (ptimer / 60 >= spawn_delay && pspawned)
				{
					ptimer = 0;
					if (powerup)
					{
						free(powerup);
						powerup = NULL;
					}
					pspawned = false;
				}
				
				// Change ship bullets to powerup version for 10 seconds
				if (petimer / 60 < powerup_buff_time && powerup_enabled)
				{
					petimer++;
				}
				else if (petimer / 60 >= powerup_buff_time && powerup_enabled)
				{
					petimer = 0;
					powerup_enabled = false;
				}

				// If it exists, draw powerup and check for powerup and ship collision.
				if (powerup)
				{
					draw_powerup(powerup);

					if (collide_powerup(powerup, &spaceship))
					{
						al_play_sample(sample_powerup, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
						powerup_enabled = true;
						ptimer = 0;
						free(powerup);
						powerup = NULL;
					}
				}

				// Fill in asteroid pointer array
				for (int i = 0; i < TOTAL_POSSIBLE_ASTEROIDS; i++)
				{
					// Checks if array indexes past the STARTING_ASTEROID_NUM are used to avoid overflow for asteroid splitting.
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
					// Only fill out certain number of Asteroids in array to leave room for more after destroying
					if (!asteroid[i] && i < STARTING_ASTEROID_NUM && empty)
					{
						asteroid[i] = malloc(sizeof(Asteroid));
						*asteroid[i] = create_asteroid(&spaceship);
						if (!(&asteroid[i]))
						{
							error("Could not create asteroid");
						}
					}
					// If an asteroid exists, draw and move it.
					else if (asteroid[i])
					{
						draw_asteroid(asteroid[i]);
						move_asteroid(asteroid[i]);

						// Check asteroid and spaceship collision. If hit, change spaceship color to red, 
						// if not hit change it to green/purple depending on powerup_enabled state.
						if (not_hit)
						{
							if (collide_ship(asteroid[i], &spaceship))
							{
								al_play_sample(sample_ship_hit, 0.1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
								spaceship.color = RED;
								last_hit = i;
								spaceship.lives--;
								// "not_hit" allows user to escape the asteroid without being hurt again until they leave the asteroid
								not_hit = false; 
							}
							else
							{
								if (powerup_enabled)
								{
									spaceship.color = PURPLE;
								}
								else
								{
									spaceship.color = GREEN;
								}
							}
						}
						else
						{
							// If spaceship has left the asteroid, reset the not_hit value and change spaceship color appropriately
							if (!collide_ship(asteroid[last_hit], &spaceship))
							{
								if (powerup_enabled)
								{
									spaceship.color = PURPLE;
								}
								else
								{
									spaceship.color = GREEN;
								}
								not_hit = true;
							}
						}

						// Checks asteroid positions and determines whether it should wrap around the board of the window or not.
						// adjustment_calc allows the asteroid to completely disappear from the window before wrapping.
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

						// Check blast and asteroid collision
						for (int j = 0; j < TOTAL_POSSIBLE_BLASTS; j++)
						{
							if (blast[j] && asteroid[i])
							{
								if (collide_blast(asteroid[i], blast[j]))
								{
									// Damage the asteroid and check if it's destroyed
									asteroid[i]->health -= blast[j]->damage;
									if (asteroid[i]->health <= 0)
									{
										// Based on the asteroid's scale, add score and play an asteroid destroy sound
										if (asteroid[i]->scale == 1)
										{
											al_play_sample(sample_asteroid_destroy_1, 0.075, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
											score += 100;
										}
										else if (asteroid[i]->scale == 2)
										{
											al_play_sample(sample_asteroid_destroy_2, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
											score += 400;
										}
										else if (asteroid[i]->scale == 3)
										{
											al_play_sample(sample_asteroid_destroy_3, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
											score += 1000;
										}

										// If the asteroid's scale is 1 (smallest), then destroy it.
										if (asteroid[i]->scale == 1)
										{
											al_play_sample(sample_asteroid_destroy_1, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
											free(asteroid[i]);
											asteroid[i] = NULL;
										}
										// Splits the asteroid if it doesn't have a scale of 1.
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
									else
									{
										// Asteroid hit sound
										al_play_sample(sample_asteroid_hit, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
									}
									free(blast[j]);
									blast[j] = NULL;
								}
							}
						}
					}
				}

				// If ship is alive and player pressed 'shoot' button, create a blast.
				if (shot && spaceship.lives > 0)
				{
					// Finds next available blast pointer to store new blast in
					for (int i = 0; i < 30; i++)
					{
						if (!blast[i])
						{
							blast[i] = malloc(sizeof(Blast));
							if (powerup_enabled)
							{
								*blast[i] = create_blast(7);
							}
							else
							{
								*blast[i] = create_blast(1);
							}
							if (!(&blast[i]))
							{
								error("Could not create blast");
							}
							blast[i]->heading = spaceship.heading;
							blast[i]->sx = spaceship.sx + sin(blast[i]->heading) * (blast[i]->speed * 1.5);
							blast[i]->sy = spaceship.sy - cos(blast[i]->heading) * (blast[i]->speed * 1.5);
							if (powerup_enabled)
							{
								al_play_sample(sample_powerup_shoot, 0.2, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
							}
							else
							{
								al_play_sample(sample_shoot, 0.1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
							}
							// Set shot to false so player must repeatedly hit shoot button
							shot = false;
							break;
						}
					}
				}
				else
				{
					shot = false;
				}

				// If a blast exists in the pointer array, draw and move it.
				for (int i = 0; i < 30; i++)
				{
					if (blast[i] != NULL)
					{
						draw_blast(blast[i]);
						move_blast(blast[i]);

						// If a blast exits the window border, destroy it.
						if (blast[i]->sx <= 0 || blast[i]->sx >= SCREEN_WIDTH || blast[i]->sy <= 0 || blast[i]->sy >= SCREEN_HEIGHT)
						{
							free(blast[i]);
							blast[i] = NULL;
						}
					}
				}

				// Checks if the spaceship reaches a window border and warps it
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

				// Calls functions for button presses activated in input thread function
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
				al_rest(0.01666666667); // Around 60fps
				ptimer++;
			}
			// Free memory for powerup, asteroids, and blasts after restarting.
			if (powerup)
			{
				free(powerup);
				powerup = NULL;
			}
			for (int i = 0; i < TOTAL_POSSIBLE_ASTEROIDS; i++)
			{
				free(asteroid[i]);
				asteroid[i] = NULL;
				if (i < TOTAL_POSSIBLE_BLASTS)
				{
					free(blast[i]);
					blast[i] = NULL;
				}
			}
			
			// Uninstall, destroy, and shutdown various allegro addons and tools if player closes game.
			if (closed)
			{
				al_destroy_font(font100);
				al_destroy_font(font30);
				al_destroy_font(font18);
				al_destroy_thread(key_thread);
				al_uninstall_keyboard();
				al_uninstall_joystick();
				al_destroy_display(display);
				al_destroy_sample(sample_shoot);
				al_destroy_sample(sample_ship_hit);
				al_destroy_sample(sample_powerup);
				al_destroy_sample(sample_asteroid_hit);
				al_destroy_sample(sample_asteroid_destroy_1);
				al_destroy_sample(sample_asteroid_destroy_2);
				al_destroy_sample(sample_asteroid_destroy_3);
				al_uninstall_audio();
				al_shutdown_primitives_addon();
				al_shutdown_ttf_addon();
				al_shutdown_font_addon();
				al_uninstall_system();
			}
		}
		else
		{
			error("Could not create display");
		}
	}
	
	return 0;
}