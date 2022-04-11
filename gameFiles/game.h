// personal header files
#include "Data_type.h"
#include "Card_Function.h"
#include <time.h>
#include "vga.h"
#include "Keyboard.h"

/* PARAMETERS */
// keycodes used for game
#define KEY_SPACE 0x29
#define KEY_ENTER1 0x5A
#define KEY_ENTER2 0xE05A
#define KEY_UP 0xE075
#define KEY_DOWN 0xE072
#define KEY_LEFT 0xE06B
#define KEY_RIGHT 0xE074
#define KEY_U 0x3C
#define KEY_ESC 0x76

// time limit
#define TIME_LIMIT_CHOOSE_CARD 30

#ifndef _GAMEH_
#define _GAMEH_

/* FUNCTION DEFINITION */
struct player* playerInit(int playerID);
struct card* cardInit(int playerID);
void cardFunction(struct player* player, int card_ID, int buff_ID, volatile int* ULT_INFO_ptr);
void removeOldShield(struct player* me, struct player* oppo);
int chooseCard(struct player* currPlayer, int round_buff, volatile int* MY_CARD_1_ptr, volatile int* MY_CARD_2_ptr,
                 volatile int* MY_CARD_3_ptr, volatile int* MY_CARD_USED_ptr, volatile int* TIME_ptr,
                 volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* CARD_SELECT_ptr,
                 volatile int* ULT_INFO_ptr);
char getPlayerMode(volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* INITIAL_SCREEN_ptr);
int getCardFromKbd(struct player* player, int round_buff, volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr,
                     volatile int* CARD_SELECT_ptr, volatile int* ULT_INFO_ptr);
void getEscape(volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* INITIAL_SCREEN_ptr);

/* FUNCTION IMPLEMENTATION */
/* Game initialization functions */

struct player* playerInit(int playerID) {
    struct player* player = malloc(sizeof(struct player));

    player->cards = cardInit(playerID);
    player->player_ID = playerID;
    player->health = 20;
    player->shield[0] = 0;
    player->shield[1] = 0;
    player->shield[2] = 0;
    player->indexAdd = 0;
    player->indexRemove = 1;
    player->alive = true;

    return player;
}

struct card* cardInit(int playerID){
    // array of cards
    struct card* card_array = malloc((card_num + 1) * sizeof(struct card));

    for (int i = 0; i < card_num; i++){
        card_array[i].card_ID = i; // index
        card_array[i].player_ID = playerID;
        card_array[i].valid = true; // set card to valid
    }

    return card_array;
}

void removeOldShield(struct player* me, struct player* oppo) {
    me -> shield[me -> indexRemove] = 0; 
    me -> indexAdd = (me -> indexAdd + 1) % 3;
    me -> indexRemove = (me -> indexRemove + 1) % 3;
    oppo -> shield[oppo -> indexRemove] = 0;
    oppo -> indexAdd = (oppo -> indexAdd + 1) % 3;
    oppo -> indexRemove = (oppo -> indexRemove + 1) % 3;
}

void cardFunction(struct player* player, int card_ID, int buff_ID, volatile int* ULT_INFO_ptr){
    // if player has round buff 2, attack point doubled
    int attack_card_multiplier = buff_ID == 2 ? 2 : 1;
    int shield_multiplier = buff_ID == 1 ? 2 : 1;

    // card from 0-6 is attack, 7-12 is defence
    if (card_ID <= 6){
        card_simple_attack(((card_ID+1) * attack_card_multiplier), player, buff_ID, ULT_INFO_ptr);
    } else {
        card_simple_defence(((card_ID-6) * shield_multiplier), player);
    }
}

/* Functions for rounds */
int chooseCard(struct player* currPlayer, int round_buff, volatile int* MY_CARD_1_ptr,  volatile int* MY_CARD_2_ptr,
                     volatile int* MY_CARD_3_ptr, volatile int* MY_CARD_USED_ptr, volatile int* TIME_ptr,
                     volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* CARD_SELECT_ptr,
                     volatile int* ULT_INFO_ptr) {
    // Draw 3 cards from deck
    int cardOnTable[3];
    int numDrew = 0;

    srand(time(NULL));

    while (numDrew < 3) {
        // get random value between 0 and 12
        int randCard = rand() % card_num;
        // if current player has not used cards[randCard] before put it on table
        if (currPlayer->cards[randCard].valid == true) {
            cardOnTable[numDrew] = currPlayer->cards[randCard].card_ID;
            currPlayer->cards[randCard].valid = false;
            
            numDrew++; // increment number of cards on table
        }
    }

    writeCard(MY_CARD_USED_ptr, 0, false);
    writeCard(MY_CARD_1_ptr, cardOnTable[0], true);
    writeCard(MY_CARD_2_ptr, cardOnTable[1], true);
    writeCard(MY_CARD_3_ptr, cardOnTable[2], true);

    printf("card on table for player %d\n", currPlayer->player_ID);
    for (int i = 0; i < 3; i++) {
        printf("Card %d: ", i);
        if (cardOnTable[i] <= 6) {
            printf("attack with point %d", cardOnTable[i]+1);
        } else {
            printf("shield with point %d", cardOnTable[i]-6);
        }
        printf("\n");
    }

    int cardChosen;
    *TIME_ptr = 29;
    *TIME_ptr = *TIME_ptr | (1 << 7);
    *TIME_ptr = 0;
    cardChosen = getCardFromKbd(currPlayer, round_buff, KEYCODE_ptr, KEYCODE_RST_ptr, CARD_SELECT_ptr, ULT_INFO_ptr); //WHEN CONNECT TO KEYBOARD

    writeCard(MY_CARD_USED_ptr, cardOnTable[cardChosen], true);
    *CARD_SELECT_ptr = 3;

    if (cardChosen == 0) writeCard(MY_CARD_1_ptr, cardOnTable[0], false);
    else if (cardChosen == 1) writeCard(MY_CARD_2_ptr, cardOnTable[1], false);
    else writeCard(MY_CARD_3_ptr, cardOnTable[2], false);

    int myCard = cardOnTable[cardChosen];

    // set valid of not picked cards to true
    for (int a = 0; a < 3; a++) {
        if (a != cardChosen) currPlayer->cards[cardOnTable[a]].valid = true;
    }

    return myCard;
}

char getPlayerMode(volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* INITIAL_SCREEN_ptr) {
    unsigned int kbd = 0x0;

    while(1) {
        kbd = getKeycode(KEYCODE_ptr, KEYCODE_RST_ptr);
        if (kbd == KEY_ENTER1 || kbd == KEY_ENTER2 || kbd == KEY_SPACE) {
            break;
        } else if (kbd == KEY_DOWN || kbd == KEY_UP) {
            *INITIAL_SCREEN_ptr = *INITIAL_SCREEN_ptr == 0 ? 2 : 0; 
        }
    }

    char mode = *INITIAL_SCREEN_ptr == 2 ? 'N' : 'Y';
    return mode;
}

void getEscape(volatile int* KEYCODE_ptr, volatile int* KEYCODE_RST_ptr, volatile int* INITIAL_SCREEN_ptr) {
    unsigned int kbd = 0x0;

    while(1) {
        kbd = getKeycode(KEYCODE_ptr, KEYCODE_RST_ptr);
        if (kbd == KEY_ESC) {
            break;
        }
    }

    *INITIAL_SCREEN_ptr = 0;
    return;
}

int getCardFromKbd(struct player* player, int round_buff, volatile int* KEYCODE_ptr,
                     volatile int* KEYCODE_RST_ptr, volatile int* CARD_SELECT_ptr, volatile int* ULT_INFO_ptr) {
    int cardIdx = 0;
    bool cardChosen = false;
    bool ultInit = (round_buff != 0 && player->ultimate >= 3) ? true : false;

    time_t start = time(NULL);
    unsigned int kbd = 0x0;
    
    while ((time(NULL)-start) <= 30) {
        kbd = getKeycode(KEYCODE_ptr, KEYCODE_RST_ptr);
        if (kbd == KEY_ENTER1 || kbd == KEY_ENTER2 || kbd == KEY_SPACE) {
            cardChosen = true;
            break;
        } else if (kbd == KEY_RIGHT) {
            cardIdx = (cardIdx + 1) % 3;
        } else if (kbd == KEY_LEFT) {
            cardIdx = (cardIdx - 1) < 0 ? 2 : cardIdx - 1;
        } else if (kbd == KEY_U && ultInit) {
            *ULT_INFO_ptr = *ULT_INFO_ptr ^ 0x40;
        }
        *CARD_SELECT_ptr = cardIdx;
    }

    // if player decided to use the ultimate, set ultUseThisRound to true
    if (*ULT_INFO_ptr & 0x40) player->ultUseThisRound = true;

    printf("cardChosen: %d\n", cardChosen);
    /* Used when we decided to use random card when player did not choose card */
    if (cardChosen) {
        return cardIdx;
    } else {
        srand(time(NULL));
        return rand()%3;
    }
}

#endif