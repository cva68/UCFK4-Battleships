/**
  @file communication.h
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Handle exchanging game state information between boards
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "system.h"
#include "ir.h"
#include "ir_serial.h"
#include "tinygl.h"
#include "gamestate.h"

/* We'll define a packet format. 

|  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
|    Type   |        X        |        Y        | 

Type: 0x00 for hit request, 0x11 for hit response, 0x10 for game initialiser, 0x01 for game initialiser ack
X and Y: data
    - x and y position for a hit request,
    - both 0x111 for a sucessful hit response
    - both 0x000 for a unsuccessful hit response
    - unused for a game initialiser
*/

#define X_BITMASK 0x38
#define Y_BITMASK 0x07
#define X_SHIFT 0x03
#define Y_SHIFT 0x03
#define HEADER_SHIFT 8
#define NUM_OF_ROWS 7

typedef enum {
    REQUEST_HEADER = 0x00,
    RESPONSE_HEADER = 0x01,
} PacketType_t;

// These follow the packet structure, but the data is fixed. Rather than forming
// a packet with bit operations, it's easier to store them whole.
typedef enum {
    RESPONSE_HIT = 0xFF,
    RESPONSE_MISS = 0x80,
    INIT_READY = 0x7F,
    INIT_READY_ACK = 0xBF
} PredefinedMessages_t;


/** Returns whether a position is a hit against this players boat
 *  @param position The position of the boat
 *  @return Whether that position contains a boat */
bool remote_is_hit(tinygl_point_t position);


/** Stores a list of boat positions locally, for transferring from setup.c 
*  to communication.c
 *  @param positions Array of position vectors */
void store_own_boats(tinygl_point_t* positions);

/** Sends a hit request packet over IR to the other board.
*  @param position Position to probe
*  @return Whether the response was a hit */
bool hit_request(tinygl_point_t* cursor_position);

/** Sends a packet and wait for a response.
*  @param packet The data packet to send
*  @param data A pointer to a uint8_t to store the response
*  @note This is a blocking function, the display will freeze until we receive a response */
void send_and_wait(uint8_t packet, uint8_t* data);

/** Sends an initialiser packet to the other board to state we are ready to begin the game, wait
 *  for an acknowledgement.
 *  @return The state of the game to enter when the second player is ready */
GameState_t send_init(void);

/** Send a response to a hit_request packet
*  @param cursor_position The cursor position received in the hit_request packet */
void hit_response(tinygl_point_t* cursor_position);

/** Periodically check for incoming IR packets in the paced loop, and react accordingly.
*  @return Whether a packet was received*/
bool check_for_request(void);

void reset_hits(void);
#endif