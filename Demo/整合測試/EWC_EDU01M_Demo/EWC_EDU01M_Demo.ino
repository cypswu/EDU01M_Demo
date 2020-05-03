/*
   EWCIoT EDU01M ESP01M 教學開發板 綜合測試
   範例說明： WiFi連線網際網路，使用LINE通知並以MQTT做遠程控制

   燈號： 開機：快閃、WiFi連線完成：慢閃、正常啟動完成：燈滅，感測到狀態:亮燈。
   模組： 蜂鳴器、DHT12、紅外線接收、紅外線發射、按鈕、人體感測、MQ2感測、ADS1115類比輸入、OLED 128*64。
   功能： 偵測異常的通知包含有人體感測、氣體感測、電壓偵測、溫度偵測，開關按住三秒進入紅外線學習模式。
   COM Port命令： reboot->重開機。

   編譯版本： ESP8266 2.6.3
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <aREST.h>
#include <TridentTD_LineNotify.h>
#include <DHT12.h>
#include <Adafruit_ADS1015.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <EWC_IR.h>

//OLED螢幕配置
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT  64 // OLED display height, in pixels
#define OLED_RESET     12 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//無線AP帳號與LINE設置
const char* ssid     = "SSID";
const char* password = "PASSWORD";
const String LINE_TOKEN = "LINE_TOKEN";  //如果沒有申請LINE_TOKEN字串，可以設定為""空字串

//PIN腳設置
const int beepPin = 9;    //蜂鳴器輸出
const int relayPin = 10;  //繼電器輸出PIN腳
const int buttonPin = 12; //板載按鈕
const int huirPin = 13;   //HUIR感測器
const int irRecPin = 14;  //紅外線接收
const int irSndPin = 15;  //紅外線發射
const int mqxPin = 16;    //MQx感測器

//模組配置宣告
DHT12 dht12;
Adafruit_ADS1115 ads;
Ticker ticker;
EWC_IR ir(irRecPin, irSndPin, beepPin);
WiFiClient espClient;
PubSubClient client(espClient);  //建立PubSubClient客戶端
aREST rest = aREST(client);      //建立aREST客戶端

//EDU01M系統配置
const char CTRL_URL[]   = "https://cypswu.gitlab.io/ewc_iotctl/EWC_Demo01.html?TOPIC=";  // 遠端控制URL

//全域變數宣告
String device_name = String("EWC_") + String(ESP.getChipId(), HEX);  //裝置顯示名稱
String mqtt = "test.mosquitto.org";  //預設的MQTT server
String device_id;         //設定裝置ID(aRest名稱)
String TOPIC = "";
String event_pin = "9";   //事件驅動PIN(感測器觸發蜂鳴器)
float tem12;       //溫度
float hum12;       //濕度
float fa1, fa2;    //電壓
int16_t adc1, adc2;//類比輸入
byte irnum = 1;    //預設紅外線學習序號

void setup() {
  Serial.begin(115200);
  Serial.println();
  //PIN腳初始化
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(beepPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  //開關使用內部上拉電阻
  pinMode(huirPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(mqxPin, INPUT);
  //WiFi連線
  ticker.attach(0.2, tickLed);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //初始化溫濕度與紅外線感測器
  ticker.attach(0.6, tickLed);
  dht12.begin();
  ir.begin();
  //初始化OLED顯示
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  //初始化電壓測量範圍
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
  ads.begin();
  //感測器數據初始化
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  digitalWrite(relayPin, LOW);
  //初始化MQTT連線與aRest服務(先更新感測器數據)
  reSensor();
  init_aRest();
  //設備初始化完成的LINE通知
  initLineNotif();
  notifActionList("初始化完成");
}

void loop() {
  rest.handle(client);  //不斷處理雲端訊息
  reSensor();           //排程執行狀態更新
  momitorStatus();      //監測設備狀態數據
  checkReboot();        //重開機命令(用於EDU01M要重新燒錄前)
}

/********************************************************************************
   檢查是否接收Serial重新開機命令
*/
void checkReboot() {
  if (Serial.available()) {  //重新開機命令
    String in = Serial.readString();
    Serial.println();
    Serial.println(in);
    if (in == "reboot\n") {
      ESP.reset();
    }
  }
}

