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

// Sensor Variables
int ldrValue = 0;
bool isBoxOpen = false;
float currentTemperature = 25.0;
float currentHumidity = 60.0;



DHTesp dhtSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo mediBoxServo;

void setup_all() // Setting up Pins and Sensors
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(BUTTON_UP, INPUT);
    pinMode(BUTTON_DOWN, INPUT);
    pinMode(BUTTON_BACK, INPUT);
    pinMode(BUTTON_SELECT, INPUT);
    pinMode(LDR_PIN, INPUT);

    dhtSensor.setup(DHTPIN, DHTesp::DHT22);
    setup_servo();
    
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

// Servo Functions
void setup_servo()
{
    mediBoxServo.attach(SERVO_PIN);
    mediBoxServo.write(0);
}


// LDR Functions
float readLDR()
{
    const int minRawValue = 32;    // Maximum light intensity (lowest resistance)
    const int maxRawValue = 4063;  // Minimum light intensity (highest resistance)

    int constrainedValue = constrain(analogRead(LDR_PIN), minRawValue, maxRawValue);

    float ldrValue = 1.0 - ((float)(constrainedValue - minRawValue) / (maxRawValue - minRawValue));

    return ldrValue;
}




// Sliding Window Servo Control with Equation
/**
 * Calculates servo angle based on temperature and humidity using an equation
 * The equation maps temperature from TEMPERATURE_MIN to TEMPERATURE_MAX to a position value
 * and adds an additional factor if humidity is above the threshold
 * 
 * @param temperature Current temperature in Celsius
 * @param humidity Current humidity percentage
 * @return Calculated servo angle
 */
int calculateServoAngle(float temperature, float light_intensity) {
    
    float log_time = log((float)LDR_sampletime/(float)LDR_updatetime);

    float angle = theta + (180-theta) * light_intensity*gamma_factor*log_time*temperature/med_temp;

    // Constrain the angle to be within the servo's range
    angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    
    return int(angle);
}


/**
 * Auto-adjust window based on current temperature and humidity readings
 */
void autoAdjustWindow() {
    // Ensure we have updated temperature and humidity values
    float temp = dhtSensor.getTemperature();
    float light = readLDR();
    
    // Check if readings are valid
    if (!isnan(temp) && !isnan(light)) {
        // Calculate and set the appropriate window position
        int angle = calculateServoAngle(temp, light);
    
        // Set servo position
        mediBoxServo.write(angle);
        
        Serial.print("Window adjusted: Temp=");
        Serial.print(temp);
        Serial.print("°C,Light=");
        Serial.print(light);
        Serial.print(", Angle=");
        Serial.print(angle);
        Serial.println("°");
    } else {
        Serial.println("Failed to read temperature or humidity for window adjustment");
    }
}