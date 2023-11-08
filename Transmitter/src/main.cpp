#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <WiiChuck.h>
#include <Defines.h>

// setup radio and nunchuck objects
RH_RF95 rf95(RFM95_CS, RFM95_INT);
Accessory nunchuck1;
int CONTROLLER = 0;

// handle data going to reciever in struct
struct dataStruct{
  float drive;
  float steer;
  float mode;
} txStruct;

// declare joystick variables
float joyX, joyY;
float JOYDEADZONE;
int modeSWout;

/*
Function joyFilter()
  Input: Float value to be filtered
  Output: Filtered value in float type

  Function takes a value and makes sure it's between -1 and 1,
  as well as applying deadzone to cancel out near zero inputs.

*/
float joyFilter(float val) {
  float newval;
  newval = val;
  if(val < JOYDEADZONE && val > -1*JOYDEADZONE) {
    newval = 0.0;
  }
  if(val > 1) {
    newval = 1.0;
  }
  if(val < -1) {
    newval = -1.0;
  }
  return newval;
}

void setup() {
  //LoRa radio pin setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);

  //Controller code, if controller is a certain type, set it up properly.
  if (CONTROLLER == 0) {
    pinMode(BLINKER, OUTPUT);
    pinMode(joyXpin, INPUT);
    pinMode(joyYpin, INPUT);
    pinMode(modeSW, INPUT_PULLUP);
  }
  else if (CONTROLLER == 1) {
    nunchuck1.begin();
    if (nunchuck1.type == Unknown) {
      nunchuck1.type = NUNCHUCK;
    }
  }

  //forces hard reset of radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //lora radio init and setting frequency
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  txStruct.drive = 0.0;
  txStruct.steer = 0.0;
  txStruct.mode = 0;
  joyX = 0.0;
  joyY = 0.0;

  JOYDEADZONE = 0.05;

  //setting TxPower
  rf95.setTxPower(23, false);
}

void loop() {
  // switch case that separates controller modes
  // if 0, it uses the analog pin inputs
  // if 1, it uses the nunchuck on i2c 
  switch (CONTROLLER) {
    case 0:
      // read analog values from joystick and convert them 
      // from analog to float from -1 to 1
      joyX = (analogRead(joyXpin)-500.0)/500.0;
      joyY = -1*(analogRead(joyYpin)-500.0)/500.0;
      modeSWout = digitalRead(modeSW);
      // changes drive mode when modeSW is high
      if (modeSWout == 0) {
        txStruct.mode += 1.0;
        if (txStruct.mode > 1) {
          txStruct.mode = 0.0;
        }
        //latching statement that stops code until button is released
        while (modeSWout == 0) {
          modeSWout = digitalRead(modeSW);
        }
      }
      break;
    case 1:
      // read all values from nunchuck on i2c
      nunchuck1.readData();
      //nunchuck1.printInputs(); // debug statment
      // export values from nunchuck and convert from 0 to 255
      // into -1 to 1
      joyX = -1*(nunchuck1.values[1]-126.0)/96;
      joyY = (nunchuck1.values[0]-126.0)/96;
      // check nunchuck z and c buttons
      // 10 is z, 11 is c
      if (nunchuck1.values[10] > 250) {
        txStruct.mode = 0.0;
      }
      else if (nunchuck1.values[11] > 250) {
        txStruct.mode = 1.0;
      }
      break;
  }

  // run gathered values from controller through filter
  joyX = joyFilter(joyX);
  joyY = joyFilter(joyY);
  /*
  Serial.print("X: ");
  Serial.print(joyX);
  Serial.print("    Y: ");
  Serial.println(joyY);
  */

  // transfer final controller values into struct and send through LoRa
  digitalWrite(BLINKER, HIGH);
  txStruct.drive = joyX;
  txStruct.steer = joyY;
  rf95.send((uint8_t *)&txStruct, sizeof(txStruct));
  digitalWrite(BLINKER, LOW);
}
