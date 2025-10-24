#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


float timer_multiplier = 1.0;

// FAILURES = STRIKES - Hunter
int failures = 0;
bool serial_number_vowel = false; // add functionaility later maybe idk i had to add this now... [cite: 2]

const int RED = 0;
const int BLUE = 1;
const int GREEN = 2;
const int YELLOW = 3;
// Predefined PIN numbers (please change these to be accurate later) - hunter [cite: 4]

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
const int MEMORYBUTTONPINS[] = {22, 23, 24, 25}; 

const int MEMORYLEDPINS[] = {26, 27, 28, 29};
const int SEG_A_PIN = 30;
const int SEG_B_PIN = 31;
const int SEG_C_PIN = 32;
const int SEG_D_PIN = 33;
const int SEG_E_PIN = 34;
const int SEG_F_PIN = 35;
const int SEG_G_PIN = 36;
const int SEG_DP_PIN = 37;
const int DIG_1_PIN = 38;
const int DIG_2_PIN = 39;
const int DIG_3_PIN = 40;
const int DIG_4_PIN = 41;
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

    const long chargeInterval = 1000;
// 1 second per block (Charging Speed)
    const long dischargeInterval = 500;
// 0.5 seconds per block (Discharge Speed)

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



// the idea with these is that the [Strikes][Flashing Color], just amke sure that we are always using the same colors that I defined. [cite: 35]
// - Hunter
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

    if(new_state && !button_state[i]){ // Button pressed: state changed from HIGH (unpressed) to LOW (pressed)

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
        button_state[i] = new_state; 

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
        button_state[i] = new_state; 

      }
    } else if (!new_state && button_state[i]) { // Button released: state changed from LOW (pressed) to HIGH (unpressed)
        button_state[i] = false;
    }
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

// ------------------------------------------------------------------
// NEW 7-SEGMENT DISPLAY LOGIC FOR 5161AS (COMMON ANODE)
// ------------------------------------------------------------------

// Segment patterns for Common Anode display (LOW = ON)
void write_number_to_7seg(int number) {
  // Use HIGH for OFF (Common Anode) and LOW for ON
  int seg_values[7]; // A, B, C, D, E, F, G

  switch (number) {
    case 1: // '1'
      // A B C D E F G
      seg_values[0] = LOW;
      seg_values[1] = HIGH;
      seg_values[2] = HIGH;
      seg_values[3] = LOW;
      seg_values[4] = LOW;
      seg_values[5] = LOW;
      seg_values[6] = LOW;
      break;
    case 2: // '2'
      seg_values[0] = HIGH;
      seg_values[1] = HIGH;
      seg_values[2] = LOW;
      seg_values[3] = HIGH;
      seg_values[4] = HIGH;
      seg_values[5] = LOW;
      seg_values[6] = HIGH;
      break;
    case 3: // '3'
      seg_values[0] = HIGH;
      seg_values[1] = HIGH;
      seg_values[2] = HIGH;
      seg_values[3] = HIGH;
      seg_values[4] = LOW;
      seg_values[5] = LOW;
      seg_values[6] = HIGH;
      break;
    case 4: // '4'
      seg_values[0] = LOW;
      seg_values[1] = HIGH;
      seg_values[2] = HIGH;
      seg_values[3] = LOW;
      seg_values[4] = LOW;
      seg_values[5] = HIGH;
      seg_values[6] = HIGH;
      break;
    default: // Turn everything off if number is not 1-4
      for(int i = 0; i < 7; i++) seg_values[i] = HIGH;
      break;
  }

  digitalWrite(SEG_A_PIN, seg_values[0]);
  digitalWrite(SEG_B_PIN, seg_values[1]);
  digitalWrite(SEG_C_PIN, seg_values[2]);
  digitalWrite(SEG_D_PIN, seg_values[3]);
  digitalWrite(SEG_E_PIN, seg_values[4]);
  digitalWrite(SEG_F_PIN, seg_values[5]);
  digitalWrite(SEG_G_PIN, seg_values[6]);
  digitalWrite(SEG_DP_PIN, HIGH); // Always off
}

void activate_7seg_digit() {
  pinMode(SEG_A_PIN, OUTPUT);
  pinMode(SEG_B_PIN, OUTPUT);
  pinMode(SEG_C_PIN, OUTPUT);
  pinMode(SEG_D_PIN, OUTPUT);
  pinMode(SEG_E_PIN, OUTPUT);
  pinMode(SEG_F_PIN, OUTPUT);
  pinMode(SEG_G_PIN, OUTPUT);
  pinMode(SEG_DP_PIN, OUTPUT);
}

// ------------------------------------------------------------------
// END 7-SEGMENT DISPLAY LOGIC
// ------------------------------------------------------------------


class Memory : public Module {
private:
  
  int current_stage = 1;
  int display_number = 0;
  int button_label[4] = {1, 2, 3, 4};
  int history_position[5] = {0, 0, 0, 0, 0};
  int history_label[5] = {0, 0, 0, 0, 0};

  bool button_state[4] = {false, false, false, false};
  long last_input_time = 0;
  const long debounce_delay = 50;

  bool currently_pressed;

