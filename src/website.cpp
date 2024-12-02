#include <Arduino.h>
#include <WebServer.h>
#include "website.h"
void handleRoot(WebServer &server) {
    String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    html += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>"; // 引入 Chart.js
    html += "<script>";
    html += "let randomNumbers = [];"; // 儲存歷史亂數
    html += "let chart;"; // 儲存圖表實例
    html += "function fetchRandomNumber() {";
    html += "  fetch('/randomNumber').then(response => response.text()).then(data => {";
    html += "    document.getElementById('randomNumber').innerHTML = data;";
    html += "    randomNumbers.push(parseInt(data));"; // 將新數據加入陣列
    html += "    if (randomNumbers.length > 10) randomNumbers.shift();"; // 保持陣列大小
    html += "    updateChart();"; // 更新圖表
    html += "  });";
    html += "}";
    html += "function createChart() {";
    html += "  const ctx = document.getElementById('randomChart').getContext('2d');";
    html += "  chart = new Chart(ctx, {";
    html += "    type: 'line',";
    html += "    data: {";
    html += "      labels: Array.from({length: randomNumbers.length}, (_, i) => i + 1),";
    html += "      datasets: [{";
    html += "        label: '歷史亂數',";
    html += "        data: randomNumbers,";
    html += "        borderColor: 'rgba(75, 192, 192, 1)',";
    html += "        borderWidth: 1";
    html += "      }]",
    html += "    },";
    html += "    options: { scales: { y: { beginAtZero: true } } }";
    html += "  });";
    html += "}";
    html += "function updateChart() {";
    html += "  chart.data.labels = Array.from({length: randomNumbers.length}, (_, i) => i + 1);";
    html += "  chart.data.datasets[0].data = randomNumbers;";
    html += "  chart.update();";
    html += "}";
    html += "window.onload = function() { createChart(); setInterval(fetchRandomNumber, 1000); };"; // 初始化圖表並每秒刷新一次
    html += "</script>";
    html += "</head><body><h1>及時數據</h1>";
    html += "<div id=\"randomNumber\"></div>"; // 顯示亂數的區域
    html += "<canvas id=\"randomChart\" width=\"400\" height=\"200\"></canvas>"; // 圖表顯示區域
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setupServer(WebServer &server) {
    server.on("/", [&server]() { handleRoot(server); });

    // 在網頁端顯示亂數
    server.on("/randomNumber", HTTP_GET, [&server]() {
        int randomNumber = random(1, 11); // 在每次請求時生成1到10的亂數
        Serial.println(randomNumber); // 將亂數輸出到 Serial Monitor
        String message = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"></head><body>";
        message += "<h1>隨機數字: ";
        message += randomNumber;
        message += "</h1></body></html>"; 
        server.send(200, "text/html", message);
    });
}