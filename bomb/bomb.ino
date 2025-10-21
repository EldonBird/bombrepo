#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


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
const int CapacitorButtonPin = 3;


byte fullBlock[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};


// explode (instant fail)
void explode() {
  // TODO hardware reset after whatever explosion effect
  Serial.println(" EXPLODED!!!!!!!!!!!!!!!!!!!");
}

// use this function when someone fails to count down faster or blow up
void failed() {
  Serial.print("failed " + String(failures));
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




class Capacitor : public Module {


  private:
    const int totalBlocks = 16;
    int charge_level = 0;
    unsigned long lastChargeTime = 0;
    unsigned long lastDischargeTime = 0;

    const long chargeInterval = 1000; // 1 second per block (Charging Speed)
    const long dischargeInterval = 500; // 0.5 seconds per block (Discharge Speed)

    byte fullBlock[8] = {
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111
    };

    void updateDisplay() {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      for (int i = 0; i < charge_level; i++) {
        lcd.setCursor(i, 0);
        lcd.write((byte)0);

        lcd.setCursor(i, 1);
        lcd.write((byte)0);
      }
    }
      
  public:
    void setup() override {



    }

    void process() override {
        bool buttonState = digitalRead(CapacitorButtonPin) == LOW; 
        unsigned long currentTime = millis();

        if(charge_level >= 16){
            explode();
        }

        // discharging
        if (buttonState) {
            if (currentTime - lastDischargeTime >= dischargeInterval) {
                lastDischargeTime = currentTime;
                if (charge_level > 0) {
                    charge_level--; 
                    updateDisplay();
                }
            }
        } 
        // charging
        else {
            if (currentTime - lastChargeTime >= chargeInterval) {
                lastChargeTime = currentTime;
                if (charge_level < totalBlocks) {
                    charge_level++;
                    updateDisplay();
                }
            }
        }
    }
    
    bool complete() override {

    }

};
Capacitor capacitorModule = Capacitor();



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
    int input_index = 0;
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

    void start_pattern_flash() {
        is_flashing = true;
        pattern_index = 0;
        last_action_time = millis();
        if (Current < 5) {
            digitalWrite(SIMONLEDPINS[Order[pattern_index]], HIGH);
            is_led_on = true;
        }
    }

    bool user_input_active() {
      for(int i = 0; i < 4; i++){
        if(button_state[i] == true) return true;
      }
      return false;
    }


  public:
  
    void setup() override {

    for (int i = 0; i < 4; i++) {
    pinMode(SIMONLEDPINS[i], OUTPUT);
    pinMode(SIMONBUTTONPINS[i], INPUT_PULLUP); // Use INPUT_PULLUP
    } 

    for(int i = 0; i < 5; i++){
    Order[i] = random(0, 4);
    }
    Current = 0; 
    start_pattern_flash();
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

        if(new_state == true && button_state[i] == true){
          return;
        }
        else if(new_state == false && button_state[i] == false){
          return;
        }
        else if(new_state == false && button_state[i] == true){
          button_state[i] = false;
        }
        else{

          all_off();
          is_flashing = false;

          int pattern_color_index = Order[input_index];
          int correct_button_color;

          if(serial_number_vowel){
            correct_button_color = VOWEL_TABLE[failures][pattern_color_index];
          } else{
            correct_button_color = NO_VOWEL_TABLE[failures][pattern_color_index];
          }

          Serial.print("Expected color (index): ");
          Serial.println(correct_button_color);

          if(i == correct_button_color){
            input_index++;
            last_action_time = millis();
            button_state[i] = new_state; // <-- **ADD THIS LINE**

            if(input_index > Current){
              Current++;
              input_index = 0;
               if(Current < 5) {
                  Serial.println("--- Sequence Completed. New length: " + String(Current + 1));
                  all_off();
                  start_pattern_flash();
               }
            }
          } else{

            Serial.println("Wrong Guess at input index: " + String(input_index));
            failed();
            input_index = 0;
            all_off();
            start_pattern_flash();
            button_state[i] = new_state; // <-- **ADD THIS LINE**

          }
        }

        // button_state[i] = new_state; // <-- **REMOVE THIS LINE**

      }


    }

    void process() override{
      check_inputs();

      if(is_flashing && !user_input_active()){
        pattern();
      } else if (Current < 5) {

        if(millis() - last_action_time > 5000){
          start_pattern_flash();
        }

      }

    }
    
    bool complete() override{
      return Current >= 5;
    }
};


SimonSays simonsaysModule = SimonSays();








void setup() {


  // setup for capacitor

  lcd.init();
  lcd.backlight();
  pinMode(CapacitorButtonPin, INPUT_PULLUP);
  lcd.createChar(0, fullBlock);
  lcd.setCursor(0, 0);

  // put your setup code here, to run once:
  Serial.begin(9600); // Initialize serial communication
  randomSeed(analogRead(A0)); // apparently eneded to generate random numbers?

  wiresModule.setup();
  simonsaysModule.setup();
  capacitorModule.setup();
}



void loop() {

  if (failures >= 3) return;




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
  //capacitorModule.process();

}


