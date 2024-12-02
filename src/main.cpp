#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "website.h" // 引入 website.h

// 替換為你的 Wi-Fi 網路名稱與密碼
const char* ssid = "diningroom";
const char* password = "thisissamuelh";

WebServer server(80); // 設置Web服務器在端口80

void setup() {
  Serial.begin(115200);

  // 等待串口監視器準備好
  while (!Serial) {
    ; // 等待
  }

  Serial.println();
  Serial.println("Serial Monitor 初始化完成");

  // 初始化隨機數生成器
  randomSeed(analogRead(0));

  // 連接Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("WiFi 連接失敗");
  }

  setupServer(server); // 設置 Web 服務器的路由

  server.begin();
  Serial.println("HTTP 服務器已啟動");
}

void loop() {
  server.handleClient(); // 處理客戶端請求
}