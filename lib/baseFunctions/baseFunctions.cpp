#include <baseFunctions.h>

// local Variables

String weekday;
int snooze_duration = 2;
bool snoozed = false;
int snoozed_alarm[2] = {0, 0};

void print_time_now() // Updating time in the home Screen
{
    display.clearDisplay();
    char formatted_days[3];
    char formatted_hours[3];
    char formatted_minutes[3];
    char formatted_seconds[3];

    // Formatting time
    sprintf(formatted_days, "%02d", days);
    sprintf(formatted_hours, "%02d", hours);
    sprintf(formatted_minutes, "%02d", minutes);
    sprintf(formatted_seconds, "%02d", seconds);

    print_line(String(formatted_days), 2, 0, 30);
    print_line(weekday, 2, 0, 70);
    print_line(String(formatted_hours), 2, 20, 20);
    print_line(":", 2, 20, 40);
    print_line(String(formatted_minutes), 2, 20, 50);
    print_line(":", 2, 20, 70);
    print_line(String(formatted_seconds), 2, 20, 80);
}

void check_alarm()
{

    for (int i = 0; i < n_alarms; i++) // checking all alarms
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
    for (int i = 0; i < 3; i++)
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
            noTone(BUZZER);
            tone(BUZZER, notes[i]);
            delay(500);
            noTone(BUZZER);
            delay(2);
        }
    }
    if (alarm != -1) // Not an already Snoozed Alarm
    {
        snooze_alarm(alarm);
        snoozed = true;
        Serial.println("Alarm Snoozed");
    }
    delay(200);
    digitalWrite(LED_1, LOW); // turn off LED1
}

void snooze_alarm(int alarm) // Snoozing the alarms which missed user confirmation
{
    int snooze_minute = (alarm_minutes[alarm] + snooze_duration);
    int snooze_hour = alarm_hours[alarm];
    if (snooze_minute >= 60)
    {
        snooze_minute = snooze_minute % 60;
        snooze_hour += 1;
        snooze_hour = snooze_hour % 24;
    }
    snoozed_alarm[0] = snooze_hour;
    snoozed_alarm[1] = snooze_minute;
    Serial.print("Snooze Time Set");
    Serial.print(snoozed_alarm[0]);
    Serial.print(":");
    Serial.println(snoozed_alarm[1]);
}

void check_temp() // Checking Temperature and Humidity
{
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    bool all_good = true;

    if (data.temperature > 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP HIGH", 1, 55, 0);
    }
    else if (data.temperature < 25)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("TEMP LOW", 1, 55, 0);
    }

    if (data.humidity > 85)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD HIGH", 1, 55, 70);
    }
    else if (data.humidity < 35)
    {
        all_good = false;
        digitalWrite(LED_2, HIGH);
        print_line("HUMD LOW", 1, 55, 70);
    }
    if (all_good)
    {
        digitalWrite(LED_2, LOW);
    }
}