#include "xbus.h"

bool Xbus_checkPreamble(const uint8_t* xbusMessage) {
  return xbusMessage[OFFSET_TO_PREAMBLE] == XBUS_PREAMBLE;
}

int Xbus_getBusId(const uint8_t* xbusMessage) {
  return (xbusMessage[OFFSET_TO_BID] & 0xff);
}


void Xbus_setBusId(uint8_t* xbusMessage, uint8_t busId) {
  xbusMessage[OFFSET_TO_BID] = busId & 0xff;
}


int Xbus_getMessageId(const uint8_t* xbusMessage) {
  return (xbusMessage[OFFSET_TO_MID] & 0xff);
}


void Xbus_setMessageId(uint8_t* xbusMessage, uint8_t messageId) {
  xbusMessage[OFFSET_TO_MID] = messageId & 0xff;
}


int Xbus_getPayloadLength(const uint8_t* xbusMessage) {
  int length = xbusMessage[OFFSET_TO_LEN] & 0xff;
  if (length != LENGTH_EXTENDER_BYTE)
    return length;
  else {
    int result = (xbusMessage[OFFSET_TO_LEN + 2] & 0xff);
    result += (xbusMessage[OFFSET_TO_LEN + 1] & 0xff) << 8;
    return result;
  }
}


void Xbus_setPayloadLength(uint8_t* xbusMessage, uint16_t payloadLength) {
  if (payloadLength < 255)
    xbusMessage[OFFSET_TO_LEN] = payloadLength & 0xff;
  else {
    xbusMessage[OFFSET_TO_LEN] = LENGTH_EXTENDER_BYTE;
    xbusMessage[OFFSET_TO_LEN + 1] = (payloadLength >> 8) & 0xff;
    xbusMessage[OFFSET_TO_LEN + 2] = payloadLength & 0xff;
  }
}


void Xbus_message(uint8_t* xbusMessage, uint8_t bid, uint8_t mid, uint16_t len) {
  xbusMessage[0] = 0xFA;
  Xbus_setBusId(xbusMessage, bid);
  Xbus_setMessageId(xbusMessage, mid);
  Xbus_setPayloadLength(xbusMessage, len);
}


int Xbus_getRawLength(const uint8_t* xbusMessage) {
  int rtrn = Xbus_getPayloadLength(xbusMessage);

  if ((xbusMessage[OFFSET_TO_LEN] & 0xff) == LENGTH_EXTENDER_BYTE)
    rtrn += 7;
  else
    rtrn += 5;
  return rtrn;
}


uint8_t* Xbus_getPointerToPayload(uint8_t* xbusMessage) {
  if ((xbusMessage[OFFSET_TO_LEN] & 0xff) == LENGTH_EXTENDER_BYTE)
    return xbusMessage + OFFSET_TO_PAYLOAD_EXT;
  else
    return xbusMessage + OFFSET_TO_PAYLOAD;
}


uint8_t const* Xbus_getConstPointerToPayload(uint8_t const* xbusMessage) {
  return Xbus_getPointerToPayload((uint8_t*)xbusMessage);
}



void Xbus_insertChecksum(uint8_t* xbusMessage) {
  int nBytes = Xbus_getRawLength(xbusMessage);

  uint8_t checksum = 0;
  for (int i = 0; i < nBytes - 2; i++)
    checksum -= xbusMessage[1 + i];

  xbusMessage[nBytes - 1] = checksum;
}


bool Xbus_verifyChecksum(const uint8_t* xbusMessage) {
  int nBytes = Xbus_getRawLength(xbusMessage);
  uint8_t checksum = 0;
  for (int n = 1; n < nBytes; n++)
    checksum += (xbusMessage[n] & 0xff);
  checksum &= 0xff;
  return (checksum == 0);
}
