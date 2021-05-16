/*
 * Boneh & Franklin encryption scheme as presented in the article "Id-based encryption from the Weil pairing"
 *
 * @download: sudo apt install libgmp3-dev
 *            sudo apt install libssl-dev
 *            pbc library: https://crypto.stanford.edu/pbc/
 *
 * @compile: gcc pkg_mqtt.c -o pkgmqtt -lpbc -lgmp -lssl -lcrypto -lmosquitto -Wl,-rpath=/usr/local/lib
 */
#include "pkg_mqtt.h"


#define DBG_MSG
#define SELF_TEST
#define ELEMENT_SIZE 1024
#define MSG_SIZE 32
#define TOPIC_SIZE 20

static bool run = true;

/* Configuration for MQTT */
const char* mqttServer =  "192.168.178.38";
const int mqttPort = 1883;
const char* base_topic = "MK/";
const char* mqttConnTopic = "MK/#";
const char* PKGID = "PKGID-1";

ibe_sys_param_public_t sys_param_pub;
ibe_sys_param_private_t sys_param_pri;

void PrintVerInfo() {
    printf("**************************************\n");
    printf("*  PKG (Private Key Generator) v1.0  *\n");
    printf("*  Author: Hangmao Liu               *\n");
    printf("**************************************\n\n");
}


