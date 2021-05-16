/*
  MQTT over SSL/TLS
*/

#include <WiFiClientSecure.h>
#include <PubSubClient.h>


#define TIME_DEBUG
#define RECON
#define ONEWAY
#define TLS_CON_TIME

WiFiClientSecure espClient;
PubSubClient mqttclient(espClient);
String clientId = "ESP32Client-"; /* MQTT client ID (will add random hex suffix during setup) */

const char* WIFI_NETWORK = "Commodore64";
const char* WIFI_PASSWORD = "ma0s#verSuch";
const char* MQTT_SERVER = "192.168.178.37";
#ifdef ONEWAY
const int MQTT_PORT = 8883;
#else const int MQTT_PORT = 8884;
#endif
const uint32_t WIFI_TIMEOUT_MS = 20000;
unsigned long lastMsg = 0;
int value = 0;

#define Size_Byte 1024 // size of the message
const char mqttCertFingerprint[] = "FD D8 83 B0 3B B8 DD 3D 34 D5 B8 03 41 74 32 4C 0C A2 B8 05 A7 11 0E A6 96 19 9A C0 C4 C3 90 B8";


#define MSG_BUFFER_SIZE 50
char msgid[MSG_BUFFER_SIZE];
char* mymsg;


// LED GPIO port on our ESP32 DEVKITV1 board
const int LedPin = 2;

// CA certificate
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDPjCCAiYCCQCh6d3x3kTMVTANBgkqhkiG9w0BAQsFADBhMQswCQYDVQQGEwJE\n" \
"RTEQMA4GA1UECAwHQmF2YXJpYTEPMA0GA1UEBwwGTXVuaWNoMRYwFAYDVQQLDA1J\n" \
"b1QgVFUgTXVuaWNoMRcwFQYDVQQDDA4xOTIuMTY4LjE3OC4zNzAeFw0yMDA3MDEx\n" \
"MzU1NDBaFw0yMTA3MDExMzU1NDBaMGExCzAJBgNVBAYTAkRFMRAwDgYDVQQIDAdC\n" \
"YXZhcmlhMQ8wDQYDVQQHDAZNdW5pY2gxFjAUBgNVBAsMDUlvVCBUVSBNdW5pY2gx\n" \
"FzAVBgNVBAMMDjE5Mi4xNjguMTc4LjM3MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\n" \
"MIIBCgKCAQEAwvmt5PH6VAXw9Q5MLsm4rWG53FdtNGq+rl3aegf1gAP/Gqx+jo7c\n" \
"q03NkWkDV2sTHDYOvlbs6PTcsy16UzkknNcAXlyCEce5450Am/iHAeqBho2czcMK\n" \
"FDuywTWGBZOvd1d/tY5MuCT2oMTs2JegJlqP8GoEH8VEewOKyGm8SRPRv01qLtus\n" \
"cDlFh0ssRuc5v03VYS2npxtsNX5MdERBGF/4u5ni1VBDg7PROp5e9OWkOLkwJw3r\n" \
"pgDzyBw46dAj1Fsk6pWfkvDXg85986HNspKuE39IUN7ERjgXPrLPKC3a5b+HZ8ok\n" \
"CYY9CkPMTZrYlnio8898TKYywJ5OjPXVpQIDAQABMA0GCSqGSIb3DQEBCwUAA4IB\n" \
"AQCXyAMRTEQFtCc7nEELoNcTCblY753eDN7qdOvziAb878dkrGupJjY/8x35fXzs\n" \
"o5A2TQ1q7Qml+v2otPgMh40W/Dd3XgYBPrOpAAzVuxKOIOpPXAgvjXAMPMMA5pMk\n" \
"eIqoFX67V9uJUScMl8O7K1pbSRHt8a5iqNzDMxmZOCYoCjHCilggwEZTHFQHzKiw\n" \
"967jR1POXAXmZywPfDMV7+35u6erNw6C0Z1dc3BGPrpV9khGHnnRxIjj0LG+omHK\n" \
"pXgaSAeBncjAQGG3bUzj7Y71DP/F00khbmm/+yJGr0eSFFzkWn829zK7B3R17dBh\n" \
"uYTHdQCOyZyheZ9IOyMGrsp3\n" \
"-----END CERTIFICATE-----\n";

