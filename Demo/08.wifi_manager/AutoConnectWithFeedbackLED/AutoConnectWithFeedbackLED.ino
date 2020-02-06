/**
   WiFi連線測試
   說明：相容esp8266的WiFi連線設置，並使用內建號回應狀態，
        當啟動時，燈號慢閃，進入配置模式時，燈號快閃，WiFi連線以後燈滅。
   程式庫：
          WiFiManager : 無線網路連線管理 https://github.com/tzapu/WiFiManager
          Ticker : 背景排程(定時執行)
   備註：此程式由WiFiManager函式庫的範例AutoConnectWithFeedbackLED測試更改
*/
#include <WiFiManager.h>

#include <Ticker.h>

Ticker ticker;

// 燈號轉向(亮→不亮、不亮→亮)
void tick() {
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

// 當WiFiManager進入配置模式的狀態回應
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

// 設定顯示時間同步
void setClock() {
  configTime(8 * 3600, 0, "time.google.com");  // UTC
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.println();
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);

  ticker.attach(0.6, tick);

  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(180); //等待3分鐘配置WiFi

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
  setClock();
  delay(60000);
}
