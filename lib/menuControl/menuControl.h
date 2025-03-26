#ifndef BASEFUNCTIONS_H
#define BASEFUNCTIONS_H

#include <config.h>

void update_time();
void update_time_with_check_alarm();
int wait_for_button_press();
void go_to_menu();
void run_mode(int mode);
void set_time_zone();
void set_alarm(int alarm);
void view_alarms();

#endif