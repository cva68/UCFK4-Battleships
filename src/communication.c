/**
  @file communication.c
  @author C. Varney, C. Horne
  @date 11/10/2024
  @brief Handle exchanging game state information between boards
 */

#include "system.h"
#include "ir_serial.h"
#include "tinygl.h"
#include "communication.h"
#include "gamestate.h"
#include "attack.h"
#include "pacer.h"

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

// Store a local copy of this players boats, to allow checking for hits later
tinygl_point_t own_boats[8] = {{0,0}};


/** Returns whether a position is a hit against this players boat
 *  @param position The position of the boat
 *  @return Whether that position contains a boat */
bool remote_is_hit(tinygl_point_t position) {
    for (uint8_t i = 0; i < 8; i++) {
        if (position.x == own_boats[i].y && position.y == own_boats[i].x) {
            return true;
        }
    }
    return false;
}


/** Stores a list of boat positions locally, for transferring from setup.c 
*  to communication.c
 *  @param positions Array of position vectors */
void store_own_boats(tinygl_point_t* positions) {
    for (uint8_t i = 0; i < 8; i++) {  
        own_boats[i].x = positions[i].x;
        own_boats[i].y = positions[i].y;
        tinygl_draw_point(positions[i], 1);
    }
}


/** Sends a hit request packet over IR to the other board.
*  @param position Position to probe
*  @return Whether the response was a hit */
bool hit_request(tinygl_point_t* cursor_position)
{
    // Form the packet
    uint8_t packet = REQUEST_HEADER; // zero for request

    // Encode the x and y positions into the packet
    packet += (cursor_position->y << Y_SHIFT);
    packet += (cursor_position->x);
  
    // Send the request
    uint8_t response;
    send_and_wait(packet, &response);

    // Process the response
    return (response == RESPONSE_HIT); // Evaluates true for hit, false for miss.
}


/** Sends a packet and wait for a response.
*  @param packet The data packet to send
*  @param data A pointer to a uint8_t to store the response
*  @note This is a blocking function, the display will freeze until we receive a response */
void send_and_wait(uint8_t packet, uint8_t* data)
{
    // Blocking function. Send a packet and wait for a response.
    ir_serial_ret_t ret = IR_SERIAL_NONE;
    bool packet_valid = false;

    while (!packet_valid) {//|| !incoming_packet) {
        ret = IR_SERIAL_NONE;
        // Send data
        ir_serial_transmit(packet);

        // Block until response received
        while (ret != IR_SERIAL_OK) {
            pacer_wait();
            ret = ir_serial_receive(data);
        }
    
        // "Parity check", i.e. making sure the response is one of three valid responses
        packet_valid = (*data == RESPONSE_HIT || *data == RESPONSE_MISS || *data == INIT_READY_ACK);
    }
}


/** Sends an initialiser packet to the other board to state we are ready to begin the game, wait
 *  for an acknowledgement.
 *  @return The state of the game to enter when the second player is ready */
GameState_t send_init(void)
{
    send_and_wait(INIT_READY, NULL);
    return WAIT;
}


/** Periodically check for incoming IR packets in the paced loop, and react accordingly.
*  @return Whether a packet was received*/
bool check_for_request(void) {
    // Check whether a response needs to be sent while in wait phase
    // Returns true if a request was responded to
    static ir_serial_ret_t ret = IR_SERIAL_NONE;
    uint8_t data = 0;

    ret = ir_serial_receive(&data);
    if (ret == IR_SERIAL_OK){
        ret = IR_SERIAL_NONE;
        //ir_serial_transmit(RESPONSE_HIT);
        // Check this is an incoming packet before processing further, in an attempt to avoid crosstalk
        if ((data >> HEADER_SHIFT) == REQUEST_HEADER) {
            tinygl_point_t cursor_position = {(data & X_BITMASK) >> X_SHIFT, data & Y_BITMASK};
            hit_response(&cursor_position);
            return true;
        } else if (data == INIT_READY) {
            ir_serial_transmit(INIT_READY_ACK);
        }
    }
    return false;
}


/** Send a response to a hit_request packet
*  @param cursor_position The cursor position received in the hit_request packet */
void hit_response(tinygl_point_t* cursor_position)
{
    // Check if hit or miss
    if (remote_is_hit(*cursor_position)) {
    //if (cursor_position->x == 6 && cursor_position->y == 0) {
        ir_serial_transmit(RESPONSE_HIT);
        increment_oponent_hits();
    } else {
        ir_serial_transmit(RESPONSE_MISS);
    }
}