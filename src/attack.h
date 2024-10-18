/**
  @file attack.c
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Attack position selection for a Battleship game.
 */

#ifndef ATTACK_H
#define ATTACK_H

#include "system.h"
#include "navswitch.h"
#include "tinygl.h"
#include "gamestate.h"

// Public
GameState_t attack(void);


// Private

tinygl_point_t hit_positions[10];
uint8_t number_of_hits;
tinygl_point_t cursor_position;

GameState_t select_attack_position(void);
bool flash_cursor(void);
GameState_t send_attack(void);
bool position_already_hit(void);
void update_hit_pixels(void);
void increment_oponent_hits(void);
void boat_length(void);


#endif // ATTACK_H