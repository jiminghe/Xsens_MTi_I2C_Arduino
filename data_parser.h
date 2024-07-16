#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <Arduino.h>
#include "xsdatapacket.h"


class DataParser {
public:
    DataParser();
    ~DataParser();

    void parseDataPacket(const uint8_t* data);
    XsDataPacket* getXsDatePacket();

private:
    void dataswapendian(uint8_t* data, int len);
    void parseMTData2( uint8_t* data, uint8_t datalength);
    double parseFP1632(const uint8_t* data);
    bool isLeapYear(uint16_t year);
    uint32_t dateToEpochDays(uint16_t year, uint8_t month, uint8_t day);
    double dateTimeToEpoch(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond);

    XsDataPacket* m_xspacket;
    size_t m_packet_length;
};

#endif // DATA_PARSER_H
