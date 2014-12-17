#ifndef __SEQUENCES_H__
#define __SEQUENCES_H__

#include "application.h"

/* SEQUENCES */
/*
cylon:
    Accepts length 1-3
    sends a blip up and back through the relays in order
*/
int cylon1_len = 6;
byte cylon1[] = {
    0b1000,
    0b0100,
    0b0010,
    0b0001,
    0b0010,
    0b0100
};

int cylon2_len = 4;
byte cylon2[] = {
    0b1100,
    0b0110,
    0b0011,
    0b0110
};

int cylon3_len = 2;
byte cylon3[] = {
    0b1110,
    0b0111
};

/*
chase:
    accepts length 1-3
    sends a blip from left to right through the relays in order
*/
int chase1_len = 5;
byte chase1[] = {
    0b0000,
    0b1000,
    0b0100,
    0b0010,
    0b0001
};

int chase2_len = 6;
byte chase2[] = {
    0b0000,
    0b1000,
    0b1100,
    0b0110,
    0b0011,
    0b0001
};

int chase3_len = 7;
byte chase3[] = {
    0b0000,
    0b1000,
    0b1100,
    0b1110,
    0b0111,
    0b0011,
    0b0001
};

// flap: flips between relays 1,4 and 2,3
int flap_len = 2;
byte flap[] = {
    0b1001,
    0b0110
};

// walk: flips between relays 1,3 and 2,4
int walk_len = 2;
byte walk[] = {
    0b1010,
    0b0101
};

#endif

