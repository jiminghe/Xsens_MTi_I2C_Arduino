#include <Arduino.h>
#include <Wire.h>
#include "mtssp_driver_i2c.h"
#include "mt_application.h"
#include "xsdatapacket.h"

#define MTI_I2C_DEVICE_ADDRESS     0x6B //Default I2C address, if you want to change the address of MTi-3, connect the ADD0/1/2 pins to GND according to the MTi-1 Datasheet.
#define DATA_READY_PIN             3  // GPIO3 for DRDY line

MtsspDriver* driver = NULL;
MtApplication* app = NULL;
bool startIsOk = false;

void liveDataHandler(const XsDataPacket& packet);

void setup() {
  driver = new MtsspDriverI2c(MTI_I2C_DEVICE_ADDRESS);
  app = new MtApplication(driver, DATA_READY_PIN);

  Serial.begin(115200);
  Wire.begin();
  delay(500); // Delay 0.5sec to allow I2C bus to stabilize
  pinMode(DATA_READY_PIN, INPUT);  //Set DATA_READY_PIN as input, indicates whether data/notifications are available to be read
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

void liveDataHandler(const XsDataPacket& packet) 
{

  if (packet.eulerAvailable)
            {
              String euler_str = "Roll: " + String(packet.euler[0]) + ", Pitch: " + String(packet.euler[1])  + ", Yaw: " + String(packet.euler[2]) ;
              Serial.println(euler_str);

            }
            if (packet.accAvailable)
            {
              String acc_str = "Acc x: " + String(packet.acc[0]) + ", Acc y: " + String(packet.acc[1])  + ", Acc z: " + String(packet.acc[2]) ;
              Serial.println(acc_str);
            }
            if (packet.gyroAvailable)
            {
                // float rateOfTurnDegree[3];
                // //RateOfTurn is in radians/sec, convert to degrees/sec.
                // rateOfTurnDegree[0] = packet.rad2deg * packet.gyro[0];
                // rateOfTurnDegree[1] = packet.rad2deg * packet.gyro[1];
                // rateOfTurnDegree[2] = packet.rad2deg * packet.gyro[2];

                // String gyro_str = "Gyro(dps), x: " + String(rateOfTurnDegree[0]) + ", y: " + String(rateOfTurnDegree[1])  + ", z: " + String(rateOfTurnDegree[2]) ;
                String gyro_str = "Gyro x: " + String(packet.gyro[0]) + ", Gyro y: " + String(packet.gyro[1])  + ", Gyro z: " + String(packet.gyro[2]) ;
                Serial.println(gyro_str);
            }
            if (packet.magAvailable)
            {
                String mag_str = "Mag, x: " + String(packet.mag[0]) + ", y: " + String(packet.mag[1])  + ", z: " + String(packet.mag[2]) ;
                Serial.println(mag_str);
            }

            Serial.println();



}
