#include "data_parser.h"
#include "xbusmessageid.h"
#include "xsdataidentifier.h"


DataParser::DataParser(const uint8_t* data)
:m_rawpacket(data)
{
  m_packet_length = data[3] + 5;
  //Debug print:
  // String str = "m_packet_length = 0x" + String(m_packet_length, HEX);
  // Serial.println(str);
}

DataParser::~DataParser() {
    delete[] m_rawpacket;
}


bool DataParser::isLeapYear(uint16_t year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Compute the number of days since the Unix epoch (1st Jan 1970) to the given date.
uint32_t DataParser::dateToEpochDays(uint16_t year, uint8_t month, uint8_t day) {
    static const uint8_t daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint32_t days = 0;

    // Days from 1970 to the last year
    for (uint16_t y = 1970; y < year; ++y) {
        days += isLeapYear(y) ? 366 : 365;
    }

    // Days from January to the last complete month
    for (uint8_t m = 1; m < month; ++m) {
        days += daysInMonth[m - 1];
        if (m == 2 && isLeapYear(year)) {
            days += 1;  // February in a leap year
        }
    }

    // Days in the current month
    days += day - 1;

    return days;
}

// Convert date and time to epoch time in seconds
uint32_t DataParser::dateTimeToEpoch(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second) {
    uint32_t days = dateToEpochDays(year, month, day);
    uint32_t totalSeconds = days * 86400 + hour * 3600 + minute * 60 + second;
    return totalSeconds;
}


XsDataPacket DataParser::parseDataPacket() {
    size_t input_size = m_packet_length;
    if (input_size < 3) return; // Ensure there's enough data to start parsing

    XsDataPacket xbusData;
    // packet = FA FF 36 16 10 60 04 09 3A 7C B7 20 30 0C BF 49 1A 52 BE C3 6A 10 C3 2E E2 EF 3E
    size_t offset = 2; // Start after the presumed packet header FA FF

    // Check for MTData2 preamble
    if (m_rawpacket[offset] == XMID_MtData2)
    {
        offset++; // move past MTData2 identifier
        if (offset >= input_size) return; // Safety check

        uint8_t dataLengthMT = m_rawpacket[offset]; // data length for MTData2
        offset++; // move to the first data type in MTData2

        if (offset >= input_size) return; // Safety check
        size_t endMTData2 = offset + dataLengthMT; // Calculate the end of the MTData2 block

        if (endMTData2 > input_size) return; // Ensure we don't read past the buffer

        while (offset < endMTData2)
        {
            if (offset + 2 > input_size) return; // Ensure space for dataType and dataLength
            offset += 2; // Move past the 2-byte dataType

            uint8_t dataLength = m_rawpacket[offset];
            offset++; // Move past dataLength byte

            if (offset + dataLength > input_size) return; // Ensure we don't read past the buffer

            const uint8_t* dataPtr = &m_rawpacket[offset - 3]; // Adjust this to point to the start of dataType
            parseMTData2(&xbusData, dataPtr, dataLength + 3);

            offset += dataLength; // Move past the data to the next data type
        }
    }

    return xbusData;
}



// void DataParser::parseDataPacket(const std::vector<uint8_t> &packet, Xbus &xbusData)
// {
//     size_t offset = 2; // packet = FA FF 36 16 10 60 04 09 3A 7C B7 20 30 0C BF 49 1A 52 BE C3 6A 10 C3 2E E2 EF 3E

//     // Check for MTData2 preamble
//     if (packet[offset] == XMID_MtData2)
//     {
//         offset++;                              // move past MTData2 identifier
//         uint8_t dataLengthMT = packet[offset]; // data length for MTData2
//         offset++;                              // move to the first data type in MTData2

//         size_t endMTData2 = offset + dataLengthMT; // Calculate the end of the MTData2 block

//         while (offset < endMTData2)
//         {
//             offset += 2; // Move past the 2-byte dataType
//             uint8_t dataLength = packet[offset];

//             offset++;                                                      // Move past dataLength byte
//             uint8_t *dataPtr = const_cast<uint8_t *>(&packet[offset - 3]); // Adjust this to point to the start of dataType
//             parseMTData2(&xbusData, dataPtr, dataLength + 3);

//             offset += dataLength; // Move past the data to the next data type
//         }
//     }
// }

void DataParser::dataswapendian(uint8_t *data, int len)
{
    // Check if the length is divisible by 4
    if (len % 4 != 0)
    {
        Serial.println("Length must be a multiple of 4");
        return;
    }

    // Loop through each 4-byte group
    for (int i = 0; i < len; i += 4)
    {
        // Swap bytes within the group
        uint8_t temp;
        temp = data[i];
        data[i] = data[i + 3];
        data[i + 3] = temp;

        temp = data[i + 1];
        data[i + 1] = data[i + 2];
        data[i + 2] = temp;
    }
}

double DataParser::parseFP1632(const uint8_t *data)
{
    uint32_t fpfrac = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    uint16_t fpint = (data[5] << 8) | data[4];

    int64_t fp_i64 = (static_cast<int64_t>(fpint) << 32) | (static_cast<int64_t>(fpfrac) & 0xffffffff);

    double rv_d = static_cast<double>(fp_i64) / 4294967296.0;
    return rv_d;
}

void DataParser::parseMTData2(XsDataPacket *xspacket, uint8_t *data, uint8_t datalength)
{
    int offset = 0;

    while (offset < datalength)
    {
        uint16_t dataId = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 3; // i.e 10 60 04 0a 14 33 17  ==> 0a 14 33 17

        switch (dataId)
        {
        case XDI_PacketCounter:
            dataswapendian(&data[offset], 2);
            xspacket->packetCounter = *reinterpret_cast<uint16_t *>(&data[offset]);
            xspacket->packetCounterAvailable = true;
            offset += 2;
            break;
        case XDI_SampleTimeFine: // Sample time fine
            dataswapendian(&data[offset], 4);
            xspacket->sampleTimeFine = *reinterpret_cast<uint32_t *>(&data[offset]);
            xspacket->sampleTimeFineAvailable = true;
            offset += 4;
            break;
        case XDI_UtcTime: // UTC Time stamp
        {
            dataswapendian(&data[offset], 4);
            uint32_t nanosec = *reinterpret_cast<uint32_t *>(&data[offset]);
            offset += 4;

            dataswapendian(&data[offset], 2);
            uint16_t year = *reinterpret_cast<uint16_t *>(&data[offset]);
            offset += 2;

            uint8_t month = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;

            uint8_t day = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;

            uint8_t hour = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;

            uint8_t minute = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;

            uint8_t sec = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;

            // uint8_t flags = *reinterpret_cast<uint8_t *>(&data[offset]);
            offset++;
            uint16_t tm_year = year - 1900; // Years since 1900
            uint8_t tm_mon = month - 1;    // Months since January
            //uint32_t dateTimeToEpoch(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
            uint32_t epochTime = dateTimeToEpoch(tm_year, tm_mon, day, hour, minute, sec);
            // Convert to time_t (seconds since 1st Jan 1970)
            xspacket->utcTime = epochTime + nanosec* 1e-9;
            xspacket->utcTimeAvailable = true;
            break;
        }
        case XDI_EulerAngles: // Euler Angles
            dataswapendian(&data[offset], 12);
            memcpy(xspacket->euler, &data[offset], 12);
            xspacket->eulerAvailable = true;
            offset += 12;
            break;
        case XDI_Quaternion: // Quaternion.
            dataswapendian(&data[offset], 16);
            memcpy(xspacket->quat, &data[offset], 16);
            xspacket->quaternionAvailable = true;
            xspacket->convertQuatToEuler();
            offset += 16;
            break;
        case XDI_Acceleration: // Acceleration
            dataswapendian(&data[offset], 12);
            memcpy(xspacket->acc, &data[offset], 12);
            xspacket->accAvailable = true;
            offset += 12;
            break;

        case XDI_RateOfTurn: // Rate of Turn
            dataswapendian(&data[offset], 12);
            memcpy(xspacket->gyro, &data[offset], 12);
            xspacket->gyroAvailable = true;
            offset += 12;
            break;

        case 0x5042: // Latitude Longitude, FP16.32
            xspacket->latlon[0] = parseFP1632(&data[offset]);
            offset += 6;
            xspacket->latlon[1] = parseFP1632(&data[offset]);
            xspacket->latlonAvailable = true;
            offset += 6;
            break;
        case 0x5022: // AltitudeEllipsoid, FP16.32
            xspacket->altitude = parseFP1632(&data[offset]);
            xspacket->altitudeAvailable = true;
            offset += 6;
            break;
        case XDI_MagneticField: // Magnetic Field
            dataswapendian(&data[offset], 12);
            memcpy(xspacket->mag, &data[offset], 12);
            xspacket->magAvailable = true;
            offset += 12;
            break;
        case XDI_StatusWord: // StatusWord
            memcpy(&xspacket->statusWord, &data[offset], 4);
            xspacket->statusWordAvailable = true;
            offset += 4;
            break;
        case 0xD012: // Velocity, FP16.32
            xspacket->vel[0] = parseFP1632(&data[offset]);
            offset += 6;
            xspacket->vel[1] = parseFP1632(&data[offset]);
            offset += 6;
            xspacket->vel[2] = parseFP1632(&data[offset]);
            offset += 6;
            xspacket->velocityAvailable = true;
            break;
        default:
            String error_str = "Unrecognized data ID: 0x" + String(dataId, HEX) + " at offset " + String(offset) + ". Following bytes: ";
            for (int i = 0; i < min(static_cast<int>(datalength - offset), 5); i++)
            { // print up to next 5 bytes
                error_str += String(data[offset + i], HEX);
            }
            Serial.println(error_str);
            break;
        }
    }
}
