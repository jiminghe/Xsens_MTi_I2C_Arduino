#ifndef XBUS_H
#define XBUS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#define OFFSET_TO_PREAMBLE		0
#define OFFSET_TO_BID			1
#define OFFSET_TO_MID			2
#define OFFSET_TO_LEN			3
#define OFFSET_TO_LEN_EXT_HI	4
#define OFFSET_TO_LEN_EXT_LO	5
#define OFFSET_TO_PAYLOAD		4
#define OFFSET_TO_PAYLOAD_EXT	6
#define XBUS_CHECKSUM_SIZE		1
#define LENGTH_EXTENDER_BYTE	0xFF
#define XBUS_PREAMBLE			0xFA
#define XBUS_MASTERDEVICE		0xFF
#define XBUS_EXTENDED_LENGTH	0xFF




bool Xbus_checkPreamble(const uint8_t* xbusMessage);

int  Xbus_getBusId(const uint8_t* xbusMessage);
void Xbus_setBusId(uint8_t* xbusMessage, uint8_t busId);

int  Xbus_getMessageId(const uint8_t* xbusMessage);
void Xbus_setMessageId(uint8_t* xbusMessage, uint8_t messageId);

int  Xbus_getPayloadLength(const uint8_t* xbusMessage);
void Xbus_setPayloadLength(uint8_t* xbusMessage, uint16_t payloadLength);

void Xbus_message(uint8_t* xbusMessage, uint8_t bid, uint8_t mid, uint16_t len);

int Xbus_getRawLength(const uint8_t* xbusMessage);

uint8_t* Xbus_getPointerToPayload(uint8_t* xbusMessage);
uint8_t const* Xbus_getConstPointerToPayload(uint8_t const* xbusMessage);


void Xbus_insertChecksum(uint8_t* xbusMessage);
bool Xbus_verifyChecksum(const uint8_t* xbusMessage);

#ifdef __cplusplus
}
//}
#endif


#endif