int main(int argc, char **argv) {
    /*
     * P,Q,R -> G1, with Q = aP, R = bP and the set D = {abP, rP} with r random be given,
     *
     * 1. Setup:
     *  corresponding pairing e,
     *  a random private key Km = s,
     *  a public key Kpub = sP,
     *  public hash functions H1 and H2.
     *
     * 2. Extract:
     *  the PKG computes the public key Qid = H1(ID) and
     *  the private key Did = sQid which is given to the user
     *
     * 3. Encrypt:
     *  Qid = H1(ID) -> G1*,
     *  choose random r -> Zp*,
     *  compute gid = e(Qid, Kpub), note that Kpub is PKG's public key
     *  set c = <rP, m XOR H2(gid^r)>
     *
     * 4. Decrypt:
     *  Given c = <u, v>, m = v XOR H2(e(Did, u))
     */

    PrintVerInfo();    

    /* Initialize system parameters */
    printf("********* IBE phase 1: setup *********\n");

    ibe_setup(&sys_param_pub, &sys_param_pri);

    /* Self test for IBE enc/decryption */
    #ifdef SELF_TEST

    printf("running test for IBE enc/decryption ...\n");

    int fd = 0;
    unsigned char msg[MSG_SIZE] = {0};
	fd = open("/dev/urandom",O_RDONLY);
	read(fd, msg, MSG_SIZE); 
	close(fd);
    printf("message in plaintext: \n");
    PrintHEX(msg, 0, 32);


    int km_len = element_length_in_bytes(sys_param_pri.Km);
    printf("km length:%d\n", km_len);
    unsigned char kpub[128], p[128], km[20];

    element_to_bytes(kpub, sys_param_pub.Kpub);
    element_to_bytes(p, sys_param_pub.P);
    element_to_bytes(km, sys_param_pri.Km);

    FILE *fp;
    fp = fopen("kpub.txt", "a");
    
    for (int i = 0; i < 128; i++) {
       fprintf(fp, "0x%.2X ", kpub[i]); 
    }
    
    fclose(fp);

    fp = fopen("p.txt", "a");
    
    for (int i = 0; i < 128; i++) {
        fprintf(fp, "0x%.2X ", p[i]);
    }
    
    fclose(fp);

    fp = fopen("km.txt", "a");

    for (int i = 0; i < 20; i++) {
        fprintf(fp, "0x%.2X ", km[i]);
    }

    fclose(fp);

    element_t kpub_e, p_e, km_e;
    element_init_G1(kpub_e, sys_param_pub.pairing);
    element_init_G1(p_e, sys_param_pub.pairing);
    element_init_Zr(km_e, sys_param_pub.pairing);

    printf("kpub value in bytes hex:\n");
    PrintHEX(kpub, 0, 128);
    printf("p value in bytes hex:\n");
    PrintHEX(p, 0, 128);


    element_from_bytes(kpub_e, kpub);
    element_from_bytes(p_e, p);
    element_from_bytes(km_e, km);

    printf("kpub_e element:\n");
    element_printf("%B\n", kpub_e);
    printf("p_e element:\n");
    element_printf("%B\n", p_e);
    printf("km_e element:\n");
    element_printf("%B\n", km_e);

    unsigned char id[20] = "test@test.com";
    unsigned char cipher[MSG_SIZE], plaintext[MSG_SIZE];
    unsigned char Uc[ELEMENT_SIZE], Didc[ELEMENT_SIZE];
    
    ibe_extract(&sys_param_pub, &sys_param_pri, id, Didc);
    ibe_encryption(&sys_param_pub, id, msg, MSG_SIZE, cipher, Uc);
    ibe_decryption(&sys_param_pub, cipher, MSG_SIZE, Uc, Didc, plaintext);

    printf("decrypted message: \n");
    PrintHEX(plaintext, 0, 32);

    printf("comparsion result: %d\n", strncmp(msg, plaintext, MSG_SIZE));

    printf("parameter for SMQTT:\n");
    unsigned char kpub_test[128] = {
0x08, 0x66, 0xA0, 0x73, 0x5C, 0xDA, 0x27, 0x3E, 0x8E, 0x24, 0xC7, 0x78, 0xB4, 0xB6, 0xCA, 0x47, 0x32, 0x26, 0xDD, 0x1B, 0xC1, 0x22, 0x3C, 0xF4, 0x80, 0x7D, 0xB9, 0x8D, 0xDF, 0x53, 0xE3, 0x11, 0xFA, 0x7D, 0x90, 0xFF, 0xE5, 0x6D, 0xDA, 0x42, 0x16, 0x06, 0xCE, 0xA6, 0x0A, 0x13, 0xD6, 0xFC, 0x63, 0x95, 0x6F, 0x47, 0x40, 0xF3, 0xBE, 0x56, 0x15, 0x3D, 0xC5, 0x5D, 0xC2, 0x2B, 0x56, 0xB2, 0x83, 0xDC, 0x6E, 0x77, 0x1E, 0x19, 0xE9, 0x86, 0x33, 0xB9, 0x2F, 0xFD, 0xE8, 0xBA, 0x84, 0x5A, 0x23, 0x08, 0xC5, 0x16, 0x9A, 0x96, 0x85, 0xB7, 0xC0, 0x80, 0x03, 0x72, 0xC2, 0xBB, 0x91, 0x42, 0x65, 0xF8, 0x96, 0x91, 0xC9, 0x78, 0xA6, 0x42, 0xB6, 0x08, 0x42, 0xBE, 0x53, 0x9C, 0x6A, 0x5E, 0xC8, 0xFB, 0x0E, 0x68, 0x22, 0xF2, 0x6A, 0x38, 0xA6, 0x0A, 0x2D, 0xAF, 0x67, 0x1D, 0x6F, 0xBD};


    unsigned char p_test[128] = {
0x27, 0x99, 0x13, 0x86, 0x16, 0x12, 0xC5, 0x11, 0x02, 0xE8, 0x65, 0x9B, 0xCB, 0xA3, 0xC2, 0x50, 0x86, 0xDB, 0xDA, 0xC9, 0x44, 0x4E, 0x66, 0x61, 0x5D, 0x37, 0x0D, 0xDF, 0x6B, 0xF5, 0x9B, 0x50, 0xE0, 0x8C, 0x1A, 0x7C, 0xBF, 0x24, 0x2A, 0x76, 0xAC, 0x8F, 0x4F, 0x0D, 0xD6, 0xD6, 0xA7, 0x5A, 0xFD, 0xEB, 0x74, 0x36, 0x24, 0x1B, 0xD6, 0x28, 0x53, 0x62, 0xDF, 0xD5, 0x20, 0x35, 0xB5, 0x68, 0x6D, 0xF1, 0xF5, 0x77, 0x41, 0x12, 0xB8, 0xF0, 0x57, 0x06, 0x30, 0x84, 0xE6, 0x5A, 0x1D, 0xF3, 0x12, 0x12, 0xAC, 0x64, 0x6B, 0xEB, 0x50, 0xD5, 0x5F, 0x0A, 0xFE, 0xF4, 0x9F, 0xC6, 0x57, 0xDB, 0x00, 0x97, 0xB9, 0xCD, 0xD5, 0x8E, 0xFA, 0xA3, 0x3C, 0x03, 0x32, 0x33, 0x67, 0x74, 0x15, 0x0E, 0x1D, 0x69, 0x46, 0xD4, 0x75, 0x99, 0x13, 0x32, 0x77, 0xCC, 0xFD, 0x39, 0xED, 0xF1, 0xA0, 0x89};
    
    element_t kpub_ee, p_ee;
    element_init_G1(kpub_ee, sys_param_pub.pairing);
    element_init_G1(p_ee, sys_param_pub.pairing);

    element_from_bytes(kpub_ee, kpub_test);
    element_from_bytes(p_ee, p_test);
    
    printf("kpub_ee element:\n");
    element_printf("%B\n", kpub_ee);
    printf("p_ee element:\n");
    element_printf("%B\n", p_ee);

    #endif
        
    /* Start mosquitto client */
    printf("Waiting connection from IoT node ...\n");

    int rc = 0;
    struct mosquitto* mosq;

    mosquitto_lib_init();
    mosq = mosquitto_new(PKGID, true, 0);
    
    if (mosq) {
        mosquitto_message_callback_set(mosq, PKGCallback);
        
        rc = mosquitto_connect(mosq, mqttServer, mqttPort, 60);
        
        if (rc) {
            printf("Error: %s \n", mosquitto_strerror(rc));
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();

            element_free(sys_param_pub.Kpub);
            element_free(sys_param_pub.P);
            element_free(sys_param_pri.Km);
            pairing_clear(sys_param_pub.pairing);

            return rc;
        }

        rc = mosquitto_subscribe(mosq, NULL, mqttConnTopic, 0);

        while (run) {
            if (rc) {
                printf("Failed to connect to the broker! Trying reconnect ... \n");
                mosquitto_reconnect(mosq);
                sleep(10);
            }

            rc = mosquitto_loop(mosq, 0, 1);
        }
        
        mosquitto_destroy(mosq);
    }

    mosquitto_lib_cleanup();

    element_free(sys_param_pub.Kpub);
    element_free(sys_param_pub.P);
    element_free(sys_param_pri.Km);
    pairing_clear(sys_param_pub.pairing);

    return rc;
}
	
void PKGCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
    /*
        topic tree of the PKG
        MK/
            clientID/
                        sysparam
                        prikey
                        acksysparam
                        ackprikey
    */

    /* a selection of the pairing type should be added to the acknowledgement */

    printf("Receive message under topic: %s \n", msg->topic);
    printf("Received message length: %d \n", msg->payloadlen);    

    int rc = 0;
    char ack_topic[TOPIC_SIZE];

    const char delimiter[2] = "/";
    char* token1 = strtok(msg->topic, delimiter);
    char* token2 = strtok(NULL, delimiter);
    char* token3 = strtok(NULL, delimiter);
    
    if(!strcmp(token3, "sysparam")) {
        printf("********* IBE Retrieve System Parameters *********\n");
        memset(ack_topic, 0, TOPIC_SIZE);
        memcpy(ack_topic, base_topic, strlen(base_topic));
        memcpy(ack_topic + strlen(base_topic), token2, strlen(token2));
        strcat(ack_topic, "/acksysparam");
        
        #ifdef DBG_MSG
        printf("Seperated topic: %s, length: %ld \n", token2, strlen(token2));
        printf("Generated acknowledgement topic: %s \n", ack_topic);
        #endif

        /*
            system parameter acknowledgement message structure
            -----------------------------------
            | param type | Kpub | generator P |
            -----------------------------------
         */
        int ParamType_len = 1;
        int Kpub_len = element_length_in_bytes(sys_param_pub.Kpub);
        int P_len = element_length_in_bytes(sys_param_pub.P);
        int buffer_len = ParamType_len + Kpub_len + P_len + 1;
        unsigned char buffer[buffer_len];

        #ifdef DBG_MSG
        printf("Kpub_len:   %d\n", Kpub_len);
        printf("P_len:      %d\n", P_len);
        printf("buffer_len: %d\n", buffer_len);
        #endif

        memset(buffer, 'a', 1);
        element_to_bytes(buffer + ParamType_len, sys_param_pub.Kpub);
        element_to_bytes(buffer + ParamType_len + Kpub_len, sys_param_pub.P);
        buffer[buffer_len - 1] = '\0';

        #ifdef DBC_MSG
        printf("Message after converted to bytes:\n");
        PrintHEX(buffer, buffer_len - 32, buffer_len);
        #endif
        
        rc = mosquitto_publish(mosq, NULL, ack_topic, buffer_len, buffer, 0, false);

        if (rc != 0) {
            printf("Error: %s \n", mosquitto_strerror(rc));
        } else {
            printf("Published public system parameters! \n");
        }
    }
    
    if(!strcmp(token3, "prikey")) {
        printf("********* IBE Extract Private Key *********\n");
        memcpy(ack_topic, base_topic, strlen(base_topic));
        memcpy(ack_topic + strlen(base_topic), token2, strlen(token2));
        strcat(ack_topic, "/ackprikey");
        
        #ifdef DBG_MSG
        printf("Seperated topic: %s, length: %ld \n", token2, strlen(token2));
        printf("Generated acknowledgement topic: %s \n", ack_topic);
        #endif

        unsigned char *Didc;

        ibe_extract(&sys_param_pub, &sys_param_pri, msg->payload, Didc);

        int Didc_len = strlen(Didc);

        #ifdef DBC_MSG
        printf("Did after converted to bytes:\n");
        PrintHEX(buffer, Didc_len - 32, Didc_len);
        #endif
        
        rc = mosquitto_publish(mosq, NULL, ack_topic, Didc_len, Didc, 0, false);

        if (rc != 0) {
            printf("Error: %s \n", mosquitto_strerror(rc));
        } else {
            printf("Published encrypted nonce! \n");
        }
    }
}

