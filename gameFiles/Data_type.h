#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define player_num 2 // can be modified later
#define card_num 13
#define max_round 10
#define card_deck_num 3
#define round_buff_num 4

#define TIME_UP 20000
#define ult_num 5 // need to be less than

#ifndef _DATAH_
#define _DATAH_

// data types
struct card{
    int card_ID;    // 1 - 13 index
    int player_ID; // to be the same for 13 cards

    // available or not?
    bool valid;
};

struct player{
    // basic information
    struct card* cards; // the card player holds *need to be removed after being used*
    int player_ID;
    int health; // initially 20
    
    int shield[3];
    int indexAdd;
    int indexRemove;

    // alive or not
    bool alive;

    // Buff relevant
    int ultimate;
    int ultUseThisRound;
};

#endif