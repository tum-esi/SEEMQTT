


#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define TIME_DEBUG
#define RECON

// Update these with values suitable for your network.
const char* ssid = "mhhap";
const char* password = "mhh1986mhh";
const char* mqtt_server =  "192.168.12.232"; //"mqtt.eclipse.org";
const int mqttPort = 2222; //1883


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// size of the message

#define Size_Byte 1024
char * mymsg; 

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




void loop() {

  if (!client.connected()) {
    reconnect();
  }
  

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    
    //client.publish("mhh_my_topic", mymsg);
    unsigned long b_msgsend = millis();
    client.beginPublish("mhh_my_topic", Size_Byte, true); 
    client.write((byte *)mymsg, (int)Size_Byte); 
    client.endPublish ();

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
   client.disconnect();
   digitalWrite(BUILTIN_LED, LOW); 
  #endif
  }

  client.loop();
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  unsigned long b_conap= millis();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

   unsigned long e_conap= millis();
  Serial.print(" Time to connect to Ap is :" );
  Serial.println(e_conap - b_conap);
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
     unsigned long b_conn = millis();
    if (client.connect(clientId.c_str())) {
     unsigned long e_conn = millis();
      Serial.println("connected");
      digitalWrite(BUILTIN_LED, HIGH);
      #ifdef TIME_DEBUG
      Serial.print("time to estiblsh connection is:");
      Serial.println(e_conn - b_conn);
      #endif
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
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  mymsg = rand_string_alloc(Size_Byte);
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
}
