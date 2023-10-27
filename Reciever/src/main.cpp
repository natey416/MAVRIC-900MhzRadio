#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

// https://forum.arduino.cc/t/how-do-i-send-sensor-data-using-rf95-when-variables-are-in-a-struct/500286

// Feather 32u4
#define RFM95_CS   8
#define RFM95_RST  4
#define RFM95_INT  7

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

struct dataStruct{
  int drive;
  int steer;
}CtrlRead;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  Serial.begin(115200);
  while (!Serial) delay(1);
  Serial.setTimeout(1); 
  Serial.println("Feather LoRa RX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(23, false);
  CtrlRead.drive = 0;
  CtrlRead.steer = 0;
}

void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      digitalWrite(LED_BUILTIN, HIGH);
      memcpy(&CtrlRead, buf, sizeof(CtrlRead));
    }
    
    Serial.println("Drive: " + String(CtrlRead.drive));
    Serial.println("Steer: " + String(CtrlRead.steer));


    /*
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      digitalWrite(LED_BUILTIN, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      Serial.print("Size: ");
      Serial.println(len);
      
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.println("Receive failed");
    }
    */
  }
}