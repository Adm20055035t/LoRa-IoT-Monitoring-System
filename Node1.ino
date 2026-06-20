#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define MD0 8
#define MD1 9

// ===== Ngưỡng cảnh báo =====
#define TEMP_LIMIT 35
#define HUM_LIMIT 85

SoftwareSerial loraSerial(10, 11);

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(9600);
  loraSerial.begin(9600);

  pinMode(MD0, OUTPUT);
  pinMode(MD1, OUTPUT);

  digitalWrite(MD0, LOW);
  digitalWrite(MD1, LOW);

  dht.begin();

  lcd.init();
  lcd.backlight();

  Serial.println("Node 1 Ready");
}

void loop() {

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  // ===== KIỂM TRA DHT11 =====
  if (isnan(temp) || isnan(hum)) {

    Serial.println("Loi DHT11");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT11 ERROR");

    delay(2000);
    return;
  }

  // ===== LCD =====
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temp, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum:");
  lcd.print(hum, 0);
  lcd.print("%");

  // ===== JSON =====
  DynamicJsonDocument doc(192);

  doc["id"] = "node1";
  doc["temp"] = temp;
  doc["hum"] = hum;

  // ===== KIỂM TRA NGƯỠNG =====
  if (temp > TEMP_LIMIT || hum > HUM_LIMIT)
  {
    doc["status"] = "OVER_LIMIT";
  }
  else
  {
    doc["status"] = "NORMAL";
  }

  String data;

  serializeJson(doc, data);

  loraSerial.println(data);

  Serial.println("Da gui:");
  Serial.println(data);

  delay(5000);
}