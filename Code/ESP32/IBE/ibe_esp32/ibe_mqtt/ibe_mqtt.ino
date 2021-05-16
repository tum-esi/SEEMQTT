#include <math.h>
#include <string.h>
#include <pbc.h>
#include <mbedtls/ssl.h>
#include <mbedtls/md.h>
#include <mbedtls/error.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TYPEA_PARAMS \
"type a\n" \
"q 87807107996633125224377819847540498158068831994142082" \
"1102865339926647563088022295707862517942266222142315585" \
"8769582317459277713367317481324925129998224791\n" \
"h 12016012264891146079388821366740534204802954401251311" \
"822919615131047207289359704531102844802183906537786776\n" \
"r 730750818665451621361119245571504901405976559617\n" \
"exp2 159\n" \
"exp1 107\n" \
"sign1 1\n" \
"sign0 1\n"

#define TIME_DEBUG
#define enc_dec_times 10
#define MSG_SIZE 1024
#define TOPIC_SIZE 50

WiFiClient espClient;
PubSubClient mqttclient(espClient);
String clientId = "ESP32Client-"; /* MQTT client ID (will add random hex suffix during setup) */
String base_topic = "MK";
char sysparam_topic[TOPIC_SIZE];
char acksysparam_topic[TOPIC_SIZE];

const char *ssid = "mhhap";
const char *password = "mhh1986mhh>>";
const char *MQTT_SERVER = "192.168.0.107";
const int MQTT_PORT = 1883;
const uint32_t WIFI_TIMEOUT_MS = 20000;
unsigned long lastMsg = 0;

/* struct to store the PKG public parameters */
struct ibe_sys_param_public {
    element_t Kpub;
    element_t P;
    pairing_t pairing;  
} sys_param_pub;

const int LedPin = 2;
int value = 0;
int loop_times;
unsigned long stime, etime, p1time, p2time, p3time, p4time, sumtime;

void setup_wifi() {

    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long b_conap= micros();

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    unsigned long e_conap= micros();
    Serial.print(" Time to connect to AP is :" );
    Serial.print(e_conap - b_conap);
    Serial.println(" (us)"); 
    randomSeed(micros());

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  
    while (!mqttclient.connected()) {
        Serial.print("Attempting MQTT connection...");
        
        unsigned long b_conn = micros();
        if (mqttclient.connect(clientId.c_str())) {
            unsigned long e_conn = micros();
            Serial.println("connected");
            
            #ifdef TIME_DEBUG
            Serial.print("Time to estiblsh connection with server is:");
            Serial.print(e_conn - b_conn);
            Serial.println(" (us)"); 
            #endif
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttclient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
    }
  }
}

void setup() {

    Serial.begin(115200);
    pinMode(LedPin, OUTPUT);

    setup_wifi();

    /* create the PKG topic */
    clientId += String(random(0xffff), HEX);
    strcpy(sysparam_topic, String((base_topic + "/" + clientId + "/" + "sysparam")).c_str());
    strcpy(acksysparam_topic, String((base_topic + "/" + clientId + "/" + "acksysparam")).c_str());

    /* connect to mqtt broker */
    Serial.println("++++ connecting to MQTT broker ++++");
    mqttclient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttclient.setCallback(PKGCallback);
    mqttclient.connect(clientId.c_str());
    mqttclient.subscribe(acksysparam_topic);
    Serial.println("++++ connected ++++");

    /* require PKG parameters */
    Serial.println("++++ requiring IBE system parameters from PKG ++++");
    char *message = "PKG system parameters";
    mqttclient.beginPublish(sysparam_topic, strlen(message) + 1, false);
    mqttclient.write((byte *)message, strlen(message) + 1); 
    mqttclient.endPublish();
}

void loop() {

    if (!mqttclient.connected()) {
        reconnect();
    }
    mqttclient.loop();
}

void PKGCallback(char* topic, uint8_t* payload, unsigned int payload_length) {
    
    printf("message received from topic %s\n", topic);

    if (!strcmp(topic, acksysparam_topic)) {
        /*
        -----------------------------------
        | param type | Kpub | generator P |
        -----------------------------------
        */

        unsigned char param_type[1];

        memcpy(param_type, payload, 1);
        
        if (param_type[0] == 'a') {
            printf("++++ choosing TYPE A curve ++++\n");
            #define CURVE_TYPE TYPEA_PARAMS
        }

        pairing_init_set_buf(sys_param_pub.pairing, CURVE_TYPE, strlen(CURVE_TYPE));
        if (!pairing_is_symmetric(sys_param_pub.pairing)) pbc_die("pairing must be symmetric");
        
        element_init_G1(sys_param_pub.P, sys_param_pub.pairing);
        element_init_G1(sys_param_pub.Kpub, sys_param_pub.pairing);

        int P_len = element_length_in_bytes(sys_param_pub.P);
        int Kpub_len = element_length_in_bytes(sys_param_pub.Kpub);

        unsigned char P_tmp[P_len], Kpub_tmp[Kpub_len];
        memcpy(Kpub_tmp, payload + 1, Kpub_len);
        memcpy(P_tmp, payload + 1 + Kpub_len, P_len);
    
        element_from_bytes(sys_param_pub.Kpub, Kpub_tmp);
        element_from_bytes(sys_param_pub.P, P_tmp);
    }
}


void PrintHEX(unsigned char* str, int start_byte, int end_byte) {

    for (int i = start_byte; i < end_byte; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}
