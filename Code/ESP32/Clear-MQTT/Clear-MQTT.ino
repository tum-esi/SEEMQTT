/*
  This code test the time to connect and send different size of data using (clear) MQTT. 
  No seaurity measured were considered here. 
*/
#include <pbc.h>
#include <WiFi.h>
#include <PubSubClient.h>


#define TIME_DEBUG
#define RECON

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "ESP32Clientclear-"; /* MQTT client ID (will add random hex suffix during setup) */

//const char* WIFI_NETWORK = "mhhap";
//const char* WIFI_PASSWORD = "mhh1986mhh>>";

const char* WIFI_NETWORK = "TP-Link_904A";
const char* WIFI_PASSWORD = "30576988";
const char* MQTT_SERVER = "192.168.0.110"; 
const int MQTT_PORT = 1883;
const uint32_t WIFI_TIMEOUT_MS = 20000;
unsigned long lastMsg = 0;
int value = 0;
int Size_Byte;


#define MSG_BUFFER_SIZE 50
char msgid[MSG_BUFFER_SIZE];
uint8_t * mymsg;
int idx = 0 ; 
  

int packet_size[] = {128, 265, 1024,2048, 5120,10240, 15360}; 

static uint8_t  *rand_string(uint8_t  *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY";
    if (size) {
        //--size;
        for (size_t n = 0; n < size; n++) {
            int key = esp_random() % 16;
            str[n] = key;
        }
    }
    return str;
}

uint8_t * rand_string_alloc(size_t size)
{
     uint8_t  *s =(uint8_t  *) malloc(size);
     if (s) {
         rand_string(s, size);
     }
     return s;
}



void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  uint32_t startAttemptTime = millis();

  while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
      delay(100);
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println(" Failed!");
  } else {
    Serial.println(" Connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
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
  connectToWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Create a random client ID and random message
  clientId += String(random(0xffff), HEX);
  //mymsg = rand_string_alloc(Size_Byte);
  //Serial.println(mymsg);
}

void loop() {

  unsigned long now = millis();


 

  
  if(now - lastMsg > 3000) {

      if(!client.connected()) {
    reconnect();
  }
 
  if( value %50 == 0) 
  {
   Size_Byte =packet_size[idx];
   mymsg = rand_string_alloc(Size_Byte);
   idx++; 
  }
    lastMsg = now;
    ++value;
    if (value >=350)
     value = 0; 

    unsigned long b_msgsend = millis();
    client.beginPublish("test",Size_Byte , true); 
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
