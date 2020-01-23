class Threaded
{

  int interval;
  unsigned long previousTime;
 
public:

  Threaded(int interval)
  {
    this->interval = interval;
    this->previousTime = 0;
  }

  virtual void setup()
  {
    // do nothing
  }
 
  virtual void loop()
  {
    
    unsigned long currentTime = millis();
    
    if (currentTime - this->previousTime >= this->interval)
    {
      this->previousTime = currentTime;
      this->update();
    }
    
  }

protected:

  virtual void update()
  {
    // do nothing
  }

};