#ifndef ONEWAY
// Client certificate
const char* client_cert = \ 
"-----BEGIN CERTIFICATE-----\n" \
"MIIDNDCCAhwCAQIwDQYJKoZIhvcNAQELBQAwYTELMAkGA1UEBhMCREUxEDAOBgNV\n" \
"BAgMB0JhdmFyaWExDzANBgNVBAcMBk11bmljaDEWMBQGA1UECwwNSW9UIFRVIE11\n" \
"bmljaDEXMBUGA1UEAwwOMTkyLjE2OC4xNzguMzcwHhcNMjAwNzAxMTM1NTQwWhcN\n" \
"MjEwNzAxMTM1NTQwWjBfMQswCQYDVQQGEwJERTEQMA4GA1UECAwHQmF2YXJpYTEP\n" \
"MA0GA1UEBwwGTXVuaWNoMRQwEgYDVQQLDAtFU1AzMiBCb2FyZDEXMBUGA1UEAwwO\n" \
"MTkyLjE2OC4xNzguMzYwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC0\n" \
"0yJdAj0vZMho0qSehZ8hrjYEA/F7DzdYmROeHA129MW+CXRz+aE0xrDYOBRENSzs\n" \
"1/rmfCl1CgJ0ikD7TDKMtHJVKGNN9Qp15X1b6et8WqOwluAmCAkIaNtQQqOstdhj\n" \
"shCTsL6oUzCduILN9/S2V+fOwy1s8fgDvwuF/B6WcUr0UQQ403jsdjZjI71eQn7n\n" \
"xgdytstWyobWRJOlAn/pvbi7DqJ3retdN1WqyutG2YlfcVkR30fuM5Zu9pbqeJ4N\n" \
"O+xgilAG7vS1qPa9Ll31G8Y/c+8N/SmtCPywbBRGgLcLrDCh2kfbkyAscwyHiUWv\n" \
"7m/pE/D9TaQRB40uj3DtAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAJskT1p+bVXw\n" \
"S5HF6OGTB6MDrn/HqKdmwFXNIKJjQygrmZarBq9Lpdj9cEmWhi1c6AmiazwwVSJ7\n" \
"jz5eH3pH7a8/OuMUV4AwJWJ88iN77jowC2vYjHjnCcIZ0kudX78e7f41yEJcaegv\n" \
"lqYqaShXBs9Id8HNoHAGnwMGIx8ntHGICbGa4GAvcCaH7YqIT8aTIuIgWA+SvZUi\n" \
"YTzKrp+te/mJ/brQaS7MuAJBjVStGk906SHvrU+8Gh+SgktXqIYQCktN0yi+AlcM\n" \
"bYQosELlzBlApqZPcwaE48S1iEypLlMgH1PqBd+R90JrVXgIEw3jEKCWW2wQd0dm\n" \
"EhKoZjA0ax4=\n" \
"-----END CERTIFICATE-----\n";

