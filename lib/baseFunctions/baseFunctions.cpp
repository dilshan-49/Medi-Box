#include <baseFunctions.h>

int zone_minutes = 0;
int zone_hours = 0;
String weekday;
bool snoozed = false;
int snoozed_alarm[2] = {0, 0};

void update_time()
{

    struct tm timeinfo;
    getLocalTime(&timeinfo);

    char days_str[8];
    char hours_str[8];
    char min_str[8];
    char sec_str[8];
    char day_of_week[4];

    strftime(days_str, 8, "%d", &timeinfo);
    strftime(sec_str, 8, "%S", &timeinfo);
    strftime(hours_str, 8, "%H", &timeinfo);
    strftime(min_str, 8, "%M", &timeinfo);
    strftime(day_of_week, 4, "%a", &timeinfo);

    hours = atoi(hours_str);
    minutes = atoi(min_str);
    seconds = atoi(sec_str);
    days = atoi(days_str);
    weekday = String(day_of_week);
}

void print_time_now()
{
    display.clearDisplay();

    char formatted_hours[3];
    char formatted_minutes[3];
    char formatted_seconds[3];

    sprintf(formatted_hours, "%02d", hours);
    sprintf(formatted_minutes, "%02d", minutes);
    sprintf(formatted_seconds, "%02d", seconds);
    print_line(weekday, 2, 0, 90);
    print_line(String(days), 2, 0, 50);
    print_line(String(formatted_hours), 2, 20, 0);
    print_line(":", 2, 20, 20);
    print_line(String(formatted_minutes), 2, 20, 30);
    print_line(":", 2, 20, 50);
    print_line(String(formatted_seconds), 2, 20, 60);
}

void update_time_with_check_alarm()
{

    update_time();
    print_time_now();

    // check for alarms

    for (int i = 0; i < n_alarms; i++) // iterating through all alarms
    {
        if (alarm_hours[i] == hours && alarm_minutes[i] == minutes && !alarm_triggered[i] && alarm_enabled[i])
        {
            Serial.println("Alarm Ringing");
            ring_alarm(i);
            alarm_triggered[i] = true;
        }
    }
    if (snoozed_alarm[0] == hours && snoozed_alarm[1] == minutes && snoozed) // Ringing Snoozed Alarm
    {
        Serial.println("Snoozed Alarm Ringing");
        ring_alarm(-1);
        snoozed = false;
    }
    if (hours == 0 && minutes == 0) // Resetting Alarms at the End of the Day
    {
        for (int i = 0; i < n_alarms; i++)
        {
            alarm_triggered[i] = false;
        }
    }
}

void ring_alarm(int alarm)
{
    display.clearDisplay();
    print_line("Medicine    Time", 2, 0, 0);

    digitalWrite(LED_1, HIGH); // light up LED1
    for (int i = 0; i < 10; i++)
    {
        for (int i = 0; i < n_notes; i++)
        {
            if (digitalRead(BUTTON_BACK) == LOW)
            {
                Serial.println("Alarm Stopped");
                delay(200);
                digitalWrite(LED_1, LOW);
                return;
            }
            tone(BUZZER, notes[i]);
            delay(500);
            noTone(BUZZER);
            delay(2);
        }
    }
    if (alarm != -1)
    {
        snooze_alarm(alarm);
        snoozed = true;
        Serial.println("Alarm Snoozed");
    }
    delay(200);
    digitalWrite(LED_1, LOW); // turn off LED1
}

void snooze_alarm(int alarm)
{
    int snooze_minute = (alarm_minutes[alarm] + 5);
    int snooze_hour = alarm_hours[alarm];
    if (snooze_minute >= 60)
    {
        snooze_minute = snooze_minute % 60;
        snooze_hour += 1;
        snooze_hour = snooze_hour % 24;
    }
    snoozed_alarm[0] = snooze_hour;
    snoozed_alarm[1] = snooze_minute;
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
    update_time();
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
void check_temp()
{
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    bool all_good = true;

    if (data.temperature > 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP HIGH", 1, 50, 0);
    }
    else if (data.temperature < 25)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP LOW", 1, 50, 0);
    }

    if (data.humidity > 85)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD HIGH", 1, 50, 70);
    }
    else if (data.humidity < 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD LOW", 1, 50, 70);
    }
    if (all_good)
    {
        digitalWrite(LED_2, LOW);
    }
}