class Threaded
{

  long interval;
  unsigned long previousTime;
 
public:

  Threaded() : Threaded(0) { }

  Threaded(long interval) : previousTime(0)
  {
    this->interval = interval;
  }

  virtual void setup()
  {
    // do nothing
  }
 
  virtual void loop()
  {
    
    unsigned long currentTime = millis();
    
    if (this->interval > 0 && currentTime - this->previousTime >= this->interval)
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
