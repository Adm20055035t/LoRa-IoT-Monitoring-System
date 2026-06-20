#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "Ngo Anh"; //đây là tên mạng
const char* password = "23455432";  //đây là mật khẩu mạng

// =====================================================
// HIVEMQ CLOUD
// =====================================================

const char* mqtt_server =
"70050b0222b3458097e8d65230eee13f.s1.eu.hivemq.cloud";

const int mqtt_port = 8883;

const char* mqtt_username = "esp8266";
const char* mqtt_password = "Adm20055035t";

// =====================================================
// LORA
// D5 = RX
// D6 = TX
// =====================================================

SoftwareSerial loraSerial(D5, D6);

WiFiClientSecure espClient;
PubSubClient client(espClient);

// =====================================================

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// =====================================================

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting MQTT...");

    String clientID = "ESP8266Gateway-";
    clientID += String(random(0xffff), HEX);

    if (client.connect(
          clientID.c_str(),
          mqtt_username,
          mqtt_password))
    {
      Serial.println("MQTT Connected");
    }
    else
    {
      Serial.print("Failed rc=");
      Serial.println(client.state());

      delay(5000);
    }
  }
}

// =====================================================

void publishAlert(char* mqtt_message)
{
  client.publish(
    "esp8266/alert",
    mqtt_message,
    true
  );

  Serial.println("ALERT PUBLISHED");
}

// =====================================================

void setup()
{
  Serial.begin(9600);

  loraSerial.begin(9600);

  setup_wifi();

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP8266 LORA GATEWAY READY");
  Serial.println("================================");
}

// =====================================================

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  // =================================================
  // RECEIVE LORA
  // =================================================

  if (loraSerial.available())
  {
    String msg = "";

    while (loraSerial.available())
    {
      char c = loraSerial.read();

      if (c != '\n' && c != '\r')
      {
        msg += c;
      }

      delay(2);
    }

    Serial.println();
    Serial.println("LoRa Received:");
    Serial.println(msg);

    // =============================================
    // PARSE JSON
    // =============================================

    DynamicJsonDocument doc(256);

    DeserializationError error =
      deserializeJson(doc, msg);

    if (error)
    {
      Serial.print("JSON ERROR: ");
      Serial.println(error.c_str());

      return;
    }

    // =============================================
    // COMMON DATA
    // =============================================

    String nodeID = doc["id"];
    String status = doc["status"];

    char mqtt_message[256];

    serializeJson(doc, mqtt_message);

    // =============================================
    // NODE 1
    // =============================================

    if (nodeID == "node1")
    {
      float temp = doc["temp"];
      float hum  = doc["hum"];

      Serial.println("----- NODE 1 -----");

      Serial.print("Temperature: ");
      Serial.println(temp);

      Serial.print("Humidity: ");
      Serial.println(hum);

      Serial.print("Status: ");
      Serial.println(status);

      client.publish(
        "esp8266/temp",
        mqtt_message,
        true
      );

      Serial.println("Published -> esp8266/temp");

      if (status == "OVER_LIMIT")
      {
        Serial.println("*** WARNING NODE1 ***");

        publishAlert(mqtt_message);
      }
    }

    // =============================================
    // NODE 2
    // =============================================

    else if (nodeID == "node2")
    {
      int gas = doc["gas"];

      Serial.println("----- NODE 2 -----");

      Serial.print("Gas Value: ");
      Serial.println(gas);

      Serial.print("Status: ");
      Serial.println(status);

      client.publish(
        "esp8266/gas",
        mqtt_message,
        true
      );

      Serial.println("Published -> esp8266/gas");

      if (status == "OVER_LIMIT")
      {
        Serial.println("*** GAS WARNING ***");

        publishAlert(mqtt_message);
      }
    }

    // =============================================
    // UNKNOWN NODE
    // =============================================

    else
    {
      Serial.println("Unknown Node");
    }
  }
}
