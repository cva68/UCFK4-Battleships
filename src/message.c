/**
  @file message.c
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Handles game states where a simple message needs to be displayed for a duration,
         or f
 */

#include "system.h"
#include "tinygl.h"
#include "gamestate.h"
#include "communication.h"

#define MESSAGE_DURATION 500

/** Display a character to the display from a void pointer
    @param character is a pointer to a char object */
static void display_character(void* character) {
    char buffer[2];
    buffer[0] = *(char*)character;
    buffer[1] = '\0';
    tinygl_text(buffer);
}

/** Display a point to the display from a void pointer
    @param point is a pointer to a tinygl_point_t object */
static void display_point(void* point) {
   tinygl_draw_point(*(tinygl_point_t*)point, 1);
}


/** Display something to the screen for a set duration, by calling a passed display_function
 *  and waiting until MESSAGE_DURATION has elapsed.
    @param item The item to be passed to a provided display_function 
    @param display_function A pointer to the function to be called for the defined duration
    @param reset A flag to reset the counter to zero, required when the function is interrupted 
    @return Whether or not the MESSAGE_DURATION has elapsed*/
static bool display_from_function(void* item, void (*display_function)(void*), bool reset)
{
    // Display a character for a predefined MESSAGE_DURATION
    // TO-DO: Move message-duration to an _init function
    
    static uint16_t display_time = 0;

    // Initialise the display if this is the first call
    if (display_time == 0) {
        tinygl_clear();
        display_function(item);
    }

    if (reset) {
        // The wait function sometimes doesn't finish a full display. This is used to reset the
        // counter to zero if we never hit MESSAGE_DURATION.
        display_time = 0; 
    } else if (display_time < MESSAGE_DURATION) {
        display_time++;
        return 0;
    }
    
    // Reset the timer and clear the display
    display_time = 0;
    tinygl_clear();
    return 1;
}


/** Display a 'M' to inform the user they missed.
    @return The next game state */
GameState_t miss(void) 
{
    // Display a miss message
    char c = 'M';
    if (display_from_function(&c, &display_character, false) == 0) {
        return MISS;
    }
    return WAIT;
}

/** Display a 'H' to inform the user they hit.
    @return The next game state */
GameState_t hit(void) 
{
    // Display a hit message
    char c = 'H';
    if (display_from_function(&c, &display_character, false) == 0) {
        return HIT;
    }
    return WAIT;
}

/** Display a 'W' to inform the user they've won the game.
    @return The next game state */
GameState_t win(void) 
{
    // Display a hit message
    char c = 'W';
    if (display_from_function(&c, &display_character, false) == 0) {
        return WIN;
    }
    return SETUP;
}

/** Display a 'L' to inform the user they've lost the game.
    @return The next game state */
GameState_t loss(void) 
{
    // Display a hit message
    char c = 'L';
    if (display_from_function(&c, &display_character, false) == 0) {
        return LOSS;
    }
    return SETUP;
}


/** Draw three moving dots (a loading symbol) on the display to inform the user
    they are waiting for the other player. The next game state is determined by
    whether a hit request has been received.
    @return The next game state */
GameState_t wait(void) 
{
   // Display three moving dots on the display
   static tinygl_point_t point = {2, 2};
   if (display_from_function(&point, &display_point, false)) {
        if (point.y == 4) {
            point.y = 2;
        } else {
            point.y++;
        }
   };

    // Use the communication module to check for an update, and determine the 
    // gamestate from this.
    if (check_for_request()) {
        // On the last display refresh, make sure the timer resets.
        display_from_function(&point, &display_point, true);
        return ATTACK;
    }
    return WAIT;
}
