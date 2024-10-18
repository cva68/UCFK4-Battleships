/**
  @file setup.h
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Setup state for a battleships game. Allows the user to place three boats, then informs 
          the communication module about these positions.
 */

#include "system.h"
#include "setup.h"
#include "navswitch.h"
#include "tinygl.h"
#include "gamestate.h"
#include "communication.h"

#define NUM_BOATS 8
#define FLASH_RATE 200 
#define NORTH_BARRIER 0
#define WEST_BARRIER 0
#define EAST_BARRIER 4

// standard variables for storing previous and current boats
static tinygl_point_t boatStart[3];
static tinygl_point_t boatEnd[3];
static tinygl_point_t pos1 = {0,0};
static tinygl_point_t pos2 = {0,2};
static tinygl_point_t boat_locations[NUM_BOATS] = {{0,0}};
static uint8_t number_of_boats = 0;
static uint8_t boat_length = 3;
static bool length_changed = false;

void reset_boats(void) 
{
    pos1.x = 0;
    pos1.y = 0;
    pos2.x = 0;
    pos2.y = 2;
    length_changed = false;
    number_of_boats = 0;
    boat_length = 3;
}

/* For each boat already placed, display it on the screen every cycle
   of the paced loop.
 */
void boat_update(void) 
{
    // Display all stored placed boats.
    for (uint8_t i = 0; i < number_of_boats; i++) {
        tinygl_draw_line(boatStart[i], boatEnd[i], 1);
    }
}


/** Uses a division of the paced loop to flash the boat 
    @return on/off led flash
 */
bool flash_boat(void) 
{
    // Keep track of elapsed time in order to flash the cursor
    static uint16_t flash = 0;
    static bool state = 1;
    flash++;

    if (flash >= FLASH_RATE) { // Need to remove magic numbers
        state = !state;
        flash = 0;
    }
    return state;
}


/** Checks wether the current location of the 
    boat being placed overlaps with a previously 
    placed boat. 
    @returns returns 0 if overlap, 1 if not
 */
bool boat_already_placed(void)
{
    // iterates over every placed boat
    for (uint8_t i = 0; i < number_of_boats; i++) {
        
        //changes type tinygl to algebra for comparrisons 
        uint8_t check_x = pos1.x ;
        uint8_t compare_x = boatStart[i].x;
        uint8_t check_y = pos1.y;
        uint8_t compare_y = boatStart[i].y;

        
        // uses if statement comparisson to compare every bit
        if (boat_length == 3) {
            if (((check_y == compare_y) || 
                (check_y == (compare_y+1)) || 
                (check_y == (compare_y+2)) || 
                ((check_y + 1) == compare_y) || 
                ((check_y + 1) == (compare_y+1)) || 
                ((check_y + 1) == (compare_y+2)) ||
                ((check_y + 2) == compare_y) || 
                ((check_y + 2) == (compare_y+1)) || 
                ((check_y + 1) == (compare_y+2))) &&
                (check_x == compare_x)) {
                    return false; // returns false if there is overlap
                }    
        } else if (boat_length == 2) {
            if (((check_y == compare_y) || 
                (check_y == (compare_y+1)) || 
                (check_y == (compare_y+2)) || 
                ((check_y + 1) == compare_y) || 
                ((check_y + 1) == (compare_y+1)) || 
                ((check_y + 1) == (compare_y+2))) &&
                (check_x == compare_x)) {
                    return false; // returns false if there is overlap
                }
        }
    }
    // returns true if no overlap found
    return true;
}


/** Sets current location to placed boat */
void boat_place(void)
{
    if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        if(boat_already_placed()) {
            boatStart[number_of_boats] = pos1;
            boatEnd[number_of_boats] = pos2;
            number_of_boats += 1;
        }
    }
}


/**changes the length of the boat after two boats are placed */
void get_boat_length(void)
{
    if ((number_of_boats == 2) && (length_changed == false)) {
        boat_length = 2;
    }
    if ((boat_length == 2) && (length_changed == false)) {
        tinygl_clear();
        pos2.y -= 1;
        length_changed = true;
    }
}


/** Reads navigation input and updates location
    of current boat
    @return The next state of the game
 */
GameState_t select_boat_position(void)
{
    uint8_t south_barrier = 6-boat_length+1;
    get_boat_length();
    
    //moves both ends of current boat using nav switch
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if (pos1.y < (south_barrier)) {
            tinygl_clear();
            pos1.y += 1; 
            pos2.y += 1; 
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_EAST)) {
        if (pos1.x < EAST_BARRIER) {
            tinygl_clear();
            pos1.x += 1; 
            pos2.x += 1;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if (pos1.y > NORTH_BARRIER) {
            tinygl_clear();
            pos1.y -= 1;
            pos2.y -= 1;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        
        if (pos1.x > WEST_BARRIER) {
            tinygl_clear();
            pos1.x -= 1; 
            pos2.x -= 1; 
        }
    }
    tinygl_draw_line(pos1, pos2, flash_boat());

    boat_place();
   return SETUP;
}


/** stores the position of each LED of boat as its own tingl_point
    treated as individual boats
 */
void boat_save(void)
{
    //stores boats as array
    uint8_t write_index = 0;
    for (uint8_t i = 0; i < number_of_boats; i++) {
        for (uint8_t pos = boatStart[i].y; pos <= boatEnd[i].y; pos++) {
            boat_locations[write_index].y = pos;
            boat_locations[write_index].x = boatStart[i].x;
            write_index++;
        }
    }
    store_own_boats(boat_locations);
}


/** Main function to run setup state
 *  @return The next state of the game
*/
GameState_t set(void)
{
    select_boat_position();
    boat_update();
    if (number_of_boats == 3) {
        boat_save();
        return send_init();
    }
    check_for_request();
    return SETUP;
}