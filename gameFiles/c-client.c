#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// personal header files
#include "game.h"
#include "vga.h"

#define MAX 1024
#define PORT 3000
#define SA struct sockaddr

//Writes a message typed into the terminal to the server
void terminalToServer(int sockfd, char * buff, int n, int size );

//Writes a given string to server
void writeToServer(int sockfd, char * buff, int size );

//Gets Player Nickname and if he wants to play vs AI
bool initializePlayer(int sockfd);

/* POINTERS FOR VGA OUTPUT */
volatile int * KEYCODE_ptr;
volatile int * KEYCODE_RST_ptr;
volatile int * INITIAL_SCREEN_ptr;
volatile int * CARD_SELECT_ptr;
volatile int * MY_CARD_1_ptr;
volatile int * MY_CARD_2_ptr;
volatile int * MY_CARD_3_ptr;
volatile int * MY_CARD_USED_ptr;
volatile int * ENEMY_CARD_USED_ptr;
volatile int * ENEMY_CARD_VISIBLE_ptr;
volatile int * MY_HP_ptr;
volatile int * MY_SHIELD_ptr;
volatile int * TIME_ptr;
volatile int * ROUND_ptr;
volatile int * ENEMY_HP_ptr;
volatile int * ENEMY_SHIELD_ptr;
volatile int * BUFF_ptr;
volatile int * ULT_INFO_ptr; 
volatile int * ENDING_INFO_ptr;
volatile int * SHOW_INSTRUCTION_ptr; 


