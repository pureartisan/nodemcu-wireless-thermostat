#include "FS.h"                   // this needs to be first, or it all crashes and burns...

#include "DHT.h"                  // https://github.com/adafruit/DHT-sensor-library
#include <LiquidCrystal_I2C.h>    // https://github.com/johnrickman/LiquidCrystal_I2C
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <DoubleResetDetector.h>  // https://github.com/datacute/DoubleResetDetector
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <PubSubClient.h>         // https://github.com/knolleary/pubsubclient


// add nothing here, but includes
// the other files get compiled in alphabetical order
