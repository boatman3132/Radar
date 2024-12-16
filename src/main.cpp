#include <Arduino.h>
#include "initial.h"
#include "UARTHandler.h"

UARTHandler uartHandler;

void setup() {
    initSystem();
    uartHandler.begin();
}

void loop() {
    buttonUp.loop();
    buttonDown.loop();
    static unsigned long lastReadTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastReadTime >= 180) {
        lastReadTime = currentTime;
        
        if (uartHandler.readData()) {
            RadarData radarData = uartHandler.getRadarData();
            currentHeartRateBPM = radarData.heartRateEst;
            currentBreathingRateRPM = radarData.breathingRateEst;

            Serial.print("心跳率: ");
            Serial.print(currentHeartRateBPM);
            Serial.print("  呼吸率: ");
            Serial.println(currentBreathingRateRPM);
        }
    }

    showPage(currentPage);
    server.handleClient();
}