void ibe_setup(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri) {

    pairing_init_set_buf(sys_param_pub->pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
    if (!pairing_is_symmetric(sys_param_pub->pairing)) pbc_die("pairing must be symmetric");

    /* initialize G1 elements */
    element_init_G1(sys_param_pub->P, sys_param_pub->pairing);
    element_init_G1(sys_param_pub->Kpub, sys_param_pub->pairing);
    
    /* initialize Zr elements */
    element_init_Zr(sys_param_pri->Km, sys_param_pub->pairing);
    
    element_random(sys_param_pub->P);
    element_random(sys_param_pri->Km);
    element_mul_zn(sys_param_pub->Kpub, sys_param_pub->P, sys_param_pri->Km);
	
    #ifdef DBG_MSG
    element_printf("++Km: %B\n", sys_param_pri->Km);
	element_printf("++P:  %B\n", sys_param_pub->P);
	element_printf("++Kpub: %B\n\n", sys_param_pub->Kpub);
    #endif
}

void ibe_extract(ibe_sys_param_public_t *sys_param_pub, ibe_sys_param_private_t *sys_param_pri, unsigned char *id, unsigned char *Didc) {

    unsigned char hash[20] = {0};
    element_t Qid, Did;

    element_init_G1(Qid, sys_param_pub->pairing);
    element_init_G1(Did, sys_param_pub->pairing);
    
    if(SHA1(id, 20, hash) == NULL) {
        printf("failed to hash receiver ID!\n");
    }

    element_from_hash(Qid, hash, strlen(hash));
    element_mul_zn(Did, Qid, sys_param_pri->Km);
    element_to_bytes(Didc, Did);

    element_clear(Qid);
    element_clear(Did);
}

void ibe_encryption(ibe_sys_param_public_t *sys_param_pub, unsigned char* rec_id, unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher, unsigned char *Uc) {

    unsigned char *gs = NULL, hash[20] = {0};
    element_t r, U, gid, Qid;

    /* Initialize parameters */
    element_init_G1(Qid, sys_param_pub->pairing);
    element_init_G1(U, sys_param_pub->pairing);
    element_init_GT(gid, sys_param_pub->pairing);
    element_init_Zr(r, sys_param_pub->pairing);

    element_random(r);

    /* Hash the receiver ID */
    if(SHA1(rec_id, 20, hash) == NULL) {
        printf("failed to hash receiver ID!\n");
    }

    element_from_hash(Qid, hash, strlen(hash));
    //element_from_hash(Qid, hash, 20);
    
    element_mul_zn(U, sys_param_pub->P, r);
    element_pairing(gid, Qid, sys_param_pub->Kpub);
    element_pow_zn(gid, gid, r);
    gs = (unsigned char *)malloc(element_length_in_bytes(gid));
    element_to_bytes(gs, gid);

    int gidlen = element_length_in_bytes(gid);
    printf("gid length: %d\n", gidlen);
    
    if(SHA1(gs, 128, hash) == NULL) {
        printf("failed to hash gs!\n");
    }

    int j = 0;
    for (int i = 0; i < MSG_SIZE; i++) {
        if (j >= 20) {
            j = 0;
        }
        cipher[i] = plaintext[i] ^ hash[j];
        j++;
    }

    element_to_bytes(Uc, U);

    free(gs);
    element_clear(r);
    element_clear(U);
    element_clear(gid);
    element_clear(Qid);
}

void ibe_decryption(ibe_sys_param_public_t *sys_param_pub, unsigned char *ciphertext, size_t ciphertext_len, \
                         unsigned char *Uc, unsigned char *Didc, unsigned char *plaintext) {

    unsigned char *gs = NULL, hash[20] = {0};
    element_t U, Did, xt;

    /* Initialize the G1 element */
    element_init_G1(U, sys_param_pub->pairing);
    element_init_G1(Did, sys_param_pub->pairing);

    /* Initialize the GT element */
    element_init_GT(xt, sys_param_pub->pairing);

    element_from_bytes(U, Uc);
    element_from_bytes(Did, Didc);

    element_pairing(xt, Did, U);
    gs = (unsigned char *)malloc(element_length_in_bytes(xt));
    element_to_bytes(gs, xt);

    if(SHA1(gs, 128, hash) == NULL) {
        printf("failed to hash receiver ID!\n");
    }

    int j = 0;
    for (int i = 0; i < MSG_SIZE; i++) {
        if (j >= 20) {
            j = 0;
        }
        plaintext[i] = ciphertext[i] ^ hash[j];
        j++;
    }

    free(gs);
    element_clear(U);
    element_clear(Did);
    element_clear(xt);
}

void PrintHEX(unsigned char* str, int start_byte, int end_byte) {

    for (int i = start_byte; i < end_byte; ++i) {
        printf("%.2X ", str[i]);
    }

    printf("\n");
}