// Client private key
const char* client_private_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpgIBAAKCAQEAtNMiXQI9L2TIaNKknoWfIa42BAPxew83WJkTnhwNdvTFvgl0\n" \
"c/mhNMaw2DgURDUs7Nf65nwpdQoCdIpA+0wyjLRyVShjTfUKdeV9W+nrfFqjsJbg\n" \
"JggJCGjbUEKjrLXYY7IQk7C+qFMwnbiCzff0tlfnzsMtbPH4A78LhfwelnFK9FEE\n" \
"ONN47HY2YyO9XkJ+58YHcrbLVsqG1kSTpQJ/6b24uw6id63rXTdVqsrrRtmJX3FZ\n" \
"Ed9H7jOWbvaW6nieDTvsYIpQBu70taj2vS5d9RvGP3PvDf0prQj8sGwURoC3C6ww\n" \
"odpH25MgLHMMh4lFr+5v6RPw/U2kEQeNLo9w7QIDAQABAoIBAQCi832NGX5v3j+o\n" \
"DuMgo8PCw06UEcWkoxK6OcBvBFKx92TXm4ehF71JZSBqySaiTNU+XmgRGqT1ft61\n" \
"jP2AOKt5FwfNbAcJcqBJnFR65vM6bigC9APs4eLZ/DxfOJ5s/raRKR89KNK3t7gg\n" \
"+eyOljulstQIBOVDv05Eef51cL1TziLPoItkcFEZQEScUl+ryrCx/pldu+IjUZII\n" \
"gTe/P2JBEzo8Vyx8lmoLpNduGhXZlnBLsv5kjql1px6r9MWQMGQGolyOxare2yyi\n" \
"vGBnF9S9k4dOLiDPMU7FWPIQ6DjMuPKcG34i3RMnuGBCDmSKHWgHmbf/ps9J5QZI\n" \
"ktZR3BT1AoGBAN14HpWrDrcbqsIWWjf99Rwayz3KzX3DqhwiJ6TgEJn7w7znMqjy\n" \
"lIk2I9ftTzPo6RaejX1Nseo1VaAH9c7SWoBmlpyflUbPku7+7rqocBCmh3x49rJn\n" \
"0Lz5oNxgD52WK+Du2fmGctMuHcTUo9IJ78BlVvsufOZUZod0dJ7a4X6XAoGBANEE\n" \
"s8PBdeT+PwwF6VMIaVD5Qb1xmfiJZYgASf6efd7WDBIis07JrrEkhyFvH6PPeJq6\n" \
"HR+DtS7+RncTePLmJwA8ZcKrRRtv1MDOZz0TckBU4o83ucZP06sjedlLsQeQ5K4k\n" \
"fjem08HC8oLeaRas8WV4fyPs0hwy9KTNvciki4EbAoGBAIWCaE2ptVFFaIdgmjqu\n" \
"6hhrPqKdo4txbRdQr72BJgDt+W3jonKe6lRdQvZkpR6SBhIQuxMSx3J9fITgx5pK\n" \
"zSwirCD3QzJQ6C8TmC8GY6hl+79Z3L5oPzns8sBnWIKTiRgsZVM7bfVJE8EYvzlb\n" \
"rCC79tetirX/o199IOCy6sU5AoGBAKzn3OWE9kf2YqVJhphfju+jIlTa0uocjtcQ\n" \
"liWBatKZ8uZVukrk4BQ+CWMJD/1stXIZn9ngFFPJmPUMMLOO3Jh/XhbJo6NswWy1\n" \
"0gVePtck4T5kdNOuWYVrFXNCADIoFGRSX+8dnHLfF8pWvkw5yH+OTG6li7I0HNjw\n" \
"MZz3MUgdAoGBANfyxVlMi6ZV/j0NiYaYrlHEuIqs+H9x6MDP4q3noMW6i1MwBTEs\n" \
"heSM2ksuH4fpkYuCmWVCLxUfOFXmQrn4Kuda0wTO2fk7HJZQX4j7NLi4HA5UiWex\n" \
"tKa2TRTnDifhwVQMB1SIPSr7A4u5+lSmmx0yF5dpdPKsAPlF6B5Yaeit\n" \
"-----END RSA PRIVATE KEY-----\n";
#endif


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
  while(!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    unsigned long b_mssl = millis();
    // Attempt to connect
    if(mqttclient.connect(mqttclientId.c_str())) {
      unsigned long e_mssl = millis();
      Serial.println("connected");
      Serial.print("time to establish SSL:");
      Serial.println(e_mssl - b_mssl);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); 
    }
  }
}

bool verifytls() {
  bool success = false;
  Serial.print("Verifying TLS connection to ");
  Serial.println(MQTT_SERVER);

  // Test time consumption for establishing SSL connection
  // WiFiClientSecure::connect(const char *host, uint16_t port) calls start_ssl_client()
  // to connect to server

  // start_ssl_client(const char *host, uint16_t port, const char *_CA_cert, const char *_cert, \
                      const char *_private_key)

  // Start SSL connection procedures:
  //  esp/esp-idf/components/mbedtls/mbedtls/library/ssl_cli.c, this file define the whole SSL handshake  procedures

  //  1. Parse CA certificate, client certificate and client private key format if they are provided
  //  2. Format ok? If ok, accept those files for SSL communication
  //  3. ssl_tls.c 5747 line, mbedtls_x509_crt_verify_restartable() this will check if server certificate is valid or not
  //  4. Symmetric key negotiate
  //  5. Certificate verify (with digital signature)
  //  6. Finished message (encrypted with symmtric key)

  unsigned long bconser = millis();
  success = espClient.connect(MQTT_SERVER, MQTT_PORT);
  unsigned long econser = millis();

  if(success) {
    Serial.println("Connection complete, valid certificate!");
    Serial.print("time to connection: ");
    Serial.print(econser - bconser);
  } else {
    Serial.println("Connection failed!");
  }
  return (success);
}

void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);

  connectToWiFi();

  #ifdef ONEWAY
  espClient.setCACert(ca_cert);
  #else
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_private_key);
  #endif

  #ifdef TLS_CON_TIME
  verifytls();
  #endif

  mqttclient.setServer(MQTT_SERVER, MQTT_PORT);

  
  // Create a random client ID and random message
  clientId += String(random(0xffff), HEX);
  mymsg = rand_string_alloc(Size_Byte);
  Serial.println(mymsg);
}

void loop() {
  if(!mqttclient.connected()) {
    reconnect();
  }
  
  unsigned long now = millis();
  if(now - lastMsg > 3000) {
    lastMsg = now;
    ++value;

    unsigned long b_msgsend = millis();
    mqttclient.beginPublish("mhh_my_topic", Size_Byte, true); 
    mqttclient.write((byte *)mymsg, (int)Size_Byte); 
    mqttclient.endPublish();
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
    mqttclient.disconnect();
    #endif
  }
  mqttclient.loop();
}
