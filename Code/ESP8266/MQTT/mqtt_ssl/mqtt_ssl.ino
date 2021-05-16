/*
 * MQTT over SSL
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>


#define SERIAL_DEBUG
#define TLS_DEBUG
#define TIME_DEBUG
#define RECON
const char* ssid = "mhhap";
const char* password = "mhh1986mhh";
const char* mqtt_server =  "192.168.12.232"; 
const int mqttPort = 8883; //1883


#define Size_Byte 20480 // size of the message




const char mqttCertFingerprint[] PROGMEM = "49 39 9F 19 FA 7F 21 93 D7 8A 35 46 23 74 AF F7 1A 8B 5F 8C";

WiFiClientSecure espClient; 
PubSubClient mqttClient(espClient);
String clientId = "ESP8266Client-"; /* MQTT client ID (will add random hex suffix during setup) */



unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


char * mymsg; 



void setup() 
{


#ifdef SERIAL_DEBUG
  /* Initialize serial output for debug */
  Serial.setDebugOutput(true);
 Serial.begin(115200);
  Serial.println();

#endif
#ifdef SERIAL_DEBUG
  Serial.print("connecting to ");
  Serial.println(ssid);
#endif
  unsigned long b_conap= millis();
  /*  Connect to local WiFi access point */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
#ifdef SERIAL_DEBUG
  Serial.print("Connecting");
 
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
#ifdef SERIAL_DEBUG
    Serial.print(".");
#endif
  }
#ifdef SERIAL_DEBUG

 unsigned long e_conap= millis();
  #ifdef TIME_DEBUG
  Serial.print(" Time to connect to Ap is:" );
  Serial.println(e_conap - b_conap);
  #endif
  /* When WiFi connection is complete, debug log connection info */
  Serial.println("");
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
#endif

#ifdef SERIAL_DEBUG
  Serial.print("connecting to ");
  Serial.println(mqtt_server);
#endif
             
  espClient.setFingerprint(mqttCertFingerprint);  /* Load SHA1 mqtt cert fingerprint for connection validation */
                                             
#ifdef TLS_DEBUG
  /* Call verifytls to verify connection can be done securely and validated - this is optional but was useful during debug */
  verifytls();
#endif

  /* Configure MQTT Broker settings */
  mqttClient.setServer(mqtt_server, mqttPort);

  /* Add random hex client ID suffix once during each reboot */
  clientId += String(random(0xffff), HEX); 
  mymsg = rand_string_alloc(Size_Byte);
}



void reconnect() {
  /* Loop until we're reconnected */
  while (!mqttClient.connected()) {

      Serial.print("Attempting MQTT broker connection...");
      
      unsigned long b_mssl= millis();
    /* Attempt to connect */
      if (mqttClient.connect(clientId.c_str())) {
      
      unsigned long e_mssl = millis();
      
      Serial.println("connected");
       #ifdef TIME_DEBUG
      Serial.print("time to estiblsh SSL:");
      Serial.println(e_mssl - b_mssl);
      #endif
     
    } 
    else {

      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Trying again in 5 seconds...");

      /* Wait 5 seconds between retries */
      delay(5000);
    }
  }
}



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



/* verifytls()
 *  Test WiFiClientSecure connection using supplied cert and fingerprint
 */
bool verifytls() {
  bool success = false;

  Serial.print("Verifying TLS connection to ");
  Serial.println(mqtt_server);


  success = espClient.connect(mqtt_server, mqttPort);


  if (success) {
    Serial.println("Connection complete, valid cert, valid fingerprint.");
  }
  else {
    Serial.println("Connection failed!");
  }


  return (success);
}


void loop() 
{
  /* Main loop. Attempt to re-connect to MQTT broker if connection drops, and service the mqttClient task. */
  if(!mqttClient.connected()) {
    reconnect();
  }
  

  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    
    unsigned long b_msgsend = millis();
    mqttClient.beginPublish("mhh_my_topic", Size_Byte, true); 
    mqttClient.write((byte *)mymsg, (int)Size_Byte); 
    mqttClient.endPublish ();
   unsigned long e_msgsend = millis();
   
   snprintf (msg, MSG_BUFFER_SIZE, "Message ID #%ld ", value );
    Serial.print("Publish message: ");
    Serial.println(msg);
    #ifdef TIME_DEBUG
    char timemsg [50] ; 
    snprintf (timemsg, MSG_BUFFER_SIZE, "time to publish %ld Byte in Ms:", Size_Byte );
    Serial.print(timemsg); 
    Serial.println(e_msgsend - b_msgsend);
    #endif
   #ifdef RECON
   mqttClient.disconnect(); 
   #endif
  }
 
mqttClient.loop();
}
