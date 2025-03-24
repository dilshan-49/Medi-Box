#include <baseFunctions.h>
#include <config.h>
void update_time()
{

    struct tm timeinfo;
    getLocalTime(&timeinfo);

    char days_str[8];
    char hours_str[8];
    char min_str[8];
    char sec_str[8];

    strftime(days_str, 8, "%d", &timeinfo);
    strftime(sec_str, 8, "%S", &timeinfo);
    strftime(hours_str, 8, "%H", &timeinfo);
    strftime(min_str, 8, "%M", &timeinfo);

    hours = atoi(hours_str);
    minutes = atoi(min_str);
    seconds = atoi(sec_str);
    days = atoi(days_str);
}

void print_time_now()
{
    display.clearDisplay();
    print_line(String(days), 2, 0, 0);
    print_line(":", 2, 0, 20);
    print_line(String(hours), 2, 0, 30);
    print_line(":", 2, 0, 50);
    print_line(String(minutes), 2, 0, 60);
    print_line(":", 2, 0, 80);
    print_line(String(seconds), 2, 0, 90);
}

void update_time_with_check_alarm()
{
    display.clearDisplay();
    update_time();
    print_time_now();

    // check for alarms
    if (alarm_enabled)
    {
        for (int i = 0; i < n_alarms; i++) // iterating through all alarms
        {
            if (alarm_hours[i] == hours && alarm_minutes[i] == minutes && !alarm_triggered[i])
            {
                ring_alarm();
                alarm_triggered[i] = true;
            }
        }
    }
}

void ring_alarm()
{
    display.clearDisplay();
    print_line("Medicine    Time", 2, 0, 0);

    digitalWrite(LED_1, HIGH); // light up LED1

    while (digitalRead(CANCEL) == HIGH)
    {
        for (int i = 0; i < n_notes; i++)
        {
            if (digitalRead(CANCEL) == LOW)
            {
                delay(200);
                break;
            }
            tone(BUZZER, notes[i]);
            delay(500);
            noTone(BUZZER);
            delay(2);
        }
    }
    delay(200);
    digitalWrite(LED_1, LOW); // turn off LED1
}

bool is_pressed(int button)
{
    if (button == BUTTON_UP)
    {
        if (up_pressed)
        {
            up_pressed = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (button == BUTTON_DOWN)
    {
        if (down_pressed)
        {
            down_pressed = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (button == BUTTON_SELECT)
    {
        if (select_pressed)
        {
            select_pressed = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (button == BUTTON_BACK)
    {
        if (back_pressed)
        {
            back_pressed = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

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
            current_mode += 1;
            current_mode = current_mode % max_modes;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            current_mode -= 1;
            current_mode = current_mode < 0 ? max_modes - 1 : current_mode;
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
            break;
        }
    }
}
void run_mode(int mode)
{
    if (mode == 0)
    {
        set_time();
    }

    else if (mode == 1 || mode == 2)
    {
        set_alarm(mode - 1);
    }

    else if (mode == 3)
    {
        alarm_enabled = false;
    }
}

void set_time_zone()
{
    int temp_hour = zone_hours;
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Hour: " + String(temp_hour), 0, 0, 2);

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
            zone_hours = temp_hour;
            delay(200);
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            break;
        }
    }

    int temp_min = zone_minutes;
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Minute: " + String(temp_min), 0, 0, 2);

        int pressed = wait_for_button_press();

        if (pressed == BUTTON_UP)
        {
            temp_min += 30;
            temp_min = temp_min % 60;
            delay(200);
        }
        else if (pressed == BUTTON_DOWN)
        {
            temp_min = temp_min < 30 ? 0 : 30;
            delay(200);
        }
        else if (pressed == BUTTON_SELECT)
        {
            zone_minutes = temp_min;
            delay(200);
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            break;
        }
    }
    UTC_OFFSET = zone_hours * 3600 + zone_minutes * 60;
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
    update_time();
    display.clearDisplay();
    print_line("Time Zone Set", 0, 0, 2);
    delay(1000);
}

void set_alarm(int alarm)
{
    int temp_hour = alarm_hours[alarm];
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Hour: " + String(temp_hour), 0, 0, 2);

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
            break;
        }
    }

    int temp_min = alarm_minutes[alarm];
    while (true)
    {
        display.clearDisplay();
        print_line("Enter Minute: " + String(temp_min), 0, 0, 2);

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
            break;
        }

        else if (pressed == BUTTON_BACK)
        {
            delay(200);
            break;
        }
    }

    display.clearDisplay();
    print_line("Alarm is Set", 0, 0, 2);
    delay(1000);
}

void check_temp()
{
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    bool all_good = true;
    if (data.temperature > 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP HIGH", 1, 40, 0);
    }
    else if (data.temperature < 25)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP LOW", 1, 40, 0);
    }

    if (data.humidity > 85)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD HIGH", 1, 50, 0);
    }
    else if (data.humidity < 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD LOW", 1, 50, 0);
    }
    if (all_good)
    {
        digitalWrite(LED_2, LOW);
    }
}