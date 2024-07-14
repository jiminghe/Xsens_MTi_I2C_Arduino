#include <Wire.h>
#include "mtssp_driver_i2c.h"

MtsspDriverI2c::MtsspDriverI2c(uint8_t deviceAddress)
  : m_deviceAddress(deviceAddress) {
}


MtsspDriverI2c::~MtsspDriverI2c() {
}


void MtsspDriverI2c::write(uint8_t opcode, uint8_t const* data, int dataLength) {
  Wire.beginTransmission(m_deviceAddress);  // Start I2C transmission to device
  Wire.write(opcode);                       // Send the opcode as the first byte

  for (int i = 0; i < dataLength; i++) {
    Wire.write(data[i]);  // Send the data bytes
  }

  byte result = Wire.endTransmission();  // End the transmission and check for success

  if (result != 0) {
    // Here you can handle errors if needed
    Serial.print("I2C write error: ");
    Serial.println(result);
  }
}


void MtsspDriverI2c::read(uint8_t opcode, uint8_t* dest, int dataLength) {
  // Start communication by sending the opcode
  if(dataLength > 64)
  {
    return; //we don't handle the case if the bytes are greater than 64 bytes.
  }
  Wire.beginTransmission(m_deviceAddress);
  Wire.write(opcode);
  byte result = Wire.endTransmission();  // End transmission but don't release the bus

  if (result != 0) {
    String error_str = "I2C write error on opcode 0x" + String(opcode, HEX) + ", error code: " + String(result);
    Serial.println(error_str);
    return;
  }

  // Request bytes from slave device
  Wire.requestFrom((int)m_deviceAddress, dataLength);

  int index = 0;
  while (Wire.available() && index < dataLength) {  // slave may send less than requested
    dest[index] = Wire.read();                      // Receive a byte as a proper uint8_t
    index++;
  }

  if (index != dataLength) {
    Serial.print("I2C read error: requested ");
    Serial.print(dataLength);
    Serial.print(" bytes, but got ");
    Serial.println(index);

  }
}



void MtsspDriverI2c::writeRaw(uint8_t const* data, int dataLength) {
  Wire.beginTransmission(m_deviceAddress);  // Start I2C transmission to device

  for (int i = 0; i < dataLength; i++) {
    Wire.write(data[i]);  // Send the data bytes
  }

  byte result = Wire.endTransmission();  // End the transmission and check for success

  if (result != 0) {
    // Error handling
    Serial.print("I2C write error: ");
    Serial.println(result);
  }
}
