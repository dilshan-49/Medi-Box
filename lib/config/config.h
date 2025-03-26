#ifndef CONFIG_H
#define CONFIG_H
// Including the libraries
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_SSD1306.h> // OLED display library
#include <DHTesp.h>
#include <WiFi.h>
#include <time.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Button pins
#define BUTTON_UP 40
#define BUTTON_DOWN 39
#define BUTTON_SELECT 42
#define BUTTON_BACK 41

// Sensor and other Pins
#define DHTPIN 15
#define BUZZER 37
#define LED_1 35
#define LED_2 36

// Other Contants
#define NTP_SERVER "pool.ntp.org"

#define UTC_OFFSET_DST 0

// Ringing tone notes
extern int n_notes;
extern int C;
extern int D;
extern int E;
extern int F;
extern int G;
extern int A;
extern int B;
extern int C_H;
extern int notes[];

// time Variables
extern int UTC_OFFSET;

extern int days;
extern int hours;
extern int minutes;
extern int seconds;
extern String weekday;

// alarm Variables
extern bool alarm_enabled[];
extern int n_alarms;
extern int alarm_hours[];
extern int alarm_minutes[];
extern bool alarm_triggered[];

// Other Peripherals
extern DHTesp dhtSensor;
extern Adafruit_SSD1306 display;

// Main Functions
void setup_all();
void setup_display();
void print_line(String text, int text_size, int row, int column);
void update_time();

#endif