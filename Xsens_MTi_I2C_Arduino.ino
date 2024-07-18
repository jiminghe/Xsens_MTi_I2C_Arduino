#include <Arduino.h>
#include <Wire.h>
#include "mtssp_driver_i2c.h"
#include "mt_application.h"
#include "xsdatapacket.h"

#define MTI_I2C_DEVICE_ADDRESS 0x6B  //Default I2C address, if you want to change the address of MTi-3, connect the ADD0/1/2 pins to GND according to the MTi-1 Datasheet.
#define DATA_READY_PIN 3             // GPIO3 for DRDY line

#define RESET_PIN 5  // GPIO5 for reset for mti-8

MtsspDriver* driver = NULL;
MtApplication* app = NULL;
bool startIsOk = false;

void liveDataHandler(const XsDataPacket& packet);

void setup() {
  driver = new MtsspDriverI2c(MTI_I2C_DEVICE_ADDRESS);
  app = new MtApplication(driver, DATA_READY_PIN, RESET_PIN);

  Serial.begin(115200);
  Wire.begin();
  delay(500);                      // Delay 0.5sec to allow I2C bus to stabilize
  pinMode(DATA_READY_PIN, INPUT);  //Set DATA_READY_PIN as input, indicates whether data/notifications are available to be read
  
  pinMode(RESET_PIN, OUTPUT);// Set the reset pin as an output
  digitalWrite(RESET_PIN, LOW); //drive the pin low
  // Serial.println("setup is called");

  if (app->start()) {
    app->setLiveDataCallback(liveDataHandler);
    startIsOk = true;
  }
}


void loop() {

  if (startIsOk) {
    app->readData();
    // delay(10);
  }
}

void liveDataHandler(const XsDataPacket& packet) {

  String output_str = "";
  if (packet.utcTimeAvailable) {
    output_str += "Utctime epochSeconds: " + String(packet.utcTime, 6) + "\n";
  }
  if (packet.sampleTimeFineAvailable) {
    output_str += "SampleTimeFine: " + String(packet.sampleTimeFine) + "\n";
  }
  if (packet.eulerAvailable) {
    output_str += "Roll: " + String(packet.euler[0], 3) + ", Pitch: " + String(packet.euler[1], 3) + ", Yaw: " + String(packet.euler[2], 3) + "\n";
  }
  if (packet.quaternionAvailable) {
    output_str += "q0: " + String(packet.quat[0], 3) + ", q1: " + String(packet.quat[1], 3) + ", q2: " + String(packet.quat[2], 3) + ", q3: " + String(packet.quat[3], 3) + "\n";
  }
  if (packet.accAvailable) {
    output_str += "Acc x: " + String(packet.acc[0], 3) + ", Acc y: " + String(packet.acc[1], 3) + ", Acc z: " + String(packet.acc[2], 3) + "\n";
  }
  if (packet.gyroAvailable) {
    output_str += "Gyro x: " + String(packet.gyro[0], 3) + ", Gyro y: " + String(packet.gyro[1], 3) + ", Gyro z: " + String(packet.gyro[2], 3) + "\n";
  }
  if (packet.magAvailable) {
    output_str += "Mag x: " + String(packet.mag[0], 3) + ", Mag y: " + String(packet.mag[1], 3) + ", Mag z: " + String(packet.mag[2], 3) + "\n";
  }
  if (packet.latlonAvailable && packet.altitudeAvailable) {
    output_str += "Lat: " + String(packet.latlon[0], 6) + ", Lon: " + String(packet.latlon[1], 6) + ", Alt: " + String(packet.altitude, 6) + "\n";
  }
  if (packet.velocityAvailable) {
    output_str += "Vel E: " + String(packet.vel[0], 6) + ", Vel N: " + String(packet.vel[1], 6) + ", Vel U: " + String(packet.vel[2], 6) + "\n";
  }
  if (packet.freeaccAvailable) {
    output_str += "Free Acc x: " + String(packet.freeacc[0], 3) + ", Free Acc y: " + String(packet.freeacc[1], 3) + ", Free Acc z: " + String(packet.freeacc[2], 3) + "\n";
  }
  if (packet.statusWordAvailable) {
    output_str += "statusWord: " + String(packet.statusWord) + "\n";
  }

  Serial.println(output_str);
}
