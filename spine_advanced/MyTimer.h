

class MyTimer
{
private: 
  unsigned long startTime;
  
public:
  MyTimer() {
    reset();
  }

  void reset() {
    startTime = millis();
  }

  int getDuration() {
    return millis() - startTime;
  }
};

