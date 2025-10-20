float timer_multiplier = 1.0;

// FAILURES = STRIKES - Hunter
int failures = 0;
bool serial_number_vowel = false; // add functionaility later maybe idk i had to add this now...

const int RED = 0;
const int BLUE = 1;
const int GREEN = 2;
const int YELLOW = 3;


// Predefined PIN numbers (please change these to be accurate later) - hunter

const int SIMONBUTTONPINS[] = {13, 12, 11, 10};
const int SIMONLEDPINS[] = {4, 5, 6, 7};

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

  bool complete() override {
    return true;
  }
  
};

Wires wiresModule = Wires();






const int NO_Strikes = 0;
const int ONE_Strikes = 1;
const int TWO_Strikes = 2;



// the idea with these is that the [Strikes][Flashing Color], just amke sure that we are always using the same colors that I defined. - Hunter
const int VOWEL_TABLE[3][4] = {
  {BLUE, RED, YELLOW, GREEN},
  {YELLOW, GREEN, BLUE, RED},
  {GREEN, RED, YELLOW, BLUE}
};

const int NO_VOWEL_TABLE[3][4] = {
  {BLUE, YELLOW, GREEN, RED},
  {RED, BLUE, YELLOW, GREEN},
  {YELLOW, GREEN, BLUE, RED}
};

class SimonSays : public Module {

  private:
    
    int Order[5]; 
    int Current = 0;
    bool is_flashing = false;
    long last_action_time = 0;
    int flash_duration = 500;
    int inter_flash_duration = 250;
    int pattern_index = 0;
    bool is_led_on = false;

    bool button_state[4] = {false, false, false, false};

    void all_off() {
        for(int i = 0; i < 4; i++){
            digitalWrite(SIMONLEDPINS[i], LOW);
        }
        is_led_on = false;
    }

  public:
  
    void setup() override {

      for(int i = 0; i < 5; i++){
        Order[i] = random(0, 4);
      }
      is_flashing = true;
      last_action_time = millis();
      all_off();
    }

    void pattern() {

      long current_time = millis();
      int current_color = Order[pattern_index];

      if(pattern_index < Current + 1) {

        if(is_led_on){
            if(current_time - last_action_time >= flash_duration){
                digitalWrite(SIMONLEDPINS[current_color], LOW);
                is_led_on = false;
                last_action_time = current_time;
            }
        } else {
            if(current_time - last_action_time >= inter_flash_duration){
                
                if(pattern_index < Current){
                    pattern_index++;
                    current_color = Order[pattern_index];
                    digitalWrite(SIMONLEDPINS[current_color], HIGH);
                    is_led_on = true;
                    last_action_time = current_time;
                } else{
                    is_flashing = false;
                    pattern_index = 0;
                    all_off();
                }
            }
        }
      } 
    }

    void check_inputs() {

      for(int i = 0; i < 4; i++){

        bool new_state = digitalRead(SIMONBUTTONPINS[i]) == LOW;

        if(new_state && !button_state[i]){
          
          is_flashing = false;
          pattern_index = 0;
          all_off();

          int correct_color;
          if(serial_number_vowel) {
            correct_color = VOWEL_TABLE[failures][Order[Current]];
          } else{
            correct_color = NO_VOWEL_TABLE[failures][Order[Current]];
          }

          if(i == correct_color){
            Current++;
            Serial.print("Correct Guess has been made");
          }
          else{
            Serial.print("Wrong Guess");
            //failed(); removed for now add this back later
          }
          
        }
        button_state[i] = new_state;
      }
    }

    void process() override{
      check_inputs();

      if(is_flashing){
        pattern();
      } else if (Current < 5) {

        if(millis() - last_action_time > 5000){

          is_flashing = true;
          last_action_time = millis();
          pattern_index = 0;

        }

      }

    }
    
    bool complete() override{
      return Current >= 5;
    }
};


SimonSays simonsaysModule = SimonSays();






void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Initialize serial communication
  randomSeed(analogRead(A0)); // apparently eneded to generate random numbers?

  wiresModule.setup();
  simonsaysModule.setup();
}



void loop() {
  // put your main code here, to run repeatedly:
  // TODO vector of 
  //wiresModule.process();

  // if (wiresModule.complete()) {
  //   Serial.println("you win");
  // }

  if(simonsaysModule.complete()){
    Serial.println("You won Simon Says");
  }
  else{
    simonsaysModule.process();
  }


}


