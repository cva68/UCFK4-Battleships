# Multiplayer UC FunKit Battleships
A battleships game design for the UCFK4 - An embedded development board used at the University of Canterbury. Features serial communication over IR for multiplayer support.

## Requirements
- Two UCFK4 boards
- avr-gcc
- UCFK4 Driver, Util and Font module folders to be located in the parent directory of this file.

## Gameplay
### Setup Phase
On startup, the boards will enter the setup phase, where you are required to place your ships. There are three ships to be placed:
- 2 ships of length 3
- 1 ship of length 2
Use the navswitch to move these ships around the screen, and click to place your ship. Once all ships are placed on both boards, the game will begin.

### Attack Phase
One of the boards will now enter the attack phase. Move the cursor around with the navswitch to chose a location to fire. "H" will be displayed if you have hit an opponents ship, "M" will be displayed otherwise. On your next turn, ships you have already hit will be displayed as a solid LED.

### Win Phase
Once all ships have been sunk on either board, the boards will display a "W" to the winner, or an "L" to the loser. The next round will start automatically.



