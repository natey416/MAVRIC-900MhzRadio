#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <WiiChuck.h>
#include <Defines.h>

RH_RF95 rf95(RFM95_CS, RFM95_INT);
Accessory nunchuck1;
int CONTROLLER = 0;

// handle data going to reciever in struct
struct dataStruct{
  float drive;
  float steer;
  float mode;
} txStruct;

float joyX, joyY;
float JOYDEADZONE;
int modeSWout;

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
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  Serial.print("Hello!");

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

  if (CONTROLLER == 0) {
    joyX = (analogRead(joyXpin)-500.0)/500.0;
    joyY = -1*(analogRead(joyYpin)-500.0)/500.0;
    modeSWout = digitalRead(modeSW);
    if (modeSWout == 0) {
      txStruct.mode += 1.0;
      if (txStruct.mode > 1) {
        txStruct.mode = 0.0;
      }
      while (modeSWout == 0) {
        modeSWout = digitalRead(modeSW);
      }
    }
  }
  else if (CONTROLLER == 1) {
    nunchuck1.readData();
    //nunchuck1.printInputs();
    joyX = -1*(nunchuck1.values[1]-126.0)/96;
    joyY = (nunchuck1.values[0]-126.0)/96;
    if (nunchuck1.values[10] > 250) {
      txStruct.mode = 0.0;
    }
    else if (nunchuck1.values[11] > 250) {
      txStruct.mode = 1.0;
    }
  }


  joyX = joyFilter(joyX);
  joyY = joyFilter(joyY);
  /*
  Serial.print("X: ");
  Serial.print(joyX);
  Serial.print("    Y: ");
  Serial.println(joyY);
  */

  txStruct.drive = joyX;
  txStruct.steer = joyY;
  rf95.send((uint8_t *)&txStruct, sizeof(txStruct));
}
