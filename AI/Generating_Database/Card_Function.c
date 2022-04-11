#include "Card_Function.h"
#include <time.h>

void card_simple_attack(int damage, struct player* _player){

    printf("The object of attack is Player%d\n", _player -> player_ID);

    printf("Player%d receives %d damage\n", _player -> player_ID, damage);

    // ultimate 1 is realized here
    if (_player -> ultimate == 1){
        printf("Player%d's ultimate 1 is activated, damage blocked. \n", _player -> player_ID);
        _player -> ultimate = 0;
        damage = 0;
    }

    int remain_damage = 0;

    _player -> shield[_player -> indexRemove] = _player -> shield[_player -> indexRemove] - damage;
    if (_player -> shield[_player -> indexRemove] <= 0){
        remain_damage = 0 - _player -> shield[_player -> indexRemove];
        _player -> shield[_player -> indexRemove] = 0;

        if (remain_damage > 0){
            _player -> shield[(_player -> indexRemove + 1) % 3] = _player -> shield[(_player -> indexRemove + 1) % 3] - damage;
            if(_player -> shield[(_player -> indexRemove + 1) % 3] <= 0){
                remain_damage = 0 - _player -> shield[(_player -> indexRemove + 1) % 3];
                _player -> shield[(_player -> indexRemove + 1) % 3] = 0;

                if (remain_damage > 0){
                    _player -> shield[(_player -> indexRemove + 2) % 3] = _player -> shield[(_player -> indexRemove + 2) % 3] - damage;
                    if(_player -> shield[(_player -> indexRemove + 2) % 3] <= 0) {
                        remain_damage = 0 - _player->shield[(_player->indexRemove + 2) % 3];
                        _player->shield[(_player->indexRemove + 2) % 3] = 0;
                    }
                }
            }
        }
    }

    if (remain_damage > 0){
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