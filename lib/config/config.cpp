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

bool alarm_enabled[] = {true, true};
int n_alarms = 2;
int alarm_hours[] = {0, 0};
int alarm_minutes[] = {0, 0};
bool alarm_triggered[] = {false, false};

DHTesp dhtSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup_all() // Setting up Pins and Sensors
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(BUTTON_UP, INPUT);
    pinMode(BUTTON_DOWN, INPUT);
    pinMode(BUTTON_BACK, INPUT);
    pinMode(BUTTON_DOWN, INPUT);
    ledcSetup(0, 5000, 8);
    ledcAttachPin(BUZZER, 0);

    dhtSensor.setup(DHTPIN, DHTesp::DHT22);
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

void setup_display() // setup oled display
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

void print_line(String text, int text_size, int row, int column) // Function to write messages to display
{
    display.setTextSize(text_size); // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(column, row);
    display.println(text);
    display.display();
}

void update_time() // Function to Update time variables using RTC
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