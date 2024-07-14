#include <Wire.h>
#include "mtssp_interface.h"
#include "xbus.h"

MtsspInterface::MtsspInterface(MtsspDriver* driver)
  : m_driver(driver) {
}


void MtsspInterface::readProtocolInfo(uint8_t& version, uint8_t& dataReadyConfig) {
  uint8_t buffer[2];
  m_driver->read(XBUS_PROTOCOL_INFO, buffer, 2);
  version = buffer[0];
  dataReadyConfig = buffer[1];
}


void MtsspInterface::configureProtocol(uint8_t dataReadyConfig) {
  m_driver->write(XBUS_CONFIGURE_PROTOCOL, &dataReadyConfig, sizeof(dataReadyConfig));
}


void MtsspInterface::readPipeStatus(uint16_t& notificationMessageSize, uint16_t& measurementMessageSize) {
  uint8_t status[4];
  m_driver->read(XBUS_PIPE_STATUS, status, sizeof(status));
  notificationMessageSize = status[0] | (status[1] << 8);
  measurementMessageSize = status[2] | (status[3] << 8);
}


void MtsspInterface::readFromPipe(uint8_t* buffer, uint16_t size, uint8_t pipe) {

  if (!(pipe == XBUS_NOTIFICATION_PIPE || pipe == XBUS_MEASUREMENT_PIPE)) {
    Serial.println("mtssp_interface.cpp, pipe is not XBUS_NOTIFICATION_PIPE or XBUS_MEASUREMENT_PIPE");
    return;
  }
  m_driver->read(pipe, buffer, size);
}


size_t Xbus_createRawMessage(uint8_t* dest, uint8_t const* message, enum XbusBusFormat format) {
  int n;
  uint8_t checksum;
  uint16_t length;
  uint8_t* dptr = dest;

  length = Xbus_getPayloadLength(message);

  if (dest == 0) {
    switch (format) {
      case XBF_I2c:
        return (length < 255) ? length + 4 : length + 6;
      case XBF_Spi:
        return (length < 255) ? length + 7 : length + 9;
      case XBF_Uart:
        return (length < 255) ? length + 5 : length + 7;
    }
  }

  switch (format) {
    case XBF_I2c:
      *dptr++ = XBUS_CONTROL_PIPE;
      break;

    case XBF_Spi:
      *dptr++ = XBUS_CONTROL_PIPE;
      // Fill bytes required to allow MT to process data
      *dptr++ = 0;
      *dptr++ = 0;
      *dptr++ = 0;
      break;

    case XBF_Uart:
      *dptr++ = XBUS_PREAMBLE;
      *dptr++ = XBUS_MASTERDEVICE;
      break;
  }

  checksum = 0;
  checksum -= XBUS_MASTERDEVICE;

  *dptr = Xbus_getMessageId(message);
  checksum -= *dptr++;

  if (length < XBUS_EXTENDED_LENGTH) {
    *dptr = length;
    checksum -= *dptr++;
  } else {
    *dptr = XBUS_EXTENDED_LENGTH;
    checksum -= *dptr++;
    *dptr = length >> 8;
    checksum -= *dptr++;
    *dptr = length & 0xFF;
    checksum -= *dptr++;
  }

  for (n = 0; n < length; n++) {
    *dptr = Xbus_getConstPointerToPayload(message)[n];
    checksum -= *dptr++;
  }

  *dptr++ = checksum;

  return dptr - dest;
}


void MtsspInterface::sendXbusMessage(uint8_t const* xbusMessage) {
  uint8_t buffer[128];
  size_t rawLength = Xbus_createRawMessage(buffer, xbusMessage, m_driver->busFormat());
  m_driver->writeRaw(buffer, rawLength);
}
