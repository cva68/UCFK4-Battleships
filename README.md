# 2 Player UC FunKit Battleships

## Requirements
- Two UCFK boards
- avr-gcc
- UCFK Driver, Util and Font module folders, located at ../../

## Gameplay
### Setup Phase
On startup, the boards will enter the setup phase, where you are required to place your ships. There are three ships to be placed:
- 2 ships of length 3
- 1 ship of length 2
Use the navswitch to move these ships around the screen, and click to place your ship. Once all ships are placed on both boards, the game will begin.

### Attack Phase
One of the boards will now enter the attack phase. Move the cursor around with the navswitch to chose a location to fire. "H" will be displayed if you have hit an opponents ship, "M" will be displayed otherwise. On your next turn, ships you have already hit will be displayed as a solid LED.

### Win Phase
Once all ships have been sunk on either board, the boards will display a "W" to the winner, or an "L" to the loser. To play again, reset the board.



