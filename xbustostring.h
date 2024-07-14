#ifndef XBUSTOSTRING_H
#define XBUSTOSTRING_H

#include <Arduino.h>

String bytesToHexString(const uint8_t *data, int length);
void printRawXbus(const uint8_t* xbusData);


#endif
