#ifndef MTSSP_DRIVER_H
#define MTSSP_DRIVER_H

#include <Arduino.h>
#include "xbus.h"
#include "mtssp.h"


class MtsspDriver {
public:

  virtual void write(uint8_t opcode, uint8_t const* data, int dataLength) = 0;

  virtual void read(uint8_t opcode, uint8_t* data, int dataLength) = 0;

  virtual void writeRaw(uint8_t const* data, int dataLength) = 0;

  virtual XbusBusFormat busFormat() const = 0;
};




#endif
