#include <Arduino.h>
#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "shondaime";
const char* password = "244466666";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* spray_topic = "remote-spray-alif";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

bool isSpraying = false;
void doSpray() {
  if (!isSpraying) {
    Serial.println("spraying");
    isSpraying = true;
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    delay(1000);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    isSpraying = false;
    Serial.println("stop");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    doSpray();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // subscribe to spray
      client.subscribe(spray_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600); // start boudrate at 115200 Hz
  Serial.println("--------------------------");
  Serial.println("Remote Spray - Muhammad Alif Akbar");
  Serial.println("--------------------------");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}