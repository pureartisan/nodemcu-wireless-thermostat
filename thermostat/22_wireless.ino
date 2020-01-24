// arduinojson.org/assistant to compute the capacity
#define MAX_JSON_OBJECT_SIZE 256

#define MAX_MQTT_SERVER_LENGTH 40
#define MAX_MQTT_PORT_LENGTH 6

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

  void (*saveConfigCallback)(void);

public:

  Wireless(char const *accessPointName, char const *accessPointPassword, int drd_timeout, int drd_address, void (*saveConfigCallback)(void))
    : Threaded()
  {
    this->apName = accessPointName;
    this->apPassword = accessPointPassword;
    
    this->saveConfigCallback = saveConfigCallback;

    this->drd = new DoubleResetDetector(drd_timeout, drd_address);

    this->shouldSaveConfig = false;
  }
 
  virtual void setup()
  {

    this->waitForFileSystem();
    this->loadConfigFromFileSystem();
    
    WiFiManager wifiManager;

    // listen to the user updating config files
    wifiManager.setSaveConfigCallback(this->saveConfigCallback);

    // setup custom params to capture
    WiFiManagerParameter custom_mqtt_server("mqtt_server", "MQTT Server", this->config.mqtt_server, MAX_MQTT_SERVER_LENGTH);
    WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", this->config.mqtt_port, MAX_MQTT_PORT_LENGTH);
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);

    if (this->drd->detectDoubleReset()) {
      Serial.println("Starting Access Point - Config Portal");
      wifiManager.startConfigPortal(this->apName, this->apPassword);
    } else {
      wifiManager.autoConnect(this->apName, this->apPassword);
    }

    if (this->shouldSaveConfig) {
      // copy over the details entered by user
      strcpy(this->config.mqtt_server, custom_mqtt_server.getValue());
      strcpy(this->config.mqtt_port, custom_mqtt_port.getValue());
      
      this->saveConfigToFileSystem();
    }
    
  }

  virtual void loop()
  {
    Threaded::loop();
    this->drd->loop();
  }
  
  void configUpdated()
  {
    this->shouldSaveConfig = true;
  }

private:

  void waitForFileSystem()
  {
    const int chipSelect = 4;
    while (!SD.begin(chipSelect)) {
      Serial.println(F("Failed to initialize SD library"));
      delay(1000);
    }
  }

  void debugPrintWifiInfo()
  {
    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());
  }

  boolean loadConfigFromFileSystem()
  {

    Serial.println("reading config file...");
    
    // Open file for reading
    File file = SD.open(CONFIG_FILE);
  
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
    SD.remove(CONFIG_FILE);
  
    // Open file for writing
    File file = SD.open(CONFIG_FILE, FILE_WRITE);
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
