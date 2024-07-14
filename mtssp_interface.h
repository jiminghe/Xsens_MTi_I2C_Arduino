#ifndef MTSSP_INTERFACE_H
#define MTSSP_INTERFACE_H

#include "mtssp_driver.h"


#define DRDY_CONFIG_MEVENT_POS 3  //Measurement pipe DataReady event enable: 0 = Disabled, 1 = Enabled
#define DRDY_CONFIG_NEVENT_POS 2  //Notification pipe DataReady event enable: 0 = Disabled, 1 = Enabled
#define DRDY_CONFIG_OTYPE_POS 1   //Output type DataReady pin: 0 = Push/pull, 1 = Open drain
#define DRDY_CONFIG_POL_POS 0     //Polarity DataReady pin: 0 = Idle low, 1 = Idle high


class MtsspInterface {
public:
  MtsspInterface(MtsspDriver* driver);

  void readProtocolInfo(uint8_t& version, uint8_t& dataReadyConfig);
  void configureProtocol(uint8_t dataReadyConfig);
  void readPipeStatus(uint16_t& notificationMessageSize, uint16_t& measurementMessageSize);
  void readFromPipe(uint8_t* buffer, uint16_t size, uint8_t pipe);
  void sendXbusMessage(uint8_t const* xbusMessage);

private:
  MtsspDriver* m_driver;
};



#endif
