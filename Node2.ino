#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MQ2_PIN A0
#define LED_PIN 3
#define FAN_PIN 4

#define MD0 8
#define MD1 9

#define GAS_LIMIT 400

SoftwareSerial loraSerial(10, 11);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  loraSerial.begin(9600);

  pinMode(MD0, OUTPUT);
  pinMode(MD1, OUTPUT);

  digitalWrite(MD0, LOW);
  digitalWrite(MD1, LOW);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Node 2 Ready");

  Serial.println("Node 2 Ready");
}

void loop()
{
  int gasValue = analogRead(MQ2_PIN);

  bool overLimit = false;

  if (gasValue > GAS_LIMIT)
  {
    overLimit = true;

    digitalWrite(LED_PIN, HIGH);
    digitalWrite(FAN_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
  }

  // ===== LCD =====
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Gas:");
  lcd.print(gasValue);

  lcd.setCursor(0, 1);

  if (overLimit)
  {
    lcd.print("WARNING !");
  }
  else
  {
    lcd.print("NORMAL");
  }

  // ===== JSON =====
  DynamicJsonDocument doc(192);

  doc["id"] = "node2";
  doc["gas"] = gasValue;

  if (overLimit)
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