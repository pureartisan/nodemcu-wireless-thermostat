#define MQTT_CLIENT_NAME "ESP8266Client"
#define MQTT_BUFFER_SIZE 10

#define MQTT_TOPIC_TEMPERATURE "sensor/temperature"
#define MQTT_TOPIC_HUMIDITY "sensor/humidity"

class Sensor: public Threaded
{

  DHT *dht;
  LiquidCrystal_I2C *lcd;
  Config *config;

  WiFiClient espClient;
  PubSubClient *mqttClient;

  boolean mqttConnected;
  char mqttBuffer[MQTT_BUFFER_SIZE];
  
public:

  Sensor(uint8_t pin, uint8_t type, long interval, LiquidCrystal_I2C *lcd)
    : Threaded(interval)
  {
    this->lcd = lcd;
    this->dht = new DHT(pin, type);

    this->config = 0;
    this->mqttConnected = false;
    this->mqttBuffer[0] = EMPTY_STRING_CHAR;

    this->mqttClient = new PubSubClient(espClient);
  }
 
  virtual void setup()
  {
    
    Serial.println("Sensor setup starting...");
    
    this->dht->begin();

    if (!this->isReadyToSendData()) {
      this->mqttConnected = this->connectToMqttBroker();
    }
    
    Serial.println("Sensor setup done!");

  }

  void setConfig(Config *config)
  {
    this->config = config; 
  }

protected:

  virtual void update()
  {
    
    float h = this->dht->readHumidity();
    float t = this->dht->readTemperature();
  
    if (isnan(h) || isnan(t)) {
      this->lcd->clear();
      this->lcd->setCursor(0, 0);
      this->lcd->print("Sensors not connected");
      return;
    }
    
    this->lcd->setCursor(0, 0);
    this->lcd->print("Temp. : ");
    this->lcd->setCursor(8, 0);
    this->lcd->print(t);
    this->lcd->setCursor(12, 0);
    this->lcd->print(" C");
    
    this->lcd->setCursor(0, 1);
    this->lcd->print("Humi. : ");
    this->lcd->setCursor(8, 1);
    this->lcd->print(h);
    this->lcd->setCursor(12, 1);
    this->lcd->print(" %");

    // send data
    if (this->mqttConnected && this->isReadyToSendData()) {
      this->sendDataToMqttBroker(t, h);
    }
    
  }

private:

  boolean isReadyToSendData()
  {
    return this->isValidConfig() && this->isWifiConnected();
  }

  boolean isValidConfig()
  {
    return (this->config != 0) && (this->config->mqtt_server[0] != EMPTY_STRING_CHAR);
  }

  boolean isWifiConnected()
  {
    return WiFi.status() != WL_CONNECTED;
  }

  boolean initialiseMqttServerDetails()
  {

    Serial.println("Initialising MQTT Server details...");

    const char* mqttServer = this->config->mqtt_server;
    const char* mqttPort = this->config->mqtt_port;

    int port = (new String(mqttPort))->toInt();
    
    Serial.print("\nUsing MQTT Server: ");
    Serial.print(mqttServer);
    Serial.print("\nUsing MQTT Port: ");
    Serial.print(port);
    Serial.println("");
    
    this->mqttClient->setServer(mqttServer, port);

    return true;
  }

  boolean connectToMqttBroker()
  {

    this->initialiseMqttServerDetails();
    
    while (!this->mqttClient->connected()) {
      
      Serial.println("Connecting to MQTT...");

      // without username/password
      if (this->mqttClient->connect(MQTT_CLIENT_NAME)) {
        Serial.println("connected");  
      } else {
        Serial.print("failed with state ");
        Serial.print(this->mqttClient->state());
        Serial.println("");  
        delay(2000);
      }
      
    }

    return true;

  }

  boolean sendDataToMqttBroker(float temp, float humidity)
  {
    this->mqttClient->publish(MQTT_TOPIC_TEMPERATURE, this->floatToStr(temp));
    this->mqttClient->publish(MQTT_TOPIC_HUMIDITY, this->floatToStr(humidity));
  }

  char* floatToStr(float num)
  {
    sprintf(this->mqttBuffer, "%.2f", num);
    return this->mqttBuffer;
  }

};
