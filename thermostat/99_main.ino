#define SENSOR_PIN D3
#define SENSOR_TYPE DHT22
#define SENSOR_INTERVAL 1000

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2



LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Sensor sensor(SENSOR_PIN, SENSOR_TYPE, SENSOR_INTERVAL, &lcd);

void setup() {
//  Serial.begin(9600);
  
  // initialize LCD
  lcd.init();                   
  lcd.backlight();
  
  // loading
  lcd.setCursor(0, 0);
  lcd.print("Loading...");

  sensor.setup();
}

void loop() {
  
  sensor.loop();
  
}
