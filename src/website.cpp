#include <Arduino.h>
#include <WebServer.h>
#include "website.h"
#include "page2.h"
#include <FS.h>

extern float currentHeartRateBPM;  // 引用 main.cpp 中的變量
extern float currentBreathingRateRPM;  // 引用 main.cpp 中的變量



void handleRoot(WebServer &server) {
    String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    html += "<script>";
    html += "function fetchHealthData() {";
    html += "  fetch('/healthData').then(response => response.json()).then(data => {";
    html += "    document.getElementById('heartRate').innerText = data.heartRate + ' BPM';";
    html += "    document.getElementById('breathingRate').innerText = data.breathingRate + ' RPM';";
    html += "    updateWaveform(data.heartWave, data.breathWave);";
    html += "  });";
    html += "}";
    
    html += "function updateWaveform(heartWave, breathWave) {";
    html += "  var canvas = document.getElementById('waveformCanvas');";
    html += "  var ctx = canvas.getContext('2d');";
    html += "  ctx.fillStyle = 'black';";  // 設置背景顏色為黑色
    html += "  ctx.fillRect(0, 0, canvas.width, canvas.height);";  // 填充背景
    html += "  ctx.strokeStyle = 'green';";
    html += "  ctx.beginPath();";
    html += "  var offset = 1;";  // 定義偏移量
    html += "  var amplitudeFactor = 10.0;";  // 增加振幅放大係數
    html += "  for (var i = offset; i < heartWave.length; i++) {";  // 從偏移量開始繪製
    html += "    var x = (i - offset) * (canvas.width / (heartWave.length - offset));";
    html += "    var y = canvas.height / 2 - heartWave[i] * amplitudeFactor * 3 ;";  // 增加振幅
    html += "    if (i === offset) ctx.moveTo(x, y); else ctx.lineTo(x, y);";
    html += "  }";
    html += "  ctx.stroke();";
    html += "  ctx.strokeStyle = 'yellow';";
    html += "  ctx.beginPath();";
    html += "  for (var i = offset; i < breathWave.length; i++) {";  // 同樣從偏移量開始繪製
    html += "    var x = (i - offset) * (canvas.width / (breathWave.length - offset));";
    html += "    var y = canvas.height / 2 - breathWave[i] * amplitudeFactor / 4;";  // 增加振幅
    html += "    if (i === offset) ctx.moveTo(x, y); else ctx.lineTo(x, y);";
    html += "  }";
    html += "  ctx.stroke();";
    html += "}";

    html += "window.onload = function() { setInterval(fetchHealthData, 30); };";
    html += "</script>";
    html += "</head><body><h1>及時數據</h1>";
    html += "<div>心率: <span id=\"heartRate\"></span></div>";
    html += "<div>呼吸率: <span id=\"breathingRate\"></span></div>";
    html += "<canvas id=\"waveformCanvas\" width=\"400\" height=\"200\"></canvas>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}


void setupServer(WebServer &server) {
    server.on("/", [&server]() { handleRoot(server); });

    server.on("/healthData", HTTP_GET, [&server]() {
        String message = "{";
        message += "\"heartRate\":" + String(static_cast<int>(currentHeartRateBPM)) + ",";
        message += "\"breathingRate\":" + String(static_cast<int>(currentBreathingRateRPM)) + ",";
        message += "\"heartWave\":[";  // 添加心跳波形數據
        for (int i = 0; i < 200; i++) {
            message += String(heartData[i]);
            if (i < 199) message += ",";
        }
        message += "],";
        message += "\"breathWave\":[";  // 添加呼吸波形數據
        for (int i = 0; i < 200; i++) {
            message += String(breathData[i]);
            if (i < 199) message += ",";
        }
        message += "]";
        message += "}";
        server.send(200, "application/json", message);
    });
}