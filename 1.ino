#include <Temperature_LM75_Derived.h>
#include <DHT22.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define BMP_SCK  (5)  // SCL
#define BMP_MISO (2)  // SDO
#define BMP_MOSI (4)  // SDA
#define BMP_CS   (3)  // CSB
#define DHTPIN   6    // DHT22数据引脚
#define BLUETOOTH_TX 10
#define BLUETOOTH_RX 11

Generic_LM75 temperature;
DHT22 dht22(DHTPIN);
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX); // RX, TX

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  Wire.begin();
  
  if (!bmp.begin()) {
    Serial.println("Could not find BMP280 sensor!");
    while (1);
  }
  
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  
  delay(1000); // 等待传感器稳定
}

void loop() {
  // 读取传感器数据
  float temp_lm75 = temperature.readTemperatureC();
  float humidity = dht22.getHumidity();
  float pressure_pa = bmp.readPressure();
  float pressure_mmhg = pressure_pa * 760.0 / 101325.0;
  
  // 格式化数据，保留一位小数
  String s_tmp = String(temp_lm75, 1);
  String s_hmd = String(humidity, 1);
  String s_prs = String(pressure_mmhg, 1);

  // 获取当前时间（简化版，实际应用中可以从RTC模块获取）
  String currentTime = getFormattedTime();

  // 创建JSON格式数据
  String jsonData = "{\"temp\":\"" + s_tmp + "\",\"humid\":\"" + s_hmd + "\",\"press\":\"" + s_prs + "\",\"time\":\"" + currentTime + "\"}";

  // 输出到串口和蓝牙
  Serial.println(jsonData);
  bluetooth.println(jsonData);

  // 显示在LCD上
  updateLCD(s_tmp, s_hmd, s_prs, currentTime);

  delay(1000); // 2秒更新一次
}

String getFormattedTime() {
  // 简化版时间获取，实际项目中应使用RTC模块
  static unsigned long seconds = 0;
  seconds += 2; // 每次循环增加2秒
  
  int hours = (seconds / 3600) % 24;
  int minutes = (seconds % 3600) / 60;
  
  String timeStr = "";
  if (hours < 10) timeStr += "0";
  timeStr += String(hours) + ":";
  if (minutes < 10) timeStr += "0";
  timeStr += String(minutes);
  
  return timeStr;
}

void updateLCD(String temp, String humid, String press, String time) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp + "C  " + humid + "%");
  lcd.setCursor(0, 1);
  lcd.print(press + "mmHg " + time);
}
