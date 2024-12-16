#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include <Arduino.h>
#include "RadarData.h"

class UARTHandler {
public:
    UARTHandler();
    void begin();
    bool readData();
    RadarData getRadarData() const;
    int getTotalPacketsReceived() const;
    int getSuccessfulParses() const;

private:
    static const int RX_PIN = 17;
    static const int TX_PIN = 18;
    static const int BAUD_RATE = 115200;
    static const int EXPECTED_DATA_SIZE = 96;
    
    HardwareSerial uart;
    uint8_t uartBuffer[EXPECTED_DATA_SIZE];
    RadarData radarData;
    const uint8_t expectedSyncHeader[8] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};
    int totalPacketsReceived;
    int successfulParses;
};

#endif