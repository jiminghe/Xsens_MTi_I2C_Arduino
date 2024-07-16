#include "mt_application.h"
#include "xbus.h"
#include "xbusmessageid.h"
#include "xbustostring.h"
#include "data_parser.h"

MtApplication::MtApplication(MtsspDriver* driver, uint8_t drdy)
  : m_state(STATE_Idle), m_driver(driver), m_drdy(drdy) {
  m_device = new MtsspInterface(m_driver);
  m_parser = new DataParser();
  m_xspacket = new XsDataPacket();
}

MtApplication::~MtApplication() {
  // Clean up dynamically allocated memory
  delete m_device;
  delete m_parser;
  delete m_xspacket;
}


void MtApplication::setLiveDataCallback(LiveDataCallback callback) {
  onLiveDataAvailable = callback;
}


bool MtApplication::checkDataReadyLine() {
  return digitalRead(m_drdy);
}



bool MtApplication::start() {
  handleEvent(EVT_Start);
  // Serial.println("MtApp::start is called.");
  delay(10);  //10 milliseconds

  return true;
}


void MtApplication::readData() {
  if (checkDataReadyLine()) {
    // Serial.println("MtApp DRDY 1.");
    readDataFromDevice();
  }
}



void MtApplication::handleEvent(Event event, const uint8_t* data) {
  // Serial.println("MtApplication::handleEvent is called.");
  switch (m_state) {
    Serial.println(m_state);
    case STATE_Idle:
      {
        if (event == EVT_Start) {
          Serial.println("Resetting the device"); //For the GNSS/INS, it would take tens of seconds to get position after reset.
          resetDevice();
          m_state = STATE_WaitForWakeUp;
        }
      }
      break;

    case STATE_WaitForWakeUp:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_Wakeup) {
          printRawXbus(data);
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_GotoConfig, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
          m_state = STATE_WaitForConfigMode;
        }
      }
      break;

    case STATE_WaitForConfigMode:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_GotoConfigAck) {
          printRawXbus(data);
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_ReqDid, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
          m_state = STATE_WaitForDeviceId;
        }
        else
        {
          //doesn't work, have to reset it again, this is useful for the first time powering up the Arduino Mega board.
          Serial.println("Resetting the device again");
          m_state = STATE_Idle;
          handleEvent(EVT_Start);
        }

      }
      break;

    case STATE_WaitForDeviceId:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_DeviceId) {
          printRawXbus(data);
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_ReqProductCode, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
          m_state = STATE_WaitForProductCode;
        }
      }
      break;

    case STATE_WaitForProductCode:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_ProductCode) {
          printRawXbus(data);
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_ReqFirmwareRevision, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
          m_state = STATE_WaitForFirmwareRevision;
        }
      }
      break;

    case STATE_WaitForFirmwareRevision:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_FirmwareRevision) {
          printRawXbus(data);

          //utc_time 0x1010, sampletimefine 0x1060, statusword 0xE020
          //euler angles 0x2030, quaternion 0x2010, free_acc 0x4030
          //rateofturn 0x8020, acc 0x4020, mag 0xC020
          //latlon FP1632 0x5042, alt FP1632 0x5022, velocity FP1632 0xD012
          uint8_t data_rate = 1; //change this value to desired data
          uint8_t output_config_payload[] = { 0x10, 0x60, 0x00, data_rate, 0x20, 0x30, 0x00, data_rate, 0x40, 0x20, 0x00, data_rate, 0x80, 0x20, 0x00, data_rate};
          String output_str = "Setting Output data to " + String(data_rate) + " Hz.";
          Serial.println(output_str);
          //uint8_t output_config_payload[] = {0x10, 0x60, 0xFF, 0xFF, 0x20, 0x30, 0x00, 0x64, 0x40, 0x20, 0x00, 0x64, 0x80, 0x20, 0x00, 0x64, 0xC0, 0x20, 0x00, 0x64,0xE0, 0x20, 0xFF, 0xFF};
          size_t payload_size = sizeof(output_config_payload);
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_SetOutputConfiguration, payload_size);
          uint8_t* payload = Xbus_getPointerToPayload(m_xbusTxBuffer);
          //assign the values from output_config_payload to payload.
          memcpy(payload, output_config_payload, payload_size);
          m_device->sendXbusMessage(m_xbusTxBuffer);
          m_state = STATE_WaitForSetOutputConfigurationAck;
        }
      }
      break;

    case STATE_WaitForSetOutputConfigurationAck:
      {
        if (event == EVT_XbusMessage && Xbus_getMessageId(data) == XMID_SetOutputConfigurationAck) {
          printRawXbus(data);
          m_state = STATE_Ready;
          handleEvent(EVT_GotoMeasuring);
        }
      }
      break;

    case STATE_Ready:
      {

        if (event == EVT_GotoConfig) {
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_GotoConfig, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
        }

        if (event == EVT_GotoMeasuring) {
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_GotoMeasurement, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
        }

        if (event == EVT_RequestDeviceId) {
          Xbus_message(m_xbusTxBuffer, 0xFF, XMID_ReqDid, 0);
          m_device->sendXbusMessage(m_xbusTxBuffer);
        }

        if (event == EVT_XbusMessage) {

          if(data[2] == XMID_MtData2)
          {
            //Debug print the raw hex bytes
            // int data_length = data[3] + 5;
            // String hexStr = bytesToHexString(data, data_length);
            // Serial.println(hexStr);
            m_parser->parseDataPacket(data);
            m_xspacket = m_parser->getXsDatePacket();
            
            onLiveDataAvailable(m_xspacket, sizeof(data));

          }

        }
      }
      break;
  }
}



void MtApplication::resetDevice() {
  //Software reset: FA FF 40 00 C1
  // cout << "MtApplication::resetDevice sending reset message 40 00 C1" <<endl;
  Xbus_message(m_xbusTxBuffer, 0xFF, XMID_Reset, 0);
  m_device->sendXbusMessage(m_xbusTxBuffer);
}



void MtApplication::readDataFromDevice() {
  uint16_t notificationMessageSize;
  uint16_t measurementMessageSize;
  m_device->readPipeStatus(notificationMessageSize, measurementMessageSize);

  m_dataBuffer[0] = XBUS_PREAMBLE;
  m_dataBuffer[1] = XBUS_MASTERDEVICE;

  if (notificationMessageSize && notificationMessageSize < sizeof(m_dataBuffer)) {
    // String noti_size = "notifSize = " + String(notificationMessageSize);
    // Serial.println(noti_size);
    m_device->readFromPipe(&m_dataBuffer[2], notificationMessageSize, XBUS_NOTIFICATION_PIPE);
    handleEvent(EVT_XbusMessage, m_dataBuffer);
  }

  if (measurementMessageSize && measurementMessageSize < sizeof(m_dataBuffer)) {
    // String meas_size = "measeSize = " + String(measurementMessageSize);
    // Serial.println(meas_size);
    m_device->readFromPipe(&m_dataBuffer[2], measurementMessageSize, XBUS_MEASUREMENT_PIPE);
    handleEvent(EVT_XbusMessage, m_dataBuffer);
  }
}
