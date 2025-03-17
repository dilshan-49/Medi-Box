#include <config.h>
#include <baseFunctions.h>
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
}

void loop()
{
  update_time_with_check_alarm();
  if (digitalRead(BUTTON_BACK) == LOW)
  {
    delay(1000);
    Serial.println("Menu");
    go_to_menu();
  }

  check_temp();
}
