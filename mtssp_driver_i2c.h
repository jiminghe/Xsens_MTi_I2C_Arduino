#ifndef MTSSP_DRIVER_I2C_H
#define MTSSP_DRIVER_I2C_H

#include "mtssp_driver.h"
#include "xbus.h"


class MtsspDriverI2c : public MtsspDriver {
public:
  MtsspDriverI2c(uint8_t deviceAddress);
  ~MtsspDriverI2c();

  virtual void write(uint8_t opcode, uint8_t const* data, int dataLength);
  virtual void read(uint8_t opcode, uint8_t* dest, int dataLength);
  virtual void writeRaw(uint8_t const* data, int dataLength);

  virtual XbusBusFormat busFormat() const {
    return XBF_I2c;
  }

private:
  uint8_t m_deviceAddress;
  int m_i2c_fd;
};


#endif
