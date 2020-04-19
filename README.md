# EWCIoT EDU01M 開發版 #

----------

## 什麼是EDU01M開發版 ##
EDU01M開發版是一塊可以使用ARDUINO IDE開發工具，進行物聯網二次開發的一塊學習板，板上內建了的功能包含蜂鳴器模組、繼電器開關、按鈕開關、紅外線接收模組、紅外線發射模組、DHT12溫濕度感測；在開發板的接口有預留OLED顯示屏幕、人體紅外線感應模組、MQ2等相關氣體感測模組、兩組比較器輸入接口、ADS1115的16bit類比輸入感測模組；在電源的輸入方面，除了燒錄的USB電源接口，還可以使用24V以下變壓器輸入、太陽能電池輸入、單顆18650電池輸入等等，此開發板使用ESP8285 MCU，開發上相容ESP8266，可做WiFi等物聯網程式開發應用。

## EDU01M如何開發 ##
開發過程首先需要在電腦下載並安裝ARDUINO IDE開發工具，然後下載ESP8266開發庫，再連接Micro USB線到電腦，確認電腦系統正確驅動USB，取得正確COM Port號碼，接著就可以進行開發作業，在開發完成要上傳到開發板時，須將P0連接GND並按放重啟鈕，進入燒錄模式，燒錄完成後，會自動進入執行模式，開發需要用到的模組程式庫，已經收錄在libraries目錄中，以下列就依序列出開發過程，可能需要的相關連結，希望對您的物聯網開發有些許的幫助。

- ARDUINO IDE開發工具<br>
[https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software "https://www.arduino.cc/en/Main/Software")

- ESP8266開發庫下載連結設置<br>
https://arduino.esp8266.com/stable/package_esp8266com_index.json <br>
設置可以參考 [ESP8266 for Arduino IDE](https://github.com/esp8266/Arduino) 官網

- CH340 USB轉TTL驅動<br>

- 開發板相關模組程式庫<br>
[ADS1115 類比輸入](https://github.com/adafruit/Adafruit_ADS1X15)<br>
[SSD1306 OLED顯示屏幕](https://github.com/adafruit/Adafruit_SSD1306)<br>
[Arduino JSON 操作](https://github.com/bblanchon/ArduinoJson)<br>
[aRest 框架](https://github.com/marcoschwartz/aREST)<br>
[DHT12 溫濕度感測](https://github.com/xreef/DHT12_sensor_library)<br>
[紅外線輸入與輸出](https://github.com/cypswu/EWC_IR)<br>
[MQTT 通訊](https://pubsubclient.knolleary.net/)<br>
[LINE 通知](https://github.com/TridentTD/TridentTD_LineNotify)<br>
[WiFi 管理](https://github.com/tzapu/WiFiManager)<br>
[ESP 上傳工具](http://wiki.ai-thinker.com/tools)<br>

- [Arduino語法參考：https://www.arduino.cc/reference/en/](https://www.arduino.cc/reference/en/ "Arduino語法參考")

- [ESP8266參考手冊：https://arduino-esp8266.readthedocs.io/en/latest/](https://arduino-esp8266.readthedocs.io/en/latest/ "ESP8266參考手冊")

- [EDU01M開發版參考：https://github.com/cypswu/EDU01M_Demo](https://github.com/cypswu/EDU01M_Demo "EDU01M開發版參考")


- 整合測試
如果想要直接使用，可以參考[**"Demo/整合測試"**](https://github.com/cypswu/EDU01M_Demo/tree/master/Demo/%E6%95%B4%E5%90%88%E6%B8%AC%E8%A9%A6)的使用說明。

----------

EDU01M開發板
![alt text](img/EDU01M_01.JPG "EDU01M 開發版")

安裝 Arduino IDE，燒錄時P0接地，選擇燒錄配置如下。<br>
![alt text](img/Tools_options.png "EDU01M 開發版")