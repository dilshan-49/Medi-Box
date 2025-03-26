#ifndef MENUCONTROL_H
#define MENUCONTROL_H

#include <config.h>

int wait_for_button_press();
void go_to_menu();
void run_mode(int mode);
void set_time_zone();
void set_alarm(int alarm);
void view_alarms();

#endif