void func(int sockfd)
{
    /* VARIABLES */
    char buff[MAX];
    char myRoundBuff[MAX];
    int n;

    bool isFirst; // tell who is chosing card first
    int myID;
    int oppoID;
    int round_buff;  
    struct player* getEffect; 
    unsigned int round = 1;
    *ROUND_ptr = round;

    bool isAI = false;
    isAI = initializePlayer(sockfd);

    /* INITIALIZATION */
    // 1. FROM SERVER: get order
    isFirst = false; //defaults to false
    myID = isFirst ? 0 : 1; // if I play first, my Id is 0
    oppoID = isFirst ? 1 : 0;

    // 2. initialize player array
    struct player* player_array[player_num];
    for (int i = 0; i < player_num; i++) {
        player_array[i] = playerInit(i);
    }

    char oldRoundBuff;

    // 3. Get Ults from server and Initialize them
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("Ults are set to : %s\n", buff);
    char myUltChar = buff[0];
    int myUlt = myUltChar - '0';
    int oppoUlt = atoi(&buff[1]);

    // VGA: show ultimate on VGA screen
    int ult_info = 0;
    if (oppoUlt == 1 || oppoUlt == 2) ult_info += (1 << 7);
    if (myUlt == 1 || myUlt == 2) ult_info += (1 << 6);
    ult_info += myUlt + (oppoUlt << 3);
    *ULT_INFO_ptr = ult_info;
    *ENEMY_CARD_VISIBLE_ptr = 0b111;

    player_array[myID]->ultimate = myUlt;
    player_array[oppoID]->ultimate = oppoUlt;

    // ultimate 2 gives impact right away
    if (myUlt == 2) player_array[myID]->health = 25;
    if (oppoUlt == 2) player_array[oppoID]->health = 25;
    *MY_HP_ptr = (unsigned int) player_array[myID]->health;
    *ENEMY_HP_ptr = (unsigned int) player_array[oppoID]->health;
    printf("my ultimate: %d, oppo ultimate: %d\n", myUlt, oppoUlt);

    //Start gaming (Gaming Loop)
    for (;;) {
        // Should contain the cards + ultimate played from opponent:
        // 1st round server lets you know if you are the first one to play
        bzero(buff, sizeof(buff));
        *TIME_ptr = 29;
        *TIME_ptr = *TIME_ptr | (1 << 7);
        *TIME_ptr = 0;
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s\n", buff);
        
        //start contains the round buff for the very first round for some python reason
        char start = '\0';
        //only get the start char when string received from sever is: "You will start the round"
        if(strlen(buff) > 10) {
            start = buff[strlen(buff)-1];
        }
        
        // Regular round calculations should go here:
        // buff contains opponent's card played and myRoundBuff contains round buff id******
        if ((strncmp(&start, "0", 1)) != 0 && (strncmp(&start, "1", 1)) != 0 && (strncmp(&start, "2", 1)) != 0) {
            // SERVER: should contain roundBuff number
            bzero(myRoundBuff, sizeof(myRoundBuff));
            read(sockfd, myRoundBuff, sizeof(myRoundBuff));
            
            // GAME: if I'm the first player, keep the oldRoundBuff
            int round_buff = (isFirst || *myRoundBuff-'0' < 0) ? oldRoundBuff : *myRoundBuff-'0';
            *BUFF_ptr = (unsigned int) round_buff;
            if (!isFirst) printf("myRoundBuff: %d\n", round_buff);

            // SERVER: get opponent information
            // buff[ult| |ultUseThisRound| |cardNum]
            player_array[oppoID]->ultimate = atoi(&buff[0]);
            player_array[oppoID]->ultUseThisRound = atoi(&buff[2]);
            int opponentCard = atoi(&buff[4]);

            // VGA: change visibility of ultimate
            if (player_array[oppoID]->ultUseThisRound) {
                *ULT_INFO_ptr = *ULT_INFO_ptr | 0x80;
            }

            // VGA: change visibility of opponent cards on table
            srand(time(NULL));
            int randOppCard = rand() % 3;
            if (randOppCard == 0) *ENEMY_CARD_VISIBLE_ptr = 0b110;
            else if (randOppCard == 1) *ENEMY_CARD_VISIBLE_ptr = 0b101;
            else *ENEMY_CARD_VISIBLE_ptr = 0b011;

            // VGA: display opponent card used 
            writeCard(ENEMY_CARD_USED_ptr, opponentCard, true);

            // GAME: apply impact of opponent's ultimate 4; delete all shield that I have
            if (player_array[oppoID]->ultimate == 4 && player_array[oppoID]->ultUseThisRound) {
                player_array[myID]->shield[0] = 0;
                player_array[myID]->shield[1] = 0;
                player_array[myID]->shield[2] = 0;

                // VGA: reset my shield to 0
                *MY_SHIELD_ptr = 0;

                player_array[oppoID]->ultimate = 0;
                *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 56);
                player_array[oppoID]->ultUseThisRound = false;
            }
            
            // GAME: if I used ultimate 3 before, duplicate opponent's behaviour
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate == 3) {
                printf("ultimate 3 is enabled! Duplicate opponent's attack\n");
                cardFunction(player_array[oppoID], opponentCard, round_buff, ULT_INFO_ptr);
                cardFunction(player_array[myID], opponentCard, round_buff, ULT_INFO_ptr);

                player_array[myID]->ultimate = 0; // remove ultimate
                *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 7);
                player_array[myID]->ultUseThisRound = false; // ultimate is already used
            } else {
                getEffect = opponentCard <= 6 ? player_array[myID] : player_array[oppoID];
                cardFunction(getEffect, opponentCard, round_buff, ULT_INFO_ptr);
            }

            // VGA: change HP and shield info for both player
            *MY_HP_ptr = player_array[myID]->health;
            *MY_SHIELD_ptr = player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
            *ENEMY_HP_ptr = player_array[oppoID]->health;
            *ENEMY_SHIELD_ptr = player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];

            // GAME & SERVER: if one player died, game ended
            if (!player_array[myID]->alive) {
                *ENDING_INFO_ptr = 3;
                *MY_HP_ptr = 0;
                *MY_SHIELD_ptr = 0;

                char gameEnded[MAX] = "gameEnded";
                writeToServer(sockfd, gameEnded, sizeof(gameEnded));

                return;
            } else if (!player_array[oppoID]->alive) {
                *ENDING_INFO_ptr = 1;
                *ENEMY_HP_ptr = 0;
                *ENEMY_SHIELD_ptr = 0;

                return;
            }

            // VGA: turn off ultimate if it was used
            if (*ULT_INFO_ptr & 0x40) *ULT_INFO_ptr = *ULT_INFO_ptr - 0x40;

            // GAME: If I am the first player, round ends here 
            if (isFirst) {
                // GAME: get new round buff
                round_buff = *myRoundBuff-'0';

                // GAME: Remove old shield of both players
                removeOldShield(player_array[myID], player_array[oppoID]);

                // VGA: show new round buff
                *BUFF_ptr = (unsigned int) round_buff;
                oldRoundBuff = round_buff;
                printf("myRoundBuff : %s\n", myRoundBuff);

                // VGA: shows new shield
                *MY_SHIELD_ptr = player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
                *ENEMY_SHIELD_ptr = player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];

                // VGA: shows next round
                round++;

                if (round == 11) {
                    printf("round==11\n");
                    int myScore = player_array[myID]->health + player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
                    int oppoScore = player_array[oppoID]->health + player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];

                    if (myScore < oppoScore) {
                        *ENDING_INFO_ptr = 3;
                        *MY_HP_ptr = 0;
                        *MY_SHIELD_ptr = 0;
                    } else if (myScore > oppoScore) {
                        *ENDING_INFO_ptr = 1;
                        *ENEMY_HP_ptr = 0;
                        *ENEMY_SHIELD_ptr = 0;
                    }
                    
                    if (!isAI) {
                        char gameEnded[MAX] = "gameEnded";
                        writeToServer(sockfd, gameEnded, sizeof(gameEnded));\
                    }

                    return;
                }

                *ROUND_ptr = round;

                printf("player%d shield [%d %d %d]\n", myID, player_array[myID]->shield[0], player_array[myID]->shield[1], player_array[myID]->shield[2]);
                printf("player%d shield [%d %d %d]\n", oppoID, player_array[oppoID]->shield[0], player_array[oppoID]->shield[1], player_array[oppoID]->shield[2]);
            }

            // GAME & VGA: choose card/ultimate usage and change visibility on VGA
            *SHOW_INSTRUCTION_ptr = player_array[myID]->ultimate >= 3 ? 3 : 1;
            int myCard = chooseCard(player_array[myID], round_buff, MY_CARD_1_ptr, MY_CARD_2_ptr, MY_CARD_3_ptr, MY_CARD_USED_ptr, TIME_ptr,
                                        KEYCODE_ptr, KEYCODE_RST_ptr, CARD_SELECT_ptr, ULT_INFO_ptr);
            *SHOW_INSTRUCTION_ptr = 0;

            // GAME & VGA: if player decided to use the ultimate and it was 4, remove opponent's shield
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate == 4) {
                player_array[oppoID]->shield[0] = 0;
                player_array[oppoID]->shield[1] = 0;
                player_array[oppoID]->shield[2] = 0;

                *ENEMY_SHIELD_ptr = 0;
            }

            // GMAE: if card is for attack opponent get effect, if card is for shield I get effect
            if (player_array[oppoID]->ultUseThisRound && player_array[oppoID]->ultimate == 3) {
                printf("ultimate 3 is enabled! Duplicate opponent's attack\n");
                cardFunction(player_array[oppoID], myCard, round_buff, ULT_INFO_ptr);
                cardFunction(player_array[myID], myCard, round_buff, ULT_INFO_ptr);

                player_array[oppoID]->ultimate = 0; // remove ultimate
                *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 56);
                player_array[oppoID]->ultUseThisRound = false; // ultimate is already used
            } else {
                getEffect = myCard <= 6 ? player_array[oppoID] : player_array[myID];
                cardFunction(getEffect, myCard, round_buff, ULT_INFO_ptr);
            }

            // VGA: change HP and shield info on VGA for me and enemy
            *MY_HP_ptr = player_array[myID]->health;
            *MY_SHIELD_ptr = player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
            *ENEMY_HP_ptr = player_array[oppoID]->health;
            *ENEMY_SHIELD_ptr = player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];            
            printf("\n your remaining health: %d \n", player_array[myID]->health);

            // VGA: turn off opponent ultimate if it was used
            if (*ULT_INFO_ptr & 0x80) {
                *ULT_INFO_ptr = *ULT_INFO_ptr - 0x80;
            }

            // SERVER: writing ultimate decisions and card choice to the server
            char myCardString[MAX];
            sprintf(myCardString, "%d %d %d", player_array[myID]->ultimate, player_array[myID]->ultUseThisRound, myCard);
            writeToServer(sockfd, myCardString, sizeof(myCardString));
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate == 4) {
                player_array[myID]->ultimate = 0;
                *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 7);
            }
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate != 3) {
                player_array[myID]->ultUseThisRound = false;
            }

            // GAME & SERVER: if one player died, game ended
            if (!player_array[myID]->alive) {
                *ENDING_INFO_ptr = 3;
                *MY_HP_ptr = 0;
                *MY_SHIELD_ptr = 0;

                char gameEnded[MAX] = "gameEnded";
                writeToServer(sockfd, gameEnded, sizeof(gameEnded));

                return;
            } else if (!player_array[oppoID]->alive) {
                *ENDING_INFO_ptr = 1;
                *ENEMY_HP_ptr = 0;
                *ENEMY_SHIELD_ptr = 0;

                return;
            }

            // changed
            if (isAI){
                char gamestat[MAX];
                sprintf(gamestat, "%d %d %d %d %s", player_array[oppoID]->health, player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1]
                                                                                  + player_array[oppoID]->shield[2], player_array[myID]->health, player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2], "thestatus");
                writeToServer(sockfd, gamestat, sizeof(gamestat));
            }
        
            // If I am the second player, this is where the round ends
            if (!isFirst) {
                // GAME: remove old shield
                removeOldShield(player_array[myID], player_array[oppoID]);

                if (round == 10) {
                    int myScore = player_array[myID]->health + player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
                    int oppoScore = player_array[oppoID]->health + player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];

                    if (myScore < oppoScore) {
                        *ENDING_INFO_ptr = 3;
                        *MY_HP_ptr = 0;
                        *MY_SHIELD_ptr = 0;
                    } else if (myScore > oppoScore) {
                        *ENDING_INFO_ptr = 1;
                        *ENEMY_HP_ptr = 0;
                        *ENEMY_SHIELD_ptr = 0;
                    }

                    return;
                }

                // GAME & VGA: increment round
                round++;
                *ROUND_ptr = round;

                // VGA: remove old shield
                *MY_SHIELD_ptr = player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
                *ENEMY_SHIELD_ptr = player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];
 
                printf("player%d shield [%d %d %d]\n", myID, player_array[myID]->shield[0], player_array[myID]->shield[1], player_array[myID]->shield[2]);
                printf("player%d shield [%d %d %d]\n", oppoID, player_array[oppoID]->shield[0], player_array[oppoID]->shield[1], player_array[oppoID]->shield[2]);
            }

            // VGA: reset enemy card visibility
            *ENEMY_CARD_VISIBLE_ptr = 0b111;
            writeCard(ENEMY_CARD_USED_ptr, 0, false);

        } else {
            //Means you are the first one to play!
            printf("\nmyRoundBuff : %c\n", start);

            int round_buff = start-'0';
            oldRoundBuff = round_buff;
            *BUFF_ptr = round_buff;
            
            isFirst = true;
            myID = isFirst ? 0 : 1; // if I play first, my Id is 0
            oppoID = isFirst ? 1 : 0;

            // 3. initialize ultimate based on the newly determined ID
            player_array[myID]->ultimate = myUlt;
            player_array[oppoID]->ultimate = oppoUlt;

            int ult_info = 0;
            if (myUlt == 1 || myUlt == 2) ult_info += (1 << 6);
            if (oppoUlt == 1 || oppoUlt == 2) ult_info += (1 << 7);
            ult_info += myUlt + (oppoUlt << 3);
            *ULT_INFO_ptr = ult_info;

            int tempHP = player_array[myID]->health;
            player_array[myID]->health = player_array[oppoID]->health;
            player_array[oppoID]->health = tempHP;
            printf("my ultimate: %d, oppo ultimate: %d\n", myUlt, oppoUlt);

            *SHOW_INSTRUCTION_ptr = player_array[myID]->ultimate >= 3 ? 3 : 1;
            int myCard = chooseCard(player_array[myID], round_buff, MY_CARD_1_ptr, MY_CARD_2_ptr, MY_CARD_3_ptr, MY_CARD_USED_ptr, TIME_ptr,
                                        KEYCODE_ptr, KEYCODE_RST_ptr, CARD_SELECT_ptr, ULT_INFO_ptr);
            *SHOW_INSTRUCTION_ptr = 0;
            // GAME: ultimate 4; disable shield of opponent
            if (player_array[myID]->ultUseThisRound) {
                if (player_array[myID]->ultimate == 4) {
                    player_array[oppoID]->shield[0] = 0;
                    player_array[oppoID]->shield[1] = 0;
                    player_array[oppoID]->shield[2] = 0;

                    *ENEMY_SHIELD_ptr = 0;
                }
            }

            // GAME: if card is for attack opponent get effect, if card is for shield I get effect
            getEffect = myCard <= 6 ? player_array[oppoID] : player_array[myID];
            cardFunction(getEffect, myCard, round_buff, ULT_INFO_ptr);
            printf("\n your remaining health: %d \n", player_array[myID]->health);

            // VGA: with new HP and shields
            *MY_HP_ptr = player_array[myID]->health;
            *MY_SHIELD_ptr = player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2];
            *ENEMY_HP_ptr = player_array[oppoID]->health;
            *ENEMY_SHIELD_ptr = player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1] + player_array[oppoID]->shield[2];  

            // GAME & SERVER: if one player died, game ended
            if (!player_array[myID]->alive) {
                *ENDING_INFO_ptr = 3;
                *MY_HP_ptr = 0;
                *MY_SHIELD_ptr = 0;

                char gameEnded[MAX] = "gameEnded";
                writeToServer(sockfd, gameEnded, sizeof(gameEnded));

                return;
            } else if (!player_array[oppoID]->alive) {
                *ENDING_INFO_ptr = 1;
                *ENEMY_HP_ptr = 0;
                *ENEMY_SHIELD_ptr = 0;

                return;
            }

            // changed
            if (isAI){
                char gamestat[MAX];
                sprintf(gamestat, "%d %d %d %d %s", player_array[oppoID]->health, player_array[oppoID]->shield[0] + player_array[oppoID]->shield[1]
                                                                                  + player_array[oppoID]->shield[2], player_array[myID]->health, player_array[myID]->shield[0] + player_array[myID]->shield[1] + player_array[myID]->shield[2], "thestatus");
                writeToServer(sockfd, gamestat, sizeof(gamestat));
            }

            //writing our card of choice to the server:
            char myCardString[MAX];
            sprintf(myCardString, "%d %d %d", player_array[myID]->ultimate, player_array[myID]->ultUseThisRound, myCard);
            writeToServer(sockfd, myCardString, sizeof(myCardString));
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate==4) {
                player_array[myID]->ultimate = 0;
                *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 7);
            }
            if (player_array[myID]->ultUseThisRound && player_array[myID]->ultimate != 3) {
                player_array[myID]->ultUseThisRound = false;
            }
        }

    }
}
   