/********************************************************************************
   NTP時間同步與顯示
*/
void setClock() {
  if (WiFi.status() != WL_CONNECTED) return ;
  int timezone = 8;  //定義時區(+8)
  int dst = 0;       //定義日光節約時間
  configTime(timezone * 3600, dst, "time.stdtime.gov.tw");  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < timezone * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }

  Serial.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/********************************************************************************
   LED閃燈處理
*/
void tickLed() {
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

/********************************************************************************
   更新感測器數據
*/
void reSensor() {
  static unsigned long pret = millis() - 1000UL;  //執行後起算
  if (! isTimeStamp(pret, 1000)) return; //間隔定時執行
  reDHT12();
  reAds1115();
  if (checkDHT12()) {
    notifActionList("溫度異常盡快檢查");
  }
  String msg = String("DHT12 Temp\n") + tem12 + " C\nLIB " + fa2 + " V";
  showSSD1306(msg);
}

/********************************************************************************
   處理雲端請求及回應
*/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("callback: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  rest.handle_callback(client, topic, payload, length);
}

/********************************************************************************
   初始化 aRest 服務
*/
void init_aRest() {
  client.setServer(mqtt.c_str(), 1883);
  device_id = String(ESP.getChipId(), HEX);
  client.setCallback(callback);  //處理雲端訊息
  rest.variable("temperature", &tem12);  //溫度參數定義
  rest.function("irCtrl", restControl);  //aRrest Function定義
  rest.set_id(device_id.c_str());  //建立裝置名稱
  rest.set_name("edu01m");  //設定連線名稱
  Serial.print("Device ID: ");
  Serial.println(device_id);
  //取得MQTT的TOPIC參數
  char* out_topic = rest.get_topic();
  TOPIC = String(out_topic);
  TOPIC.replace("_out", "");
  Serial.println ("TOPIC: " + TOPIC);
}

/********************************************************************************
   aRrest Function 控制呼叫
*/
int restControl(String cmd) {
  String msg;
  Serial.print("IR控制:");
  Serial.println(cmd);
  if (cmd.substring(0, 6) == "set_ir") {
    String setIr = String("ir") + irnum;
    if (ir.saveScanRaw(setIr)) {
      msg = setIr + " 設置完成";
    } else {
      msg = setIr + " 設置失敗";
    }
  } else if (cmd.substring(0, 3) == "ir1") {
    irnum = 1;
    if (ir.sendIrRaw("ir1")) {
      msg = "IR1 發送完成";
    } else {
      msg = "IR1 發送失敗";
    }
  } else if (cmd.substring(0, 3) == "ir2") {
    irnum = 2;
    if (ir.sendIrRaw("ir2")) {
      msg = "IR2 發送完成";
    } else {
      msg = "IR2 發送失敗";
    }
  } else {
    msg = "命令錯誤";
  }
  notifActionList(msg);
  return 7;  //aRest回應狀態(自定義)
}

/********************************************************************************
   設備通知初始化LINE
*/
void initLineNotif() {
  Serial.println(String(CTRL_URL) + TOPIC);
  if (LINE_TOKEN.length() == 0) return;
  LINE.setToken(LINE_TOKEN);
  LINE.notifySticker("設備啟動完成： https://www.ewciot.com", 2, 512);
}

/********************************************************************************
   遠程控制功能列表
*/
void notifActionList(String txt) {
  String msg = "";
  msg = txt + "\n目前溫度 " + tem12 + " ℃  濕度 " + hum12 + " %";
  msg = msg + "\n鋰電電壓 " + fa2 + "V";
  msg = msg + "\n<<遠程控制列表-" + device_name + ">>\n" +
        "遠端控制： " + FPSTR(CTRL_URL) + TOPIC + "\n";
  Serial.println(txt);
  if (LINE_TOKEN.length() != 0) {
    LINE.notify(msg);
    delay(100);
  }
  showSSD1306();
}

/********************************************************************************
   監測設備狀態
*/
void momitorStatus() {
  static unsigned long pret = millis();
  if (! isTimeStamp(pret, 300)) return; //間隔定時執行
  static bool prel = false;
  static bool ppir = false;
  static bool pmqx = false;
  String msg;
  bool crel = getRelay();  //繼電器狀態
  bool cpir = getPIR();    //人體感測狀態
  bool cmqx = getMQX();    //氣體感測狀態
  bool alarm = false;      //警訊通知狀態
  if (cpir) alarm = true;
  if (cmqx) alarm = true;
  if (checkDHT12()) alarm = true;
  if (alarm) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(event_pin.toInt(), HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(event_pin.toInt(), LOW);
  }
  //繼電器轉換通知
  if (prel != crel) {
    prel = crel;
    setClock();
    if (crel) {
      msg = "Relay 已啟動";
    } else {
      msg = "Relay 已關閉";
    }
    notifActionList(msg);
  }
  //人體偵測通知
  if (ppir != cpir) {
    ppir = cpir;
    if (cpir) {
      setClock();
      showSSD1306("PIR alram !");
      msg = "PIR 人體移動感測警訊";
      notifActionList(msg);
    } else {
      showSSD1306();
    }
  }
  //氣體偵測通知
  if (pmqx != cmqx) {
    pmqx = cmqx;
    if (cmqx) {
      setClock();
      showSSD1306("MQ2 alram !");
      msg = "MQ2 氣體煙霧感測警訊";
      notifActionList(msg);
    } else {
      showSSD1306();
    }
  }
  //電壓偵測通知
  if (checkBattery()) {
    setClock();
    msg = "電池即將耗盡，請盡快充電";
    notifActionList(msg);
  }
  //開關狀態是否短按或長按
  for (int i = 0; true; i++) {
    bool bin = digitalRead(buttonPin);
    if (bin == LOW) {
      if (i == 99) {
        restControl("set_ir");
      }
    } else {
      if (i > 3 && i < 99) {
        restControl(String("ir") + irnum);
      }
      break;
    }
    delay(30);
  }
}

/********************************************************************************
   顯示 SSD1306 OLED 畫面訊息
*/
void showSSD1306(void) {
  String msg = String("DHT12 Temp\n") + tem12 + " C\nLIB " + fa2 + "V";
  showSSD1306(msg);
}
void showSSD1306(String msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(msg);

  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  display.setTextSize(1);
  display.print(asctime(&timeinfo));

  display.display();
  delay(100);
}

/********************************************************************************
   取得 Relay 繼電器當前狀態
*/
bool getRelay() {
  bool relay = digitalRead(relayPin);
  if (relay == HIGH) {
    return true;
  } else {
    return false;
  }
}

/********************************************************************************
   偵測 HUIR 是否發生警訊(HIGH) PIR 人體移動感測警訊
*/
bool getPIR() {
  bool huir = digitalRead(huirPin);
  if (huir == HIGH) {
    return true;
  } else {
    return false;
  }
}

/********************************************************************************
   偵測 MQx 是否發生警訊(HIGH) MQ2 氣體煙霧感測警訊
*/
bool getMQX() {
  bool mqx = digitalRead(mqxPin);
  if (mqx == LOW) {
    return true;
  } else {
    return false;
  }
}

/********************************************************************************
   偵測溫度是否太高
*/
bool checkDHT12() {
  if (tem12 >= 45.0) {
    return true;
  } else {
    return false;
  }
}

/********************************************************************************
   偵測電池是否還有電(小於3.5)
*/
bool checkBattery() {
  static float fax = 0;
  bool alarm = false;
  const float diff = 0.02;
  float x;
  if (fax > fa2)
    x = fax - fa2;
  else
    x = fa2 - fax;
  if (x > diff ) {
    fax = fa2;
    if (fax <= 3.50) {
      Serial.println(String("電池即將耗盡，請盡快充電 ") + fax + "V");
      alarm = true;
    }
    if (fax <= 3.2) {
      //ESP.deepSleep(100000000);  //正式使用可啟動休眠100秒(必須RST接IO16)
    }
  }
  return alarm;
}

/********************************************************************************
  更新 DHT12 溫濕度數據
*/
void reDHT12() {
  tem12 = dht12.readTemperature();
  hum12 = dht12.readHumidity();
  Serial.print("DHT12 => Humidity: ");
  Serial.print(hum12);
  Serial.print(" % \t");
  Serial.print("Temperature: ");
  Serial.print(tem12);
  Serial.println(" *C ");
}

/********************************************************************************
  更新 ADS1115 16bit電壓測量數據
*/
void reAds1115() {
  adc1 = ads.readADC_SingleEnded(1);
  fa1 = adc1 * 0.1875 / 1000 / 1.034 * 20;
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("\t\t"); Serial.print(fa1); Serial.println("V");
  adc2 = ads.readADC_SingleEnded(2);
  fa2 = adc2 * 0.1875 / 1000;
  Serial.print("AIN2: "); Serial.print(adc2); Serial.print("\t\t"); Serial.print(fa2); Serial.println("V");
}

/********************************************************************************
  時間間隔是否大於(PTime與目前時間間隔是否大於Stamp) 單位毫秒
*/
bool isTimeStamp(unsigned long &ptime, int stamp) {
  unsigned long nowTime;   //當前時間
  unsigned long stmpTime;  //間隔時間
  nowTime = millis();
  if (ptime > nowTime)
    stmpTime = ULONG_MAX - ptime + nowTime + 1UL;  // 加 1 是因為從0起算
  else
    stmpTime = nowTime - ptime;
  if (stmpTime >= stamp) {
    ptime = nowTime;
    return true;
  } else {
    return false;
  }
}
