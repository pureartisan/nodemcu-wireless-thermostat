// callback for when config has been updated
Wireless *wirelessRef;
void saveConfigCallback() {
  wirelessRef->configUpdated();
}

LcdControl lcd(LCD_CONTROL_PIN, LCD_SLEEP_TIMEOUT);
Wireless wireless(WIRELESS_ACCESS_POINT_NAME, WIRELESS_ACCESS_POINT_PASSWORD, WIRELESS_DRD_TIMEOUT, WIRELESS_DRD_ADDRESS, &lcd, &saveConfigCallback);
Sensor sensor(SENSOR_PIN, SENSOR_TYPE, SENSOR_INTERVAL, SENSOR_MQTT_INTERVAL, &lcd, &wireless);

void setup() {

  // init serial
  Serial.begin(9600);
  while (!Serial) continue;
  
  // show splash in serial
  Serial.println("\n-----------");
  Serial.println(DEVICE_NAME);
  Serial.println("-----------");

  // update reference
  wirelessRef = &wireless;
  
  lcd.setup();
  wireless.setup();
  sensor.setup();

  // enable power saving
  lcd.setPowerSaving(true);
  
}

void loop() {
  lcd.loop();
  wireless.loop();
  sensor.loop();
}