int main()
{
    /* map memory */
    int fd = -1;               // used to open /dev/mem for access to physical addresses
    void *LW_virtual;          // used to map physical addresses for the light-weight bridge

    // Create virtual memory access to the FPGA light-weight bridge
    if ((fd = open_physical (fd)) == -1)
        return (-1);
    if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL)
        return (-1);
    
    // Initialize pointer address
    KEYCODE_ptr = (unsigned int *) (LW_virtual + 0x00000010);
    KEYCODE_RST_ptr = (unsigned int *) (LW_virtual + 0x00000000);
    INITIAL_SCREEN_ptr = (unsigned int *) (LW_virtual + 0x00000020);
    CARD_SELECT_ptr = (unsigned int *) (LW_virtual + 0x00000030);
    MY_CARD_1_ptr = (unsigned int *) (LW_virtual + 0x00000040);
    MY_CARD_2_ptr = (unsigned int *) (LW_virtual + 0x00000050);
    MY_CARD_3_ptr = (unsigned int *) (LW_virtual + 0x00000060);
    MY_CARD_USED_ptr = (unsigned int *) (LW_virtual + 0x00000070);
    ENEMY_CARD_USED_ptr = (unsigned int *) (LW_virtual + 0x00000080);
    ENEMY_CARD_VISIBLE_ptr = (unsigned int *) (LW_virtual + 0x00000090);
    MY_HP_ptr = (unsigned int *) (LW_virtual + 0x000000a0);
    MY_SHIELD_ptr = (unsigned int *) (LW_virtual + 0x000000b0);
    TIME_ptr = (unsigned int *) (LW_virtual + 0x000000c0);
    ROUND_ptr = (unsigned int *) (LW_virtual + 0x000000d0);
    ENEMY_HP_ptr = (unsigned int *) (LW_virtual + 0x000000e0);
    ENEMY_SHIELD_ptr = (unsigned int *) (LW_virtual + 0x000000f0);
    BUFF_ptr = (unsigned int *) (LW_virtual + 0x00000100);
    ULT_INFO_ptr = (unsigned int *) (LW_virtual + 0x00000110);
    ENDING_INFO_ptr = (unsigned int *) (LW_virtual + 0x00000120);
    SHOW_INSTRUCTION_ptr = (unsigned int *) (LW_virtual + 0x00000130); 

    while (1) {
        // Initialize pointer values
        *INITIAL_SCREEN_ptr = 0;
        *ENEMY_CARD_VISIBLE_ptr = 0;
        *CARD_SELECT_ptr = 3;
        *MY_CARD_1_ptr = 0;
        *MY_CARD_2_ptr = 0;
        *MY_CARD_3_ptr = 0;
        *MY_CARD_USED_ptr = 0;
        *MY_HP_ptr = 0;
        *MY_SHIELD_ptr = 0;
        *ENEMY_CARD_USED_ptr = 0;
        *TIME_ptr = 0;
        *ROUND_ptr = 0;
        *ENEMY_HP_ptr = 0;
        *ENEMY_SHIELD_ptr = 0;
        *KEYCODE_RST_ptr = 1;
        *KEYCODE_RST_ptr = 0;
        *ENDING_INFO_ptr = 0;
        *SHOW_INSTRUCTION_ptr = 0;
        
        int sockfd, connfd;
        struct sockaddr_in servaddr, cli;
        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));
    
        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("192.168.0.101");
        printf("%d\n", PORT);
        servaddr.sin_port = htons(PORT);
    
        // connect the client socket to server socket
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("connected to the server...\n");
    
        // function for chat
        func(sockfd);
        
        // close the socket
        close(sockfd);

        getEscape(KEYCODE_ptr, KEYCODE_RST_ptr, INITIAL_SCREEN_ptr);
    }
}

