#include <config.h>
#include <timeFunctions.h>
#include <menuControl.h>

#include <WiFi.h>

void setup()
{
  setup_all();

  setup_display();

  print_line("Welcome to Medibox", 2, 0, 0);
  delay(3000);

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi..", 2, 0, 0);
  }
  display.clearDisplay();
  print_line("Connected to WiFi", 2, 0, 0);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  update_time();
  display.clearDisplay();
  delay(500);
}

void loop()
{
  update_time();
  print_time_now();
  check_alarm();
  if (alarm_enabled[0])
    print_line("Alm 1:On", 1, 40, 2);
  else
    print_line("Alm 1:Off", 1, 40, 2);

  if (alarm_enabled[1])
    print_line("Alm 2:On", 1, 40, 65);
  else
    print_line("Alm 2:Off", 1, 40, 65);

  if (digitalRead(BUTTON_BACK) == LOW)
  {
    delay(1000);

    Serial.println("Menu");
    go_to_menu();
  }

  check_temp();
}
