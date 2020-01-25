#define DEVICE_NAME "Smart Thermostat"

#define MQTT_CLIENT_NAME DEVICE_NAME
#define MQTT_DATA_BUFFER_SIZE 10

#define MAX_MQTT_SERVER_LENGTH 40
#define MAX_MQTT_PORT_LENGTH 6

#define MQTT_TOPIC_TEMPERATURE "sensor/temperature"
#define MQTT_TOPIC_HUMIDITY "sensor/humidity"

// http://arduinojson.org/assistant to compute the capacity
#define MAX_JSON_OBJECT_SIZE 256
#define CONFIG_FILE "/config.json"

#define SENSOR_PIN D3
#define SENSOR_TYPE DHT22
#define SENSOR_INTERVAL 1000
#define SENSOR_MQTT_INTERVAL 5000

#define LCD_CONTROL_PIN D4
#define LCD_SLEEP_TIMEOUT 7000
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define EMPTY_STRING_CHAR '\0'
#define DISPLAY_TEXT_BUFFER_SIZE LCD_COLS

#define WIRELESS_ACCESS_POINT_NAME DEVICE_NAME
#define WIRELESS_ACCESS_POINT_PASSWORD NULL // you can set password to connect to device using Wifi
#define WIRELESS_DRD_TIMEOUT 10 // Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define WIRELESS_DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use