//write a given string to server
void writeToServer(int sockfd, char * buff, int size ){
    //writing our card of choice to the server:
    write(sockfd, buff, size);
}

//writes a message typed into the terminal to the server
void terminalToServer(int sockfd, char * buff, int n, int size ){
     //writing our card of choice to the server:
    bzero(buff, size);
    printf("Play a card (1-4) : ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, size);
    bzero(buff, size);
}

//Gets Player Nickname and if he wants to play vs AI
bool initializePlayer(int sockfd) {
    char buff[MAX];
    char myRoundBuff[MAX];
    int n;

    bool toReturn = false;
    //Single Player
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s\n", buff);
    bzero(buff, sizeof(buff));
    printf("Would you like to play Singleplayer mode (Y/N): ");
    buff[0] = getPlayerMode(KEYCODE_ptr, KEYCODE_RST_ptr, INITIAL_SCREEN_ptr);

    // changed important! how to detect AI
    if (buff[0] == 'Y' || buff[0] == 'y'){
        toReturn = true;
    }

    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    *INITIAL_SCREEN_ptr = 1;
    
    //Nickname
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s\n", buff);
    bzero(buff, sizeof(buff));
    printf("Choose your Nickname: ");
    sprintf(buff, "%d", (int) time(NULL));
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));

    return toReturn;
}