#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "ESP32Client-"; /* MQTT client ID (will add random hex suffix during setup) */

const char* WIFI_NETWORK = "Commodore64";
const char* WIFI_PASSWORD = "ma0s#verSuch";
const char* MQTT_SERVER = "192.168.178.38";
const int MQTT_PORT = 1883;
const uint32_t WIFI_TIMEOUT_MS = 20000;
unsigned long lastMsg = 0;

// LED GPIO port on our ESP32 DEVKITV1 board
const int LedPin = 2;

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  uint32_t startAttemptTime = millis();

  while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
      digitalWrite(LedPin, HIGH);
      delay(100);
      digitalWrite(LedPin, LOW);
      delay(100);
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println(" Failed!");
  } else {
    Serial.print(" Connected!");
    Serial.println(WiFi.localIP());
    digitalWrite(LedPin, HIGH);
  }
}

void reconnect() {
  while(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    unsigned long b_mssl = millis();
    if(client.connect(clientId.c_str())) {
      unsigned long e_mssl = millis();
      Serial.println("connected");
      Serial.print("time to establish default connection:");
      Serial.println(e_mssl - b_mssl);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); 
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]");
    char delimiter[2] = "/";
    char* token;

    token = strtok(topic, delimiter);
    token = strtok(NULL, delimiter);
    Serial.print("Seperated topic name: ");
    Serial.println(token);
}


void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);
  connectToWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  clientId += String(random(0xffff), HEX);
}

void loop() {
  if(!client.connected()) {
    reconnect();
    client.subscribe("MK/#");
  }
  client.loop();
}
