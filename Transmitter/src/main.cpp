#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

#include <Defines.h>

RH_RF95 rf95(RFM95_CS, RFM95_INT);

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
  pinMode(BLINKER, OUTPUT);
  pinMode(joyXpin, INPUT);
  pinMode(joyYpin, INPUT);
  pinMode(modeSW, INPUT_PULLUP);

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

  JOYDEADZONE = 0.05;

  //setting TxPower
  rf95.setTxPower(23, false);
}

void loop() {
  joyX = (analogRead(joyXpin)-500.0)/500.0;
  joyY = (analogRead(joyYpin)-500.0)/500.0;
  modeSWout = digitalRead(modeSW);
  if (modeSWout == 0) {
    txStruct.mode += 1.0;
    if (txStruct.mode > 1) {
      txStruct.mode = 0.00;
    }
    while (modeSWout == 0) {
      modeSWout = digitalRead(modeSW);
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
