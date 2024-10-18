/**
  @file attack.c
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Attack position selection for a Battleship game.
 */

#include "system.h"
#include "navswitch.h"
#include "tinygl.h"
#include "gamestate.h"
#include "pio.h"
#include "communication.h"

#define MAX_HITS 8 
#define FLASH_RATE 200

static tinygl_point_t hit_positions[10];
static uint8_t number_of_hits;
static uint8_t number_of_opponent_hits = 0;
static tinygl_point_t cursor_position;

void reset_hits(void) {
    cursor_position.x = 0;
    cursor_position.y = 0;
    number_of_opponent_hits = 0;
    number_of_hits = 0;
    for (uint8_t i = 0; i < MAX_HITS; i++) {
        hit_positions[i].x = 0;
        hit_positions[i].y = 0;
    }
}

void increment_oponent_hits(void) {
    number_of_opponent_hits++;
}
/** Update each pixel on the display that the player
    has already hit, from the hit_positions array
*/
static void update_hit_pixels(void)
{

    for (uint8_t i = 0; i < number_of_hits; i++) {
        if (!(cursor_position.x == hit_positions[i].x && cursor_position.y == hit_positions[i].y)) {
            tinygl_draw_point(hit_positions[i], 1);
        }
    }
}

/** Check if a current cursor position has already been hit.
    @returns Returns 1 if hit, 0 if not
*/
static bool position_already_hit(void)
{

    for (uint8_t i = 0; i < number_of_hits; i++) {
        if (cursor_position.x == hit_positions[i].x && cursor_position.y == hit_positions[i].y) {
            return 1;
        }
    }
    return 0;
}

/** Check the firing position is valid, send 
    a request to the second board, then return
    @return returns the next game state 
    (hit, miss or win)
 */
static GameState_t send_attack(void)
{

    if (!(position_already_hit())) {
        if (hit_request(&cursor_position)) { 
            hit_positions[number_of_hits] = cursor_position;
            number_of_hits += 1;
            if (number_of_hits == MAX_HITS) {
                return WIN; // We've hit all the ships. Change the game state.
            }
            return HIT;// Turn is over, and we hit. Change the game state
        }
        return MISS;// Turn is over, and we missed. Change the game state
    }

    return ATTACK; // Turn is not over, keep the existing state
}

/** Uses a division of the paced loop to flash the boat 
    @return on/off led flash
 */
static bool flash_cursor(void) 
{
    // Keep track of elapsed time in order to flash the cursor
    static uint16_t flash = 0;
    static bool state = 1;
    flash++;

    if (flash >= FLASH_RATE) {
        tinygl_clear();
        state = !state;
        flash = 0;
    }
    return state;
}

/** Update the attack position using the navswitch, 
    and send an attack if the middle button is pressed.
 */
static GameState_t select_attack_position(void)
{

    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if (cursor_position.y < 6) {
            tinygl_clear();
            cursor_position.y += 1; 
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_EAST)) {
        if (cursor_position.x < 4) {
            tinygl_clear();
            cursor_position.x += 1; 
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if (cursor_position.y > 0) {
            tinygl_clear();
            cursor_position.y -= 1;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        
        if (cursor_position.x > 0) {
            tinygl_clear();
            cursor_position.x -= 1; 
        }
    }

    tinygl_draw_point(cursor_position, flash_cursor());

    if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        return send_attack();
    }

    return ATTACK; // select_attack_position
} 

/** main function to run attack state */
GameState_t attack(void)
{
    GameState_t game_state = select_attack_position();
    if (number_of_opponent_hits >= MAX_HITS) {
        return LOSS;
    }
    update_hit_pixels();
    return game_state;
}