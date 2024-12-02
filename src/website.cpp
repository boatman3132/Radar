#include <Arduino.h>
#include <WebServer.h>
#include "website.h"

extern float currentHeartRateBPM;  // 引用 main.cpp 中的變量
extern float currentBreathingRateRPM;  // 引用 main.cpp 中的變量


void handleRoot(WebServer &server) {
    String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    html += "<script>";
    html += "function fetchHealthData() {";
    html += "  fetch('/healthData').then(response => response.text()).then(data => {";
    html += "    document.getElementById('healthData').innerHTML = data;";
    html += "  });";
    html += "}";
    html += "window.onload = function() { setInterval(fetchHealthData, 1000); };"; // 每秒刷新一次
    html += "</script>";
    html += "</head><body><h1>及時數據</h1>";
    html += "<div id=\"healthData\"></div>"; // 顯示心率和呼吸率的區域
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setupServer(WebServer &server) {
    server.on("/", [&server]() { handleRoot(server); });

    // 在網頁端顯示心率和呼吸率
    server.on("/healthData", HTTP_GET, [&server]() {


        String message = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"></head><body>";
        message += "<h1>心率: ";
        message += String(static_cast<int>(currentHeartRateBPM)); // 將心率取整
        message += " BPM</h1>";
        message += "<h1>呼吸率: ";
        message += String(static_cast<int>(currentBreathingRateRPM)); // 將呼吸率取整
        message += " RPM</h1>";
        message += "</body></html>"; 
        server.send(200, "text/html", message);


    });
}