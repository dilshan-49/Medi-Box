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
#define BUTTON_UP 35
#define BUTTON_DOWN 32
#define BUTTON_SELECT 33
#define BUTTON_BACK 34

// Sensor and other Pins
#define DHTPIN 2
#define BUZZER 18
#define LED_1 15
#define LED_2 16

// Other Contants
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

// Variables
int n_notes = 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

bool alarm_enabled = false;
int n_alarms = 2;
int alarm_hours[] = {0, 0};
int alarm_minutes[] = {1, 10};
bool alarm_triggered[] = {false, false};

unsigned long timeNow = 0;
unsigned long timeLast = 0;

int current_mode = 0;
int max_modes = 4;
String options[] = {"1 - Set Time", "2 - Set Alarm", "3 - Set Alarm 2", "4 - Remove Alarm"};

DHTesp dhtSensor;
Adafruit_SSD1306 display;

// basic Functions
void setup_all();
void setup_display();
void print_line(String text, int text_size, int row, int column);

#endif // CONFIG_H