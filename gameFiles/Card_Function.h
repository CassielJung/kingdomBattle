#include "Data_type.h"

// card functions
void card_simple_attack(int damage, struct player* _player, int buff_ID, volatile int* ULT_INFO_ptr);
void card_simple_defence(int shield, struct player* _player);

void card_simple_attack(int damage, struct player* _player, int buff_ID, volatile int* ULT_INFO_ptr){

    printf("Player%d receives %d damage\n", _player -> player_ID, damage);

    // ultimate 1 is realized here
    if (buff_ID != 0) {
        if (_player -> ultimate == 1){
            printf("Player%d's ultimate 1 is activated, damage blocked. \n", _player -> player_ID);
            _player -> ultimate = 0;

            *ULT_INFO_ptr = *ULT_INFO_ptr - (*ULT_INFO_ptr & 56);
            damage = 0;
        }
    }

    int remain_damage = 0;

    int i = _player->indexRemove;
    if (_player->shield[i] <= damage) {
        remain_damage = damage - _player->shield[i];
        _player->shield[i] = 0;

        if (_player->shield[(i+1)%3] <= remain_damage) {
            remain_damage = remain_damage - _player->shield[(i+1)%3];
            _player->shield[(i+1)%3] = 0;

            if (_player->shield[(i+2)%3] <= remain_damage) {
                remain_damage = remain_damage - _player->shield[(i+2)%3];
                _player->shield[(i+2)%3] = 0;
            } else {
                _player->shield[(i+2)%3] -= remain_damage;
                remain_damage = 0;
            }
        } else {
            _player->shield[(i+1)%3] -= remain_damage;
            remain_damage = 0;
        }
    } else {
        _player->shield[i] -= damage;
        remain_damage = 0;
    }

    if (remain_damage > 0) {
        _player -> health = _player -> health - remain_damage;
    }

    printf("\n");
    printf("Player%d:\n    Shield: %d\n    Health: %d\n", _player -> player_ID, _player -> shield[0] + _player -> shield[1] + _player -> shield[2], _player -> health);
    printf("\n");

    if (_player -> health <= 0){
        printf("Player%d is eliminated!", _player -> player_ID);

        _player -> alive = false;
    }
}

void card_simple_defence(int shield, struct player* _player){
    _player -> shield[_player -> indexAdd] = shield + _player -> shield[_player -> indexAdd];

    printf("Player%d receives %d shield\n", _player -> player_ID, shield);

    printf("Shield: %d %d %d\n", _player -> shield[0], _player -> shield[1], _player -> shield[2]);

    printf("\n");
    printf("Player%d:\n    Shield: %d\n    Health: %d\n", _player -> player_ID, _player -> shield[0] + _player -> shield[1] + _player -> shield[2], _player -> health);
    printf("\n");
}