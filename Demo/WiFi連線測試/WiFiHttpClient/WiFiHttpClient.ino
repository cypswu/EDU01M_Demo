/*
    HTTP連線測試
	使用HTTP通訊協定連線到www.google.com網頁，並且以Serial輸出網頁內容。
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "ssid";
const char* password = "password";

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client;

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, "http://www.google.com")) {  // HTTP

    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // 回應狀態碼200或301(轉址)
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
  
  Serial.print("[HTTP] end...\n");
  delay(60000);
}
