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
    this->dht->begin();
  }

protected:

  virtual void update()
  {
    
    float h = this->dht->readHumidity();
    float t = this->dht->readTemperature();
  
    if (isnan(h) || isnan(t)) {
      this->lcd->clear();
      this->lcd->setCursor(0, 0);
      this->lcd->print("Devices not connected");
    }
    
    this->lcd->setCursor(0, 0);
    this->lcd->print("Temp. :");
    this->lcd->setCursor(7, 0);
    this->lcd->print(t);
    this->lcd->setCursor(12, 0);
    this->lcd->print(" C");
    
    this->lcd->setCursor(0, 1);
    this->lcd->print("Humi. :");
    this->lcd->setCursor(7, 1);
    this->lcd->print(h);
    this->lcd->setCursor(12, 1);
    this->lcd->print(" %");
    
  }
  
};
