# LoRa IoT Monitoring System

## Giới thiệu

Hệ thống giám sát nhiệt độ, độ ẩm và khí gas sử dụng:

- Arduino Uno R3
- ESP8266 NodeMCU
- LoRa AS32-TTL-100
- DHT11
- MQ-2
- LCD1602 I2C
- MQTT
- HiveMQ Cloud

## Kiến trúc hệ thống

Node 1:
DHT11 + LCD1602 + LoRa

Node 2:
MQ2 + LCD1602 + LED + Quạt + LoRa

Gateway:
ESP8266 + LoRa + MQTT

## Luồng dữ liệu

Node1 -> LoRa -> ESP8266 -> MQTT -> HiveMQ

Node2 -> LoRa -> ESP8266 -> MQTT -> HiveMQ

## Các file

Node1.ino
Node2.ino
gateway_mqtt.ino

## Tác giả

Ngô Anh Đức
