#ifndef XSDATAPACKET_H
#define XSDATAPACKET_H

#include <Arduino.h>

struct XsDataPacket {
    float euler[3];
    bool eulerAvailable = false;

    float quat[4];
    bool quaternionAvailable = false;  

    float acc[3];
    bool accAvailable = false;  

    float freeacc[3];
    bool freeaccAvailable = false;  

    float gyro[3];
    bool gyroAvailable = false;

    double latlon[2]; //FP16.32
    bool latlonAvailable = false;

    double altitude; //FP16.32
    bool altitudeAvailable = false;

    double vel[3]; //FP16.32
    bool velocityAvailable = false;

    float mag[3];
    bool magAvailable = false;

    uint16_t packetCounter;
    bool packetCounterAvailable = false;

    uint32_t sampleTimeFine;
    bool sampleTimeFineAvailable = false;

    double utcTime; //seconds since 1st Jan 1970
    // struct tm utcTimeInfo = {0};
    bool utcTimeAvailable = false;

    uint32_t statusWord;
    bool statusWordAvailable = false;


    static constexpr double rad2deg = 57.295779513082320876798154814105;
    static constexpr double minusHalfPi = -1.5707963267948966192313216916397514420985846996875529104874;
    static constexpr double halfPi = 1.5707963267948966192313216916397514420985846996875529104874;

    static float asinClamped(float x) {
        if (x <= -1.0 ) return minusHalfPi;
        if (x >= 1.0) return halfPi;
        return asin(x);
    }


    void convertQuatToEuler() {
        if (!quaternionAvailable) {
            // Handle error: Quaternion data not available.
            return;
        }
        
        float sqw = quat[0] * quat[0];
        float dphi = 2.0 * (sqw + quat[3] * quat[3]) - 1.0;
        float dpsi = 2.0 * (sqw + quat[1] * quat[1]) - 1.0;
    
        euler[0] = atan2(2.0 * (quat[2] * quat[3] + quat[0] * quat[1]), dphi) * rad2deg;
        euler[1] = -asinClamped(2.0 * (quat[1] * quat[3] - quat[0] * quat[2]))* rad2deg;
        euler[2] = atan2(2.0 * (quat[1] * quat[2] + quat[0] * quat[3]), dpsi)* rad2deg;

        eulerAvailable = true;
    }
};


#endif