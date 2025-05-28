#include <Temperature_LM75_Derived.h>
#include <DHT22.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

#define BMP_SCK  (5) //scl
#define BMP_MISO (2) //sdo
#define BMP_MOSI (4) //sda
#define BMP_CS   (3) //csb

Generic_LM75 temperature;
DHT22 dht22(6);
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  while(!Serial) delay(1000);
  Serial.begin(9600);
  Wire.begin();
  bmp.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  String s_tmp = String(round(temperature.readTemperatureC() * 10) / 10.);
  String s_hmd = String(round(dht22.getHumidity() * 10) / 10.);
  String s_prs = String(round(bmp.readPressure()*760/101325 * 10) / 10.);

  s_tmp = s_tmp.substring(0, s_tmp.length() - 1);
  s_hmd = s_hmd.substring(0, s_hmd.length() - 1);
  s_prs = s_prs.substring(0, s_prs.length() - 1);

  String data = "{'temp': '" + s_tmp + "', 'humid': '" + s_hmd + "', 'press': '" + s_prs + "'}";

  Serial.println(data);
  lcd.setCursor(0, 0);
  lcd.print(s_tmp + "C  " + s_hmd + "%  ");
  lcd.setCursor(0, 1);
  lcd.print(s_prs + "mmHg  " + "HH" + ":" + "MM");

  delay(1000);
}
