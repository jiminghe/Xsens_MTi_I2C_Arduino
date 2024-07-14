#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <Arduino.h>
#include "xsdatapacket.h"


class DataParser {
public:
    DataParser(const uint8_t* data);
    ~DataParser();

    XsDataPacket parseDataPacket();

private:
    void dataswapendian(uint8_t* data, int len);
    void parseMTData2(XsDataPacket* xspacket, uint8_t* data, uint8_t datalength);
    double parseFP1632(const uint8_t* data);
    bool isLeapYear(uint16_t year);
    uint32_t dateToEpochDays(uint16_t year, uint8_t month, uint8_t day);
    uint32_t dateTimeToEpoch(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second);

    uint8_t* m_rawpacket;
    size_t m_packet_length;
};

#endif // DATA_PARSER_H
