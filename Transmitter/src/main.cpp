#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

//defines for 32u4
#define blinker 13
#define RFM95_CS   8
#define RFM95_RST  4
#define RFM95_INT  7

//frequency channel and rf95 instancing
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int drive, steer;

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) delay(1);
  Serial.setTimeout(1); 
  pinMode(blinker, OUTPUT);

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

  //setting TxPower
  rf95.setTxPower(23, false);
}

void loop() {
  while (!Serial.available());
  drive = Serial.readString().toInt(); 
  while (!Serial.available());
  steer = Serial.readString().toInt();
  
  char buf[6] = {0};
  itoa(drive,buf,10);
  rf95.send((uint8_t *)buf,6);
  /*
  //for testing the serial integer data transmission
  for (int i=0; i < abs(steer); i++) {
    digitalWrite(blinker, HIGH);
    delay(200);
    digitalWrite(blinker, LOW);
    delay(200);
  }
   */
}
