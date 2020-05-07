/*
   I2C 通訊協定測試，掃描已連接的 I2C 設備序號
   使用 I2C 的接腳為 4(SDA) and 5(SCL)
*/
#include <Wire.h>

#define SERIAL_BAUD 115200

void setup()
{
  Wire.begin();

  Serial.begin(SERIAL_BAUD);
  Serial.println("I2C Scanner started");
  Serial.println();
}


void loop()
{
  uint8_t error, i2cAddress, devCount, unCount;

  Serial.println("Scanning...");

  devCount = 0;
  unCount = 0;
  // 搜尋 I2C 地址從 1 ~ 126
  for (i2cAddress = 1; i2cAddress < 127; i2cAddress++ )
  {
    Wire.beginTransmission(i2cAddress);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at 0x");
      if (i2cAddress < 16) Serial.print("0");
      Serial.print(i2cAddress, HEX);
      Serial.print(" for ");
      if (i2cAddress == 0x3C)
        Serial.print("OLED 0.96");
      else if (i2cAddress == 0x40)
        Serial.print("INA219");
      else if (i2cAddress == 0x48)
        Serial.print("ADS1115");
      else if (i2cAddress == 0x5C)
        Serial.print("DHT12");
      else if (i2cAddress == 0x68)
        Serial.print("MPU-6050");
      else
        Serial.print("unknow");
      Serial.println();
      devCount++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at 0x");
      if (i2cAddress < 16) Serial.print("0");
      Serial.println(i2cAddress, HEX);
      unCount++;
    }
  }
  // 顯示掃描分析結果
  if (devCount + unCount == 0)
    Serial.println("No I2C devices found\n");
  else {
    Serial.print(devCount);
    Serial.print(" device(s) found");
    if (unCount > 0) {
      Serial.print(", and unknown error in ");
      Serial.print(unCount);
      Serial.print(" address");
    }
    Serial.println();
  }
  Serial.println();
  delay(5000);
}
