#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <SerialTransfer.h>

#include <Defines.h>

// https://forum.arduino.cc/t/how-do-i-send-sensor-data-using-rf95-when-variables-are-in-a-struct/500286

// setup radio and serialtransfer objects
SerialTransfer roverTransfer;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// struct for recieving data from transmitter
struct dataStruct{
  float drive;
  float steer;
  float mode;
}CtrlRead;

// struct for sending data to jetson
struct __attribute__((packed)) STRUCT {
  float drive;
  float steer;
  float mode;
} roverTX;

// timeout variable declarations
unsigned long time, lastTime;
int timeouts;

void setup() {
  pinMode(BLINKER, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  Serial.begin(115200);
  roverTransfer.begin(Serial);  // start serialtransfer library on Serial object

  // manual reset for LoRa
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
  CtrlRead.mode = 0;
  roverTX.drive = 0;
  roverTX.steer = 0;
  roverTX.mode = 0;
}

void loop() {
  time = millis();  // set current Beginning Of Cycle Time
  if (rf95.available()) { // checks if there is a new message ready for "depacketing"
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) { // read values to struct
      digitalWrite(LED_BUILTIN, HIGH);
      memcpy(&CtrlRead, buf, sizeof(CtrlRead));
      roverTX.drive = CtrlRead.drive;
      roverTX.steer = CtrlRead.steer;
      roverTX.mode = CtrlRead.mode;
      lastTime = millis();  // reset last recieved message timestamp
      timeouts = 0; //reset timeout counter
    }
  }
  // if the time between last message and current time is
  // greater than timeout, add a timeout occurance
  if(time - lastTime > TIMEOUT) {
    timeouts = timeouts + 1;
  }
  // if too many timeouts occur, set all outputs to zero
  if (timeouts > MAX_TIMEOUTS) {
    roverTX.drive = 0.00;
    roverTX.steer = 0.00;
    roverTX.mode = 0.00;
    digitalWrite(BLINKER,LOW);
  }
  /*
    // test code to print recieved values
    Serial.print("Drive: ");
    Serial.print(CtrlRead.drive,12);
    Serial.print("    ");
    Serial.print("Steer: ");
    Serial.println(CtrlRead.steer,12);
  */
    // transfer recieved struct to serialtransfer library
    uint16_t sendSize = 0;
    sendSize = roverTransfer.txObj(roverTX, sendSize);
    roverTransfer.sendData(sendSize);
}