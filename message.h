/**
  @file message.h
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Handles simple game states where a message needs to be displayed for a duration,
         and the next game state needs to be returned.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include "system.h"
#include "tinygl.h"
#include "gamestate.h"
#include "communication.h"

/** Display a 'M' to inform the user they missed.
    @return The next game state */
GameState_t miss(void);

/** Display a 'H' to inform the user they hit.
    @return The next game state */
GameState_t hit(void);

/** Display a 'W' to inform the user they've won the game. */
GameState_t win(void);

/** Display a 'L' to inform the user they've lost the game. */
GameState_t loss(void);

/** Draw three moving dots (a loading symbol) on the display to inform the user
    they are waiting for the other player. The next game state is determined by
    whether a hit request has been received.
    @return The next game state */
GameState_t wait(void);


// Private functions

/** Display a character to the display from a void pointer
    @param character is a pointer to a char object */
void display_character(void* character);

/** Display a point to the display from a void pointer
    @param point is a pointer to a tinygl_point_t object */
void display_point(void* point);

/** Display something to the screen for a set duration, by calling a passed display_function
 *  and waiting until MESSAGE_DURATION has elapsed.
    @param item The item to be passed to a provided display_function 
    @param display_function A pointer to the function to be called for the defined duration
    @param reset A flag to reset the counter to zero, required when the function is interrupted 
    @return Whether or not the MESSAGE_DURATION has elapsed*/
bool display_from_function(void* item, void (*display_function)(void*), bool reset);

#endif // MESSAGE_H