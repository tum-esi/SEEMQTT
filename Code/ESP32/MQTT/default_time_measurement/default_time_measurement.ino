/*
  MQTT over SSL/TLS
*/

#include <WiFi.h>
#include <PubSubClient.h>


#define TIME_DEBUG
#define RECON

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "ESP32Client-"; /* MQTT client ID (will add random hex suffix during setup) */

const char* WIFI_NETWORK = "Commodore64";
const char* WIFI_PASSWORD = "ma0s#verSuch";
const char* MQTT_SERVER = "192.168.178.37";
const int MQTT_PORT = 1883;
const uint32_t WIFI_TIMEOUT_MS = 20000;
unsigned long lastMsg = 0;
int value = 0;

#define Size_Byte 15360 // size of the message
#define MSG_BUFFER_SIZE 50
char msgid[MSG_BUFFER_SIZE];
char* mymsg;


// LED GPIO port on our ESP32 DEVKITV1 board
const int LedPin = 2;


static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

char* rand_string_alloc(size_t size)
{
     char *s =(char *) malloc(size + 1);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

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
  // Loop until we're reconnected
  while(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    unsigned long b_mssl = millis();
    
    // Attempt to connect
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


void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);
  connectToWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Create a random client ID and random message
  clientId += String(random(0xffff), HEX);
  mymsg = rand_string_alloc(Size_Byte);
  Serial.println(mymsg);
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }
  
  unsigned long now = millis();
  if(now - lastMsg > 3000) {
    lastMsg = now;
    ++value;

    unsigned long b_msgsend = millis();
    client.beginPublish("mhh_my_topic", Size_Byte, true); 
    client.write((byte *)mymsg, (int)Size_Byte); 
    client.endPublish();
    // client.publish("mhh_my_topic", "testmsg");
    unsigned long e_msgsend = millis();
    
    snprintf(msgid, MSG_BUFFER_SIZE, "Message ID #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msgid);

    #ifdef TIME_DEBUG
    char timemsg[MSG_BUFFER_SIZE];
    snprintf(timemsg, MSG_BUFFER_SIZE, "time to publish %ld Bytes in ms: ", Size_Byte);
    Serial.print(timemsg);
    Serial.println(e_msgsend - b_msgsend);
    #endif

    #ifdef RECON
    client.disconnect();
    #endif
  }
  client.loop();
}