  void generate_stage() {
    randomSeed(analogRead(A0) + millis());
    display_number = random(1, 5);
    for (int i = 0; i < 4; i++) button_label[i] = i + 1;
    for (int i = 0; i < 4; i++) {
      int swap_index = random(i, 4);
      int temp = button_label[i];
      button_label[i] = button_label[swap_index];
      button_label[swap_index] = temp;
    }
  }

  int get_correct_position(int stage) {
    if (stage == 1) {
      if (display_number == 1 || display_number == 2) return 2;
      if (display_number == 3) return 3;
      if (display_number == 4) return 4;
    } else if (stage == 2) {
      if (display_number == 1) return 4;
      if (display_number == 2 || display_number == 4) return history_position[1];
      if (display_number == 3) return 1;
    } else if (stage == 3) {
      if (display_number == 1) return history_label[2];
      if (display_number == 2) return history_label[1];
      if (display_number == 3) return 3;
      if (display_number == 4) return 4;
    } else if (stage == 4) {
      if (display_number == 1) return history_position[1];
      if (display_number == 2) return 1;
      if (display_number == 3 || display_number == 4) return history_position[2];
    } else if (stage == 5) {
      if (display_number == 1) return history_label[1];
      if (display_number == 2) return history_label[2];
      if (display_number == 3) return history_label[4];
      if (display_number == 4) return history_label[3];
    }
    return 0; // Should not happen
  }

  void update_display() {
    // Actual 7-Segment Display update
    write_number_to_7seg(display_number); // <--- CHANGE HERE

    // Debug output (KEEP THIS) 
    Serial.print("MEMORY: Stage ");
    Serial.print(current_stage);
    Serial.print(", Display: ");
    Serial.print(display_number);
    Serial.print(", Labels: ");
    for(int i=0; i<4; i++) {
      Serial.print(button_label[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  void handle_correct_guess(int pos, int label) {
    history_position[current_stage] = pos;
    history_label[current_stage] = label;
    current_stage++;
    if (current_stage <= 5) {
      generate_stage();
      update_display();
      Serial.println("Correct! Moving to next stage.");
    } else {
      Serial.println("Memory Module Complete!");
    }
  }

  void handle_incorrect_guess() {
    Serial.println("Incorrect guess!");
    failed();

    if (failures >= 3) {
      return; 
    }
    
    current_stage = 1; 
    for(int i = 1; i <= 5; i++) {
      history_position[i] = 0;
      history_label[i] = 0;
    }
    generate_stage();
    update_display();
  }

public:
  void setup() override {
    for (int i = 0; i < 4; i++) {
      pinMode(MEMORYBUTTONPINS[i], INPUT_PULLUP);
      pinMode(MEMORYLEDPINS[i], OUTPUT);
      digitalWrite(MEMORYLEDPINS[i], LOW);
    }
    generate_stage();
    update_display();
    Serial.println("Memory Module setup complete.");
  }

  void process() override {
  if (current_stage > 5) return;
  if (millis() - last_input_time < debounce_delay) return;

  for (int i = 0; i < 4; i++) {
    bool new_state = digitalRead(MEMORYBUTTONPINS[i]) == LOW;
    if (new_state && !button_state[i]) {
      button_state[i] = true;
      last_input_time = millis();

      int correct_pos = get_correct_position(current_stage);

      if (i + 1 == correct_pos) {
        handle_correct_guess(i + 1, button_label[i]);
      } else {
        handle_incorrect_guess();
        // Add this line to enforce debounce after a failure
        last_input_time = millis(); 
        // button_state[i] remains true, forcing user release
        return;
      }
      break;
    } else if (!new_state && button_state[i]) {
      button_state[i] = false;
      last_input_time = millis();
    }
  }
}

  bool complete() override {
    return current_stage > 5;
  }
  
  // Public accessor for display_number to allow loop() to keep the final number on
  int get_display_number() {
      return display_number;
  }
};

Memory memoryModule = Memory(); // Create the instance


void setup() {


  // setup for capacitor

  lcd.init();
  lcd.backlight();
  pinMode(CapacitorButtonPin, INPUT_PULLUP);
  lcd.createChar(0, fullBlock);
  lcd.setCursor(0, 0);

  // setup 7-Segment pins
  activate_7seg_digit(); // <--- ADDED

  // put your setup code here, to run once:
  Serial.begin(9600);
  // Initialize serial communication [cite: 110]
  randomSeed(analogRead(A0)); // apparently eneded to generate random numbers?

  //wiresModule.setup();
  //simonsaysModule.setup();
  //capacitorModule.setup();
  memoryModule.setup();
}



void loop() {

  if (failures >= 3) return;




  // put your main code here, to run repeatedly:
  // TODO vector of 
  //wiresModule.process();
  // if (wiresModule.complete()) {
  //   Serial.println("you win");
  // }
  

  // if(simonsaysModule.complete()){
  //   Serial.println("You won Simon Says");
  // }
  // else{ 
  //   simonsaysModule.process();
  // }
  
  if(memoryModule.complete()){
    write_number_to_7seg(memoryModule.get_display_number()); // Keep final number displayed
    Serial.println("You won Memory");
  }
  else{ 
    memoryModule.process();
  }
  capacitorModule.process();

}