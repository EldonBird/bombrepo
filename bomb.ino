float timer_multiplier = 1.0;

int failures = 0;

// explode (instant fail)
void explode() {
  // TODO hardware reset after whatever explosion effect
}

// use this function when someone fails to count down faster or blow up
void failed() {
  failures++;
  if (failures >= 3) explode(); 
  timer_multiplier += 0.5;
}


class Module {

  //public:
  virtual void setup() = 0; 
  virtual void process() = 0;
  virtual bool complete() = 0;
};

class Wires : public Module {

  private:
    bool wires[6];
    int wireToCut;

  public:

  void setup() override {
    Serial.println("setting up wires");
  }

  void process() override {
    for (int i = 0; i < 6; i++) {
      if (wires[i] != analogRead(i)>300) {
        Serial.print(i);
        Serial.println(" changed");
      }
      wires[i] = analogRead(i)>300;
    }
  } 
  
};

Wires wiresModule = Wires();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Initialize serial communication

  wiresModule.setup();
}



void loop() {
  // put your main code here, to run repeatedly:
  // TODO vector of 
  wiresModule.process();

  if (wiresModule.complete()) {
    Serial.println("you win");
  }

}


