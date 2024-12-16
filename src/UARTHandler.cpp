#include "UARTHandler.h"

UARTHandler::UARTHandler() : uart(1), totalPacketsReceived(0), successfulParses(0) {
}

void UARTHandler::begin() {
    uart.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
}

bool UARTHandler::readData() {
    totalPacketsReceived++;
    
    for (int j = 0; j < ((EXPECTED_DATA_SIZE-8)/8); j++) {
        static int syncPos = 0;
        uart.readBytes(uartBuffer, 8);
        
        for (int i = 0; i < 8; i++) {
            if (uartBuffer[i] == expectedSyncHeader[syncPos]) {
                syncPos++;
                if (syncPos == 8) {
                    if (uart.available() >= EXPECTED_DATA_SIZE-8) {
                        uart.readBytes(&uartBuffer[8], EXPECTED_DATA_SIZE-8);
                        memcpy(&radarData, &uartBuffer[0], sizeof(RadarData));
                        successfulParses++;
                        syncPos = 0;
                        return true;
                    }
                }
            } else {
                syncPos = 0;
                break;
            }
        }
    }
    return false;
}

RadarData UARTHandler::getRadarData() const {
    return radarData;
}

int UARTHandler::getTotalPacketsReceived() const {
    return totalPacketsReceived;
}

int UARTHandler::getSuccessfulParses() const {
    return successfulParses;
}