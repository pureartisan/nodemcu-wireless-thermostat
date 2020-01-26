class LcdControl: public Threaded
{

  LiquidCrystal_I2C *lcd;

  long sleepTimeout;
  uint8_t buttonPin;

  boolean isActive;
  boolean powerSaving;
  boolean firstLoop;
  
public:

  LcdControl(uint8_t buttonPin, long sleepTimeout)
    : Threaded(sleepTimeout)
  {
    this->buttonPin = buttonPin;
    this->sleepTimeout = sleepTimeout;

    this->isActive = false;
    this->powerSaving = false;
    this->firstLoop = true;
    
    this->lcd = new LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
  }
 
  virtual void setup()
  {
    
    Serial.println("LCD Control setup starting...");
      
    this->lcd->init();
    this->activate(true);
    
    // loading screen
    this->writeLine(0, "   Loading...   ");

    pinMode(this->buttonPin, INPUT);
    
    Serial.println("LCD Control setup done!");

  }

  virtual void loop()
  {
    Threaded::loop();

    // check if button is pressed
    int pinStatus = digitalRead(this->buttonPin);

    // since the pin is pull-up, it's HIGH when not pressed
    if (pinStatus == LOW) {
      Serial.println("Display activated");
      this->resetLoopInterval();
      this->activate();
    }
  }

  void activate(boolean shouldClear = false)
  {
    this->lcd->backlight();
    this->isActive = true;
    if (shouldClear) {
      this->lcd->clear();
    }
  }
  
  void deactivate(boolean shouldClear = true)
  {
    this->lcd->noBacklight();
    this->isActive = false;
    if (shouldClear) {
      this->lcd->clear();
    }
  }
  
  void clear()
  {
    this->lcd->clear();
  }
  
  void writeLine(int row, const char* text)
  {    
    this->lcd->setCursor(0, row);
    this->lcd->print(text);
  }

  void setPowerSaving(boolean powerSaving)
  {
    this->powerSaving = powerSaving;
  }

protected:

  virtual void update()
  {

    if (this->powerSaving && this->isActive && !this->firstLoop) {
      Serial.println("Turning off display to save power");
      this->deactivate();
    }

    this->firstLoop = false;
  }

};
