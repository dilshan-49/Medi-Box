#ifndef BASEFUNCTIONS_H
#define BASEFUNCTIONS_H

int zone_minutes = 0;
int zone_hours = 0;

void print_time_now();
void update_time();
void update_time_with_check_alarm();
void ring_alarm();
int wait_for_button_press();
void go_to_menu();
void run_mode(int mode);
void set_time();
void set_alarm(int alarm);
void check_temp();
bool is_pressed(int button);

#endif