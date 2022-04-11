#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

// other files
#include "Keyboard.h"

// need to be changed
#include "Data_type.h"
#include "Card_Function.h"
#include "ultimate_function.h"
#include "input_test_out.h"

// 13 cards
struct card* cardInitialization(int player);
void cardFunction(struct card _card, struct player* _player, int round_buff_id);
void ultimateInitialization(struct player* players);


// global variable
int playerNum;

int main(){

    int count = 0;

    while(count < test_loop) {

        playerNum = 2;

        printf("0");

        struct player *player_array = malloc(playerNum * sizeof(struct player));

        // initialize players
        for (int i = 0; i < playerNum; i++){
            player_array[i].cards = cardInitialization(i); //stores the pointers
            player_array[i].player_ID = i;
            player_array[i].health = 20;
            player_array[i].shield[0] = 0;
            player_array[i].shield[1] = 0;
            player_array[i].shield[2] = 0;
            player_array[i].indexAdd = 0;
            player_array[i].indexRemove = 1;
            player_array[i].time_limit = 0; // 20 seconds time limit
            player_array[i].alive = true;

            //player_array[i].cardHolds = malloc(sizeof(struct card));
        }

        input_test_begin(count);

        // initialize ultimates
        ultimateInitialization(player_array);

        int p_index = 0;

        int round = 0;
        bool roundbegin = true;
        int round_buff_id = -1; // not initialized

        int player_num_alive = playerNum;

        ////////////////////////////////////////////////////
        // game play in round
        while (round < max_round && player_num_alive > 1) {
            // do something with keyboard
            if (roundbegin) {
                roundbegin = false;
                /////////////////Check for round buff/////////////////
                round_buff_id = (rand() + (int) time(NULL)) % round_buff_num; // fake random..
                printf("----------Round Buff %d----------\n", round_buff_id);

                input_test_record('r',  round_buff_id, 0);

                if (round_buff_id == 0) {
                    printf("All active ultimates are disabled in this round! \n");
                } else {
                    if (round_buff_id == 3) {
                        p_index = playerNum - 1;
                    }
                    /////////////////Begin of the round/////////////////
                    /////////////////Check for ultimate 3 / 4(active)/////////////////
                    for (int i = 0; i < playerNum; i++) {
                        if (player_array[i].ultimate == 3) {
                            printf("Player%d, press a number of the opponent to activate ultimate 3 in this round in 5 seconds \n",
                                   player_array[i].player_ID);
                            clock_t thetime = clock();

                            int opid = input_test_ult(-1, playerNum, i, player_array[i].player_ID);

                            printf("Ultimate 3 is activated! \n");

                            player_array[i].ultimate = 300;

                            not_active:
                            continue;
                        } else if (player_array[i].ultimate == 4) {
                            printf("Player%d, press a number of the opponent to activate ultimate 4 in this round in 5 seconds \n",
                                   player_array[i].player_ID);
                            clock_t thetime = clock();

                            int opid = input_test_ult(-1, playerNum, i, player_array[i].player_ID);

                            player_array[opid].shield[0] = 0;
                            player_array[opid].shield[1] = 0;
                            player_array[opid].shield[2] = 0;
                            player_array[i].ultimate = 0;
                            printf("Ultimate 4 is activated! \n");

                            not_active2:
                            continue;
                        }
                    }
                }

            }

            struct player *current_player = &player_array[p_index];

            if (current_player->alive) {
                printf("Round %d(player%d)\n\n", round, current_player->player_ID);

                int num_alive = 0;

                current_player->time_limit = clock(); // set current time to player

                // randomly draw 3 cards from the card deck...
                struct card *current_card_deck = malloc(card_deck_num * sizeof(struct card));

                // if the player has ultimate 3... then

                // probably need to update timer...
                while (clock() - current_player->time_limit <= 20 && current_player->alive) {

                    // randomly choose 3 cards
                    int index = 0;

                    // currently, can draw out multiple same card !!!
                    while (index < 3) { // if the card is less than 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        int card_index = (rand() + (int) time(NULL)) % card_num; // 0 - 12, need a better random?
                        if (current_player->cards[card_index].valid == true) {
                            current_card_deck[index] = current_player->cards[card_index]; // append the card to the array, new copies
                            current_player->cards[card_index].valid = false;

                            printf("Card %d ---> Num%d\n", current_card_deck[index].card_ID, index + 1);

                            index++;
                        }
                    }

                    printf("\n");

                    // reset the true / false
                    index = 0;
                    while (index < 3) {
                        current_player->cards[current_card_deck[index].card_ID].valid = true;
                        index++;
                    }

                    ////////////////////////////////////////////////////
                    // user choose card
                    //user_card_choice(current_player, current_card_deck);//

                    input_test_record('h',  current_player -> health, current_player -> player_ID);
                    input_test_record('s',  current_player -> shield[0] + current_player -> shield[1] + current_player -> shield[2], current_player -> player_ID);

                    input_test_record('h',  player_array[(p_index + 1) % playerNum].health, player_array[(p_index + 1) % playerNum].player_ID);
                    input_test_record('s',  player_array[(p_index + 1) % playerNum].shield[0] + player_array[(p_index + 1) % playerNum].shield[1] + player_array[(p_index + 1) % playerNum].shield[2], player_array[(p_index + 1) % playerNum].player_ID);

                    int index_user = -1;
                    int typeIn = input_test(0, 4, -1, current_player->player_ID);
                    index_user = typeIn - 1;

                    current_player->cardHolds = current_card_deck[index_user];

                    // card being choosen
                    input_test_record('c',  (current_player->cardHolds).card_ID, current_player -> player_ID);

                    printf("Card %d being chosen\n", (current_player->cardHolds).card_ID);

                    // choose an opponent
                    //user_object_choice(current_player, playerNum, player_array);//

                    if ((current_player->cardHolds).card_ID > 6){
                        index_user = input_test(-1, playerNum, (current_player -> player_ID + 1) % 2, current_player->player_ID);
                    }
                    else{
                        index_user = input_test(-1, playerNum, current_player -> player_ID, current_player->player_ID);;
                    }

                    ////////////////////////////////////////////////////
                    current_player->objectID = index_user;

                    printf("Player%d chooses the card ", current_player->player_ID);
                    printf("%d, used on Player%d", (current_player->cardHolds).card_ID, current_player->objectID);
                    printf("\n");

                    cardFunction(current_player->cardHolds, &player_array[current_player->objectID],
                                 round_buff_id); //...

                    // Object directed
                    if (player_array[current_player->objectID].ultimate > 100) {
                        player_array[current_player->objectID].ultimate = 0;
                        int opid = (current_player->objectID + 1) % 2;
                        printf("Object directed from %d to %d! \n", current_player->objectID, opid);
                        cardFunction(current_player->cardHolds, &player_array[opid],
                                     round_buff_id);
                    }

                    // disable using card id
                    current_player->cards[(current_player->cardHolds).card_ID].valid = false;

                    // Timer works well now

                    out_of_time:

                    // Check for number of players alive

                    for (int i = 0; i < playerNum; i++) {
                        if (player_array[i].alive) {
                            num_alive++;
                        }
                    }

                    player_num_alive = num_alive;

                    free(current_card_deck);

                    if (player_num_alive <= 1) {
                        goto game_finished;
                    }

                    break;
                }

                current_player -> shield[current_player -> indexRemove] = 0; // Remove the old shield
                current_player -> indexAdd = (current_player -> indexAdd + 1) % 3;
                current_player -> indexRemove = (current_player -> indexRemove + 1) % 3;

                printf("End Round (player%d)\n\n", current_player->player_ID);

                input_test_end_round_p();
            }

            // keep the loop
            if (round_buff_id == 3) {
                p_index--;
                if (p_index < 0) {
                    p_index = 0;

                    // all players used
                    round++;
                    roundbegin = true;

                    input_test_end_round();
                }
            } else {
                p_index++;
                if (p_index >= playerNum) {
                    p_index = 0;

                    // all players used
                    round++;
                    roundbegin = true;

                    input_test_end_round();
                }
            }
        }

        game_finished:
        printf("Game finished!");

        int winner_index = 0;
        int maxPoint = 0;
        int point;

        for (int i = 0; i < playerNum; i++) {
            if (player_array[i].alive) {
                point = player_array[i].health + player_array[i].shield[0] + player_array[i].shield[1] + player_array[i].shield[2];
                if (point > maxPoint) {
                    maxPoint = point;
                    winner_index = i;
                }
            }
        }

        struct player *winner = &player_array[winner_index];

        printf("Winner is %d", winner->player_ID);

        input_test_end(winner->player_ID);

        count++;
    }

    return 0;
}

struct card* cardInitialization(int playerID){
    // array of cards
    struct card* card_array = malloc((card_num + 1) * sizeof(struct card));

    for (int i = 0; i < card_num; i++){
        card_array[i].card_ID = i; // index
        card_array[i].player_ID = playerID;
        card_array[i].valid = true; // set card to valid
    }

    return card_array;
}

// now we suppose card 1 - 6 is damage card, 7 - 13 is difference card
void cardFunction(struct card _card, struct player* player, int round_buff_id){
    int attack_card_multiplier = 1;
    int shield_card_multiplier = 1;

    if (round_buff_id == 2){
        attack_card_multiplier = 2;
    }
    else if (round_buff_id == 1){
        shield_card_multiplier = 2;
    }

    if (_card.card_ID == 1){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }
    else if (_card.card_ID == 2){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }
    else if (_card.card_ID == 3){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }
    else if (_card.card_ID == 4){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }
    else if (_card.card_ID == 5){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }
    else if (_card.card_ID == 6){
        card_simple_attack((_card.card_ID + 1) * attack_card_multiplier, player);
    }

        // defensive card here
    else if (_card.card_ID == 7){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 8){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 9){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 10){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 11){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 12){
        card_simple_defence((_card.card_ID - 6) * shield_card_multiplier, player);
    }
    else if (_card.card_ID == 0){ // need to change
        card_simple_defence((_card.card_ID + 1) * attack_card_multiplier, player);
    }
}

void ultimateInitialization(struct player* players){
    // I don't know why this doesnt show... ***
    FILE* file = fopen("ult_description.txt", "r");
    int c;

    if (file) {
        while ((c = getc(file)) != EOF)
            putchar(c);
        fclose(file);
    }

    for (int i = 0; i < playerNum; i++){

        players[i].ultimate = (rand() + (int) time(NULL)) % ult_num;;

        input_test_record('u',  players[i].ultimate, players[i].player_ID);

        if (players[i].ultimate == 2){
            players[i].health = 25;
        }

        printf("Player%d received ultimate %d!\n", players[i].player_ID, players[i].ultimate);
    }
}