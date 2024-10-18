/**
  @file gamestate.h
  @author C. Varney, C. Horne
  @date 13/10/2024
  @brief Defines all states of game in enum.
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum {
    SETUP = 0,
    ATTACK,
    WAIT,
    HIT,
    MISS,
    WIN,
    LOSS
} GameState_t;

#endif // GAMESTATE_H