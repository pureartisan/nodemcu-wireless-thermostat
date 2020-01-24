// arduinojson.org/assistant to compute the capacity
#define MAX_JSON_OBJECT_SIZE 256

#define MAX_MQTT_SERVER_LENGTH 40
#define MAX_MQTT_PORT_LENGTH 6

#define EMPTY_STRING_CHAR '\0'

#define CONFIG_FILE "/config.json"

// Our configuration structure.
//
// Never use a JsonDocument to store the configuration!
// A JsonDocument is *not* a permanent storage; it's only a temporary storage
// used during the serialization phase. See:
// https://arduinojson.org/v6/faq/why-must-i-create-a-separate-config-object/
struct Config {
  char mqtt_server[MAX_MQTT_SERVER_LENGTH];
  char mqtt_port[MAX_MQTT_PORT_LENGTH];
};


class Wireless: public Threaded
{
  
  bool shouldSaveConfig;
  Config config;
  
  const char *apName;
  const char *apPassword;

  DoubleResetDetector *drd;
  LiquidCrystal_I2C *lcd;

  void (*saveConfigCallback)(void);

public:

  Wireless(char const *accessPointName, char const *accessPointPassword, int drd_timeout, int drd_address, LiquidCrystal_I2C *lcd, void (*saveConfigCallback)(void))
    : Threaded()
  {
    this->apName = accessPointName;
    this->apPassword = accessPointPassword;

    this->lcd = lcd;
    this->saveConfigCallback = saveConfigCallback;

    this->drd = new DoubleResetDetector(drd_timeout, drd_address);

    this->shouldSaveConfig = false;

    this->config.mqtt_server[0] = EMPTY_STRING_CHAR;
    this->config.mqtt_port[0] = EMPTY_STRING_CHAR;
  }
 
  virtual void setup()
  {
    
    Serial.println("Wireless setup starting...");

    this->loadConfigFromFileSystem();
    
    WiFiManager wifiManager;

    // listen to the user updating config files
    wifiManager.setSaveConfigCallback(this->saveConfigCallback);

    // setup custom params to capture
    WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", this->config.mqtt_server, MAX_MQTT_SERVER_LENGTH);
    WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", this->config.mqtt_port, MAX_MQTT_PORT_LENGTH);
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);

    // force setup?
    if (this->drd->detectDoubleReset()) {
      Serial.println("Starting setup mode");
      this->displaySetupMode();
      wifiManager.startConfigPortal(this->apName, this->apPassword);
    } else {
      Serial.println("Auto connect");
      wifiManager.autoConnect(this->apName, this->apPassword);
    }

    if (this->shouldSaveConfig) {
      // copy over the details entered by user
      strcpy(this->config.mqtt_server, custom_mqtt_server.getValue());
      strcpy(this->config.mqtt_port, custom_mqtt_port.getValue());
      
      this->saveConfigToFileSystem();
    }
    
    Serial.println("Wireless setup done!");
    
  }

  virtual void loop()
  {
    Threaded::loop();
    this->drd->loop();
  }
  
  void configUpdated()
  {
    Serial.println("Config updated, we need to save it");
    this->shouldSaveConfig = true;
  }

private:

  void displaySetupMode()
  {
      this->lcd->clear();
      this->lcd->setCursor(0, 0);
      this->lcd->print("(Setup Mode)");
  }

  boolean loadConfigFromFileSystem()
  {

    Serial.println("reading config file...");
    
    // Open file for reading
    File file = SPIFFS.open(CONFIG_FILE, "r");
  
    // Allocate a temporary JsonDocument
    StaticJsonDocument<MAX_JSON_OBJECT_SIZE> doc;
  
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println(F("Failed to read file, using default configuration"));
      return false;
    }
  
    // Copy values from the JsonDocument to the Config
    
    strlcpy(this->config.mqtt_server,         // <- destination
            doc["mqtt_server"] | "",          // <- source
            sizeof(config.mqtt_server));      // <- destination's capacity

    strlcpy(this->config.mqtt_port,           // <- destination
            doc["mqtt_port"] | "8080",        // <- source
            sizeof(config.mqtt_port));        // <- destination's capacity
  
    file.close();

    return true;
    
  }

  boolean saveConfigToFileSystem()
  {
    
    Serial.println("saving config");
    
    // Delete existing file, otherwise the configuration is appended to the file
    SPIFFS.remove(CONFIG_FILE);
  
    // Open file for writing
    File file = SPIFFS.open(CONFIG_FILE, "w");
    if (!file) {
      Serial.println(F("Failed to create file"));
      return false;
    }
  
    // Allocate a temporary JsonDocument
    StaticJsonDocument<MAX_JSON_OBJECT_SIZE> doc;
  
    // Set the values in the document
    doc["mqtt_server"] = this->config.mqtt_server;
    doc["mqtt_port"] = this->config.mqtt_port;

    boolean successful = true;
  
    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
      Serial.println(F("Failed to write to file"));
      successful = false;
    }
  
    file.close();

    return successful;
    
  }

};
