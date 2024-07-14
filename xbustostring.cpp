#include "xbustostring.h"
#include "xbusmessageid.h"
#include "xbus.h"
#include "xsdataidentifier.h"

String g_textBuffer;

uint8_t readUint8(const uint8_t *data, int &index) {
  uint8_t result = data[index++];
  return result;
}

uint16_t readUint16(const uint8_t *data, int &index) {
  // uint16_t result = 0;
  // result |= data[index++] << 8;
  // result |= data[index++] << 0;
    uint16_t value = (data[index] << 8) | data[index + 1];
    index += 2;
    return value;
  // return result;
}

uint32_t readUint32(const uint8_t *data, int &index) {
  uint32_t result = 0;
  result |= data[index++] << 24;
  result |= data[index++] << 16;
  result |= data[index++] << 8;
  result |= data[index++] << 0;
  return result;
}

// uint32_t readUint32BE(const uint8_t *data, int &index) {
//   uint32_t result = 0;
//   result |= data[index++] << 0;  // Least significant byte first
//   result |= data[index++] << 8;
//   result |= data[index++] << 16;
//   result |= data[index++] << 24;  // Most significant byte last
//   return result;
// }

void dataswapendian(uint8_t *data, int len) {
  int i = 0, j = len - 1;
  while (i < j) {
    uint8_t temp = data[i];
    data[i] = data[j];
    data[j] = temp;
    i++;
    j--;
  }
}

double parseFP1632(const uint8_t *data) {
  int32_t fpfrac;
  int16_t fpint;

  // Note: memcpy is used to prevent issues caused by alignment requirements
  memcpy(&fpfrac, data, sizeof(fpfrac));
  memcpy(&fpint, data + 4, sizeof(fpint));

  // Convert the big-endian to the host's byte order
  fpfrac = ((fpfrac >> 24) & 0xff) | ((fpfrac << 8) & 0xff0000) | ((fpfrac >> 8) & 0xff00) | ((fpfrac << 24) & 0xff000000);

  fpint = (fpint >> 8) | (fpint << 8);

  int64_t fp_i64 = (static_cast<int64_t>(fpint) << 32) | (static_cast<int64_t>(fpfrac) & 0xffffffff);

  double rv_d = static_cast<double>(fp_i64) / 4294967296.0;
  return rv_d;
}

// float readFloat(const uint8_t *data, int &index) {
//   Serial.print("Reading float at index: ");
//   Serial.println(index);
//   for (int i = 0; i < 4; i++) {
//     Serial.print(data[index + i], HEX);
//     Serial.print(" ");
//   }
//   Serial.println();

//   uint32_t temp = readUint32(data, index);
//   Serial.print("temp big endian  = ");
//   Serial.println(temp);
//   float result;
//   memcpy(&result, &temp, 4);
//   Serial.print("result  = ");
//   Serial.println(result);
//   return result;
// }

String bytesToHexString(const uint8_t *data, int length) {
  String hexString = "";  // Create an empty string to hold the result
  for (int i = 0; i < length; ++i) {
    // Format each byte as a two-digit hexadecimal number and append to the string
    if (data[i] < 0x10) {
      // Add a leading zero for values less than 0x10 (16 in decimal)
      hexString += "0";
    }
    hexString += String(data[i], HEX);
  }
  hexString.toUpperCase();  // Convert to uppercase if needed
  return hexString;
}

//FA FF 36 2D 40 20 0C 3CE4BCAA 3FF5E331 4119537C 8020 0C BB9B0AA13C0722883BCCBAF3C0200CBD69C6C03D98C820BF3843441A

void readFloatMTi(float &axisValue, uint8_t *data, int &currentIndex) {
  Serial.print("index rfv1= "); Serial.println(currentIndex); 
  dataswapendian((uint8_t *)&data[currentIndex], 4);
  memcpy(&axisValue, &data[currentIndex], 4);
  currentIndex += 4;
  Serial.print("index rfv2  = "); Serial.println(currentIndex); 
}

void printRawXbus(const uint8_t *xbusData) {
  if (!Xbus_checkPreamble(xbusData))
    return "Invalid xbus message";

  // int data_length = xbusData[3] + 5;
  // String hexStr = bytesToHexString(xbusData, data_length);
  // Serial.println(hexStr);

  uint8_t messageId = Xbus_getMessageId(xbusData);
  int index = 4;

  switch (messageId) {
    case XMID_Wakeup:
      {
        Serial.println("XMID_Wakeup");
      }
      break;

    case XMID_DeviceId:
      {
        String deviceId_String = "";
        for (int i = 4; i <= 7; i++) {
          if (xbusData[i] < 0x10) {
            // Ensure two characters for each byte, add leading zero if necessary
            deviceId_String += "0";
          }
          deviceId_String += String(xbusData[i], HEX);
        }
        deviceId_String.toUpperCase();  // Convert the hex string to uppercase

        g_textBuffer = "XMID_DeviceId: " + deviceId_String;
        Serial.println(g_textBuffer);
      }
      break;

    case XMID_ProductCode:
      {
        char productCode[21];  // 20 characters + 1 for null terminator
        memcpy(productCode, xbusData + index, 20);
        productCode[20] = '\0';  // Ensure null termination
        g_textBuffer = "XMID_ProductCode: " + String(productCode);
        Serial.println(g_textBuffer);
      }
      break;

    case XMID_GotoConfigAck:
      {
        Serial.println("XMID_GotoConfigAck");
      }
      break;

    case XMID_GotoMeasurementAck:
      {
        Serial.println("XMID_GotoMeasurementAck");
      }
      break;

    case XMID_MtData2:
      break;

    case XMID_FirmwareRevision:
      {
        uint8_t major = readUint8(xbusData, index);
        uint8_t minor = readUint8(xbusData, index);
        uint8_t patch = readUint8(xbusData, index);
        g_textBuffer = "Firmware revision: " + String(major) + "." + String(minor) + "." + String(patch);
        Serial.println(g_textBuffer);
      }
      break;

    case XMID_GotoBootLoaderAck:
      {
        g_textBuffer = "XMID_GotoBootLoaderAck";
        Serial.println(g_textBuffer);
      }
      break;

    case XMID_FirmwareUpdate:
      {
        g_textBuffer = "XMID_FirmwareUpdate";
        Serial.println(g_textBuffer);
      }
      break;

    case XMID_ResetAck:
      {
        g_textBuffer = "XMID_ResetAck";
        Serial.println(g_textBuffer);
      }
      break;
    
    case XMID_SetOutputConfigurationAck:
      {
        g_textBuffer = "XMID_SetOutputConfigurationAck";
        Serial.println(g_textBuffer);
      }
      break;

    default:
      {
        g_textBuffer = "Unhandled xbus message: MessageId = 0x" + String(messageId, HEX);
        Serial.println(g_textBuffer);
      }
  }
}
