#include <config.h>

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

int UTC_OFFSET = 0;

int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

bool alarm_enabled = false;
int n_alarms = 2;
int alarm_hours[] = {0, 0};
int alarm_minutes[] = {1, 10};
bool alarm_triggered[] = {false, false};

volatile bool up_pressed = false;
volatile bool down_pressed = false;
volatile bool select_pressed = false;
volatile bool back_pressed = false;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

int current_mode = 0;
int max_modes = 4;
String options[] = {"1 - Set Time Zone", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Remove Alarm"};
DHTesp dhtSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void handle_up()
{
    up_pressed = true;
    Serial.println("UP PRESSED");
}

void handle_down()
{
    down_pressed = true;
    Serial.println("DOWN PRESSED");
}

void handle_select()
{
    select_pressed = true;
    Serial.println("SELECT PRESSED");
}

void handle_back()
{
    back_pressed = true;
    Serial.println("BACK PRESSED");
}

void setup_all()
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(BUTTON_UP, INPUT);
    pinMode(BUTTON_DOWN, INPUT);
    pinMode(BUTTON_BACK, INPUT);
    pinMode(BUTTON_DOWN, INPUT);

    // attachInterrupt(digitalPinToInterrupt(BUTTON_UP), handle_up, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), handle_down, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT), handle_select, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BUTTON_BACK), handle_back, FALLING);

    dhtSensor.setup(DHTPIN, DHTesp::DHT22);
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

void reset_buttons()
{
    up_pressed = false;
    down_pressed = false;
    select_pressed = false;
    back_pressed = false;
}

void setup_display()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    } // Initialize with the I2C addr 0x3D (for the 128x64)
    display.display();
    delay(2000);            // Pause for 2 seconds
    display.clearDisplay(); // Clear the buffer
}

void print_line(String text, int text_size, int row, int column)
{
    display.setTextSize(text_size); // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(column, row);
    display.println(text);
    display.display();
}