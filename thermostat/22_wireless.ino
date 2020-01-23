class Wireless: public Threaded
{

  char const *apName;
  char const *apPassword;

  DoubleResetDetector *drd;
  
public:

  Wireless(char const *accessPointName, char const *accessPointPassword, int drd_timeout, int drd_address)
    : Threaded()
  {
    this->apName = accessPointName;
    this->apPassword = accessPointPassword;

    this->drd = new DoubleResetDetector(drd_timeout, drd_address);
  }
 
  virtual void setup()
  {
    
    WiFiManager wifiManager;
      
    if (this->drd->detectDoubleReset()) {
      Serial.println("Starting Access Point - Config Portal");
      wifiManager.startConfigPortal(this->apName, this->apPassword);
    } else {
      wifiManager.autoConnect(this->apName, this->apPassword);
    }
    
  }

  virtual void loop()
  {
    Threaded::loop();
    this->drd->loop();
  }

};
