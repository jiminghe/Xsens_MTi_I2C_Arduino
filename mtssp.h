#ifndef MTSSP_H
#define MTSSP_H


#define XBUS_PROTOCOL_INFO (0x01)
#define XBUS_CONFIGURE_PROTOCOL (0x02)
#define XBUS_CONTROL_PIPE (0x03)
#define XBUS_PIPE_STATUS (0x04)
#define XBUS_NOTIFICATION_PIPE (0x05)
#define XBUS_MEASUREMENT_PIPE (0x06)



enum XbusBusFormat {
  XBF_I2c,
  XBF_Spi,
  XBF_Uart
};



#endif
