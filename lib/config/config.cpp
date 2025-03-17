#include <config.h>

void setup_all()
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(BUTTON_UP, OUTPUT);
    pinMode(BUTTON_DOWN, OUTPUT);
    pinMode(BUTTON_BACK, OUTPUT);
    pinMode(BUTTON_DOWN, OUTPUT);

    dhtSensor.setup(DHTPIN, DHTesp::DHT22);
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
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