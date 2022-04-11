//
// Created by 23612 on 2022/3/15.
//

#ifndef PROJECT_C_INPUT_TEST_OUT_H
#define PROJECT_C_INPUT_TEST_OUT_H

#include "Data_type.h"

void input_test_begin(int count);
int input_test(int lower_bound, int upper_bound, int not_allowed, int player_id);
int input_test_ult(int lower_bound, int upper_bound, int not_allowed, int player_id);
void input_test_end(int winner);
void input_test_end_round_p();
void input_test_end_round();

void input_test_record(char mode, int num, int player_id);
#endif //PROJECT_C_INPUT_TEST_OUT_H
