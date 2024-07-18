#ifndef MTAPPLICATION_H
#define MTAPPLICATION_H

#include <Arduino.h>
#include <Wire.h>
#include "mtssp_driver.h"
#include "mtssp_interface.h"
#include "xsdatapacket.h"
#include "data_parser.h"



class MtsspDriver;
class MtsspInterface;

enum Event {
  EVT_Start,
  EVT_GotoConfig,
  EVT_GotoMeasuring,
  EVT_Reset,
  EVT_XbusMessage,
  EVT_RequestDeviceId,
};

enum State {
  STATE_Idle,
  STATE_WaitForWakeUp,
  STATE_WaitForConfigMode,
  STATE_WaitForDeviceId,
  STATE_WaitForProductCode,
  STATE_WaitForFirmwareRevision,
  STATE_WaitForSetOutputConfigurationAck,
  STATE_Ready,
};

// Define a callback type for data updates
typedef void (*LiveDataCallback)(const XsDataPacket*, size_t size);

class MtApplication {
public:
  MtApplication(MtsspDriver* driver, uint8_t drdy, uint8_t resetpin);
  ~MtApplication();
  bool start();
  void readData();
  void setLiveDataCallback(LiveDataCallback callback);

private:
  void resetDevice();
  void readDataFromDevice();
  bool checkDataReadyLine();
  void handleEvent(Event event, const uint8_t* data = 0);

  State m_state;
  MtsspDriver* m_driver;
  MtsspInterface* m_device;

  uint8_t m_xbusTxBuffer[256];
  uint8_t m_dataBuffer[256];
  LiveDataCallback onLiveDataAvailable = nullptr;
  XsDataPacket m_packet;


  uint8_t m_drdy;
  uint8_t m_resetpin;
  DataParser* m_parser; //For Arduino, we must use dynamic library for this, otherwise, the prints get stuck.
  XsDataPacket* m_xspacket;
};


#endif
