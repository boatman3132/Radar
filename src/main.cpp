#include <Arduino.h>
#include "initial.h"


void setup() {
    initSystem();
}

void loop() {
    buttonUp.loop();
    buttonDown.loop();

// 更新隨機心跳和呼吸率

    static unsigned long lastUpdateTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastUpdateTime >= 3000) {
        lastUpdateTime = currentTime;
        currentHeartRateBPM = static_cast<float>(random(40, 91));
        currentBreathingRateRPM = static_cast<float>(random(8, 16));
    }

    showPage(currentPage);
    server.handleClient();
}