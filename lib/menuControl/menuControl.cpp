#include <menuControl.h>

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
        update_time();
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
