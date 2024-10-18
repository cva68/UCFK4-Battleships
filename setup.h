/**
  @file setup.h
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Setup ships
 */

#ifndef SETUP_H
#define SETUP_H

#include "system.h"
#include "setup.h"
#include "navswitch.h"
#include "tinygl.h"
#include "gamestate.h"
#include "communication.h"

void reset_boats(void);

/* For each boat already placed, display it on the screen every cycle
   of the paced loop.
 */
void boat_update(void);


/** Uses a division of the paced loop to flash the boat 
    @return on/off led flash
 */
bool flash_boat(void);


/** Checks wether the current location of the 
    boat being placed overlaps with a previously 
    placed boat. 
    @returns returns 0 if overlap, 1 if not
 */
bool boat_already_placed(void);

/** Sets current location to placed boat */
void boat_place(void);

/**Changes the length of the boat after two boats are placed */
void get_boat_length(void);


/** Reads navigation input and updates location
    of current boat
    @return The next state of the game
 */
GameState_t select_boat_position(void);

/** stores the position of each LED of boat as its own tingl_point
    treated as individual boats
 */
void boat_save(void);


/** Main function to run setup state
 *  @return The next state of the game
*/
GameState_t set(void);

#endif