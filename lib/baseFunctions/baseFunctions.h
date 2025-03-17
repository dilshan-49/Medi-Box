#ifndef BASEFUNCTIONS_H
#define BASEFUNCTIONS_H

#include <config.h>

void print_time_now();
void update_time(bool mode = false);
void update_time_with_check_alarm();
void ring_alarm();
int wait_for_button_press();
void go_to_menu();
void run_mode(int mode);
void set_time();
void set_alarm(int alarm);
void check_temp();

#endif