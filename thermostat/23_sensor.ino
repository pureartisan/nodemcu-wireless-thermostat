class Sensor: public Threaded
{

  DHT *dht;
  LcdControl *lcd;
  
  char textBuffer[DISPLAY_TEXT_BUFFER_SIZE];

  WiFiClient espClient;
  PubSubClient *mqttClient;
  Wireless *wireless;

  boolean mqttConnected;
  char mqttBuffer[MQTT_DATA_BUFFER_SIZE];
  long mqttInterval;
  unsigned long previousMqtt;
  
public:

  Sensor(uint8_t pin, uint8_t type, long interval, long mqttInterval, LcdControl *lcd, Wireless *wireless)
    : Threaded(interval)
  {
    this->lcd = lcd;
    this->wireless = wireless;
    this->dht = new DHT(pin, type);

    this->mqttInterval = mqttInterval;
    this->previousMqtt = 0;
    this->mqttConnected = false;
    this->mqttBuffer[0] = EMPTY_STRING_CHAR;

    this->mqttClient = new PubSubClient(espClient);
  }
 
  virtual void setup()
  {
    
    Serial.println("Sensor setup starting...");
    
    this->dht->begin();

    if (this->isReadyToSendData()) {
      this->mqttConnected = this->connectToMqttBroker();
    }
    
    Serial.println("Sensor setup done!");

  }

protected:

  virtual void update()
  {
    
    float h = this->dht->readHumidity();
    float t = this->dht->readTemperature();
  
    if (isnan(h) || isnan(t)) {
      this->lcd->clear();
      this->lcd->writeLine(0, "ERROR: Sensors");
      this->lcd->writeLine(1, "not connected!!");
      return;
    }

    sprintf(this->textBuffer, "Temp. : %.2f C", t);
    this->lcd->writeLine(0, this->textBuffer);

    sprintf(this->textBuffer, "Humi. : %.2f %%", h);
    this->lcd->writeLine(1, this->textBuffer);
    
    // send data    
    if (this->mqttConnected && this->isReadyToSendData()) {
      this->sendDataToMqttBroker(t, h);
    }
    
  }

private:

  boolean isReadyToSendData()
  {
    return this->isValidConfig() && this->wireless->isWifiConnected();
  }

  boolean isValidConfig() {
    Config *config = this->wireless->getConfig();
    return strlen(config->mqtt_server) > 0;
  }

  boolean initialiseMqttServerDetails()
  {

    Serial.println("Initialising MQTT Server details...");

    Config *config = this->wireless->getConfig();

    const char* mqttServer = config->mqtt_server;
    const char* mqttPort = config->mqtt_port;

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

    unsigned long currentTime = millis();
    
    if (this->mqttInterval > 0 && currentTime - this->previousMqtt >= this->mqttInterval)
    {
      
      this->previousMqtt = currentTime;
        
      Serial.println("Sending data via MQTT");
      Serial.print("\nTemp: ");
      Serial.print(this->floatToStr(temp));
      Serial.print("\nHumidity: ");
      Serial.print(this->floatToStr(humidity));
      Serial.println("");
      
      this->mqttClient->publish(MQTT_TOPIC_TEMPERATURE, this->floatToStr(temp));
      this->mqttClient->publish(MQTT_TOPIC_HUMIDITY, this->floatToStr(humidity));

    }
  }

  char* floatToStr(float num)
  {
    sprintf(this->mqttBuffer, "%.2f", num);
    return this->mqttBuffer;
  }

};
