#include <menuControl.h>
#include <config.h>

// Local variables in library

int zone_minutes = 0;
int zone_hours = 0;

// Functions

int wait_for_button_press()
{
    while (true)
    {
        if (digitalRead(BUTTON_UP) == LOW)
        {
            delay(200);
            return BUTTON_UP;
        }
        else if (digitalRead(BUTTON_DOWN) == LOW)
        {
            delay(200);
            return BUTTON_DOWN;
        }
        else if (digitalRead(BUTTON_SELECT) == LOW)
        {
            delay(200);
            return BUTTON_SELECT;
        }
        else if (digitalRead(BUTTON_BACK) == LOW)
        {
            delay(200);
            return BUTTON_BACK;
        }
    }
}

void go_to_menu()
{
    while (digitalRead(BUTTON_BACK) == HIGH)
    {
        display.clearDisplay();
        print_line(options[current_mode], 2, 0, 0);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            current_mode -= 1;
            current_mode = current_mode < 0 ? max_modes - 1 : current_mode;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            current_mode += 1;
            current_mode = current_mode % max_modes;
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            Serial.println(current_mode);
            delay(200);
            run_mode(current_mode);
        }
        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            display.clearDisplay();
            current_mode = 0;
            break;
        }
    }
}

void run_mode(int mode)
{
    if (mode == 0)
    {
        Serial.println("Set Time Zone");
        set_time_zone();
    }

    else if (mode == 1 || mode == 2)
    {
        Serial.println("Set Alarm");
        set_alarm(mode - 1);
    }

    else if (mode == 3)
    {
        Serial.println("View Alarms");
        view_alarms();
    }
}

void set_time_zone()
{
    int temp_hour = zone_hours;
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Hour: " + String(temp_hour), 2, 0, 2);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            temp_hour += 1;
            if (temp_hour > 12)
                temp_hour = -12; // Wrap around to -12 if it exceeds +12
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            temp_hour -= 1;
            if (temp_hour < -12)
                temp_hour = 12; // Wrap around to +12 if it goes below -12
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            zone_hours = temp_hour;
            delay(200);
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            return;
        }
    }

    int temp_min = zone_minutes;
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Minute: " + String(temp_min), 2, 0, 2);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            temp_min += 1;
            temp_min = temp_min % 60;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            temp_min -= 1;
            temp_min = temp_min < 0 ? 59 : temp_min;
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            zone_minutes = temp_min;
            if (zone_hours < 0)
                zone_minutes = -zone_minutes;
            delay(200);
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            return;
        }
    }
    UTC_OFFSET = zone_hours * 3600 + zone_minutes * 60;
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
    //  update_time();  /////////////////////////////////////////////////////////////////////////////////////
    display.clearDisplay();
    print_line("Time Zone Set", 2, 0, 2);
    delay(1000);
}

void set_alarm(int alarm)
{
    int temp_hour = alarm_hours[alarm];
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Hour: " + String(temp_hour), 2, 0, 2);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            temp_hour += 1;
            temp_hour = temp_hour % 24;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            temp_hour -= 1;
            temp_hour = temp_hour < 0 ? 23 : temp_hour;
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            alarm_hours[alarm] = temp_hour;
            delay(200);
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            return;
        }
    }

    int temp_min = alarm_minutes[alarm] % 60;
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Minute: " + String(temp_min), 2, 0, 2);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            temp_min += 1;
            temp_min = temp_min % 60;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            temp_min -= 1;
            temp_min = temp_min < 0 ? 59 : temp_min;
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            alarm_minutes[alarm] = temp_min;
            delay(200);
            alarm_enabled[alarm] = true;
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            break;
        }
    }

    display.clearDisplay();
    print_line("Alarm is Set", 2, 0, 2);
    delay(1000);
}

void view_alarms()
{
    int i = 0;
    display.clearDisplay();
    while (digitalRead(BUTTON_BACK) == HIGH)
    {
        display.clearDisplay();

        if (alarm_enabled[i])
        {
            print_line("Alarm " + String(i + 1), 2, 0, 10);
            print_line(String(alarm_hours[i]) + ":" + String(alarm_minutes[i]), 2, 20, 2);
            print_line("Press Select to Turn off", 1, 40, 0);
        }
        else
        {
            print_line("Alarm " + String(i + 1) + ": OFF", 2, 0, 2);
            print_line("Press Select to Turn on", 1, 40, 0);
        }

        int pressed = wait_for_button_press();
        if (pressed == BUTTON_BACK)
        {
            delay(200);
            return;
        }
        else if (pressed == BUTTON_SELECT)
        {
            alarm_enabled[i] = !alarm_enabled[i];
            delay(200);
        }
        else if (pressed == BUTTON_UP)
        {
            i -= 1;
            i = i < 0 ? n_alarms - 1 : i;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            i += 1;
            i = i % n_alarms;
            delay(200);
        }
    }
}