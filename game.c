/**
  @file game.c
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Battleships game for the UCFK4. This file runs a paced 
         loop to update the display, navswitch and game state.
 */

#include "system.h"
#include "attack.h"
#include "setup.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "../fonts/font5x5_1_r.h"
#include "gamestate.h"
#include "message.h"
#include "communication.h"

#define PACER_RATE 500
#define TEXT_RATE 10

int main (void)
{ 
    // Initialise external modules
    system_init();
    tinygl_init(PACER_RATE);
    pacer_init(PACER_RATE);
    tinygl_font_set(&font5x5_1_r);
    ir_serial_init ();

    // Set the initial game state to the "SETUP" state
    GameState_t game_state = SETUP;

    // Paced loop
    while (1)
    {
        pacer_wait();

        // Switch to the correct game state based on the return of the current game state
        switch (game_state) {
            case SETUP:
                game_state = set();
                break;
            case ATTACK:
                game_state = attack();
                break;
            case WAIT:
                game_state = wait();
                break;
            case HIT:
                game_state = hit();
                break;
            case MISS:
                game_state = miss();
                break;
            case WIN:
                game_state = win();
                reset_boats();
                reset_hits();
                break;
            case LOSS:
                game_state = loss();
                reset_boats();
                reset_hits();
                break;
        }

        tinygl_update();
        navswitch_update();
        
    }   
}
