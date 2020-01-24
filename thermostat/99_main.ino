#define SENSOR_PIN D3
#define SENSOR_TYPE DHT22
#define SENSOR_INTERVAL 1000

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define WIRELESS_ACCESS_POINT_NAME "Smart Thermostat"
#define WIRELESS_ACCESS_POINT_PASSWORD NULL // you can set password to connect to device using Wifi
#define WIRELESS_DRD_TIMEOUT 10 // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define WIRELESS_DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use


// callback for when config has been updated
Wireless *wirelessRef;
void saveConfigCallback() {
  wirelessRef->configUpdated();
}

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Sensor sensor(SENSOR_PIN, SENSOR_TYPE, SENSOR_INTERVAL, &lcd);
Wireless wireless(WIRELESS_ACCESS_POINT_NAME, WIRELESS_ACCESS_POINT_PASSWORD, WIRELESS_DRD_TIMEOUT, WIRELESS_DRD_ADDRESS, &saveConfigCallback);

void setup() {

  // init serial
  Serial.begin(9600);
  while (!Serial) continue;

  // update reference
  wirelessRef = &wireless;
  
  // initialize LCD
  lcd.init();                   
  lcd.backlight();
  
  // loading
  lcd.setCursor(0, 0);
  lcd.print("Loading...");

  sensor.setup();
  wireless.setup();
}

void loop() {
  sensor.loop();
  wireless.loop();
}
