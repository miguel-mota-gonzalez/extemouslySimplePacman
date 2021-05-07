#ifndef COMMONCONST_H
#define COMMONCONST_H

const int BOARD_PIXELS_X = 18;
const int BOARD_PIXELS_Y = 18;

const int EMPTY               = 0;
const int WALL                = 1;
const int GOST                = 2;
const int WEAK_GOST           = 3;
const int PACMAN              = 4;
const int MINT                = 5;
const int PILL                = 6;
const int DSBLD               = 7; // Disabled
const int NO_DYNAMIC_CONTENT  = 8;

const int NO_SCORE            = -1;
const int MINT_SCORE          = 1;
const int PILL_SCORE          = 4;
const int GOST_SCORE          = 5;

const int NO_COMMAND_RECEIVED = -1;

// Messages to the board
const int NO_MESSAGE_FOR_THE_BOARD = 0;
const int CHANGE_CHASING_STATUS    = 1;
const int EXTEND_CHASING_TIME      = 2;
const int GAME_OVER                = 3;
const int NO_MESSAGE_FOR_GOST      = 4;

#endif // COMMONCONST_H
