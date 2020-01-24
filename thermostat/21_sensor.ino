class Sensor: public Threaded
{

  DHT *dht;
  LiquidCrystal_I2C *lcd;
  
public:

  Sensor(uint8_t pin, uint8_t type, long interval, LiquidCrystal_I2C *lcd)
    : Threaded(interval)
  {
    this->lcd = lcd;
    this->dht = new DHT(pin, type);
  }
 
  virtual void setup()
  {
    
    Serial.println("Sensor setup starting...");
    
    this->dht->begin();
    
    Serial.println("Sensor setup done!");

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
    
  }
  
};
