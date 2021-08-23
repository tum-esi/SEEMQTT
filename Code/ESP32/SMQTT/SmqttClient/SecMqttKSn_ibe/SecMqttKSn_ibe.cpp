#include "SecMqttKSn_ibe.h"


SecMqtt::SecMqtt() : PubSubClient() {
    this->_base_topic = "MK";
    this->_cred = NULL;
    this->_iot_pk_key = NULL;
    this->_iot_pk_key_size = 0;
    this->_iot_pr_key = NULL;
    this->_iot_pr_key_size = 0;
    this->_sk_counter = 0;
    this->_ibe_mode = false;
    this->_sss_mode = false;
    this->_ksn_mode = false;
    this->_secmqtt_state = SECMQTT_KS_DISCONNECTED;
}

SecMqtt::SecMqtt(Client& client) : PubSubClient(client) {
    this->_base_topic = "MK";
    this->_cred = NULL;
    this->_iot_pk_key = NULL;
    this->_iot_pk_key_size = 0;
    this->_iot_pr_key = NULL;
    this->_iot_pr_key_size = 0;
    this->_iot_credntial = NULL ;
    this->_iot_credntial_size =0 ;
    this->_sk_counter = 0;
    this->_ibe_mode = false;
    this->_sss_mode = false;
    this->_ksn_mode = false;
    this->_secmqtt_state = SECMQTT_KS_DISCONNECTED;
}


void SecMqtt::SecConnect(const char *client_id) {

    /* connect client to broker */

    time_info.t_s = micros();
    connect(client_id);
    while(!connected()) {}

    time_info.t_connect = micros() - time_info.t_s;

    if (this->_secmqtt_state == SECMQTT_KS_CONNECTED) {

    } else {

        /**
         *  1. generate a random topic under base topic (MK)
         *  2. subscribe to acknowledgement topic
         *  3. publish encrypted symmetric key to Key Store
         *  4. check acknowledgement from Key Store
         *  5. OK? publish encrypted session key to Key Store
         *  6. session key expired? negotiate a new session key
         */

        /* the number of key stores out there */
        unsigned char keync[KSN_NUM][2*BLOCK_SIZE];
        unsigned char keync_enc[KSN_NUM][512];

        #ifdef DBG_MSG
        Serial.println("*************************************");
        Serial.println("*   Phase 1: negotiate master key   *");
        Serial.println("*************************************");
        Serial.println("Phase 1: Publishing Epk(K1) and nonce");
        #endif

        /* setup key store list */
        #ifdef DBG_MSG
        Serial.println("++++++ setting up key stores' list ++++++");
        #endif

        time_info.t_p11s = micros();

        for (int i = 0; i < KSN_NUM; i++) {
            sym_key_generator(ksn_list[i].masterkey);
            esp_fill_random(ksn_list[i].nonce, BLOCK_SIZE);

            memcpy(keync[i], ksn_list[i].masterkey, BLOCK_SIZE);
            memcpy(keync[i] + BLOCK_SIZE, ksn_list[i].nonce, BLOCK_SIZE);

            #ifdef DBG_MSG
            Serial.println("KS[%d]. masterkey: ");
            PrintHEX (ksn_list[i].masterkey, BLOCK_SIZE);
            Serial.println("KS[%d]. nonce: ");
            PrintHEX (ksn_list[i].nonce, BLOCK_SIZE);
            #endif
        }

        /* create key store connection topic */
        String random_topic = "ClientID-";
        random_topic += String(random(0xffff), HEX);
        strcpy(this->_conn_topic, String((this->_base_topic + "/" + random_topic + "/" + "value")).c_str());
        strcpy(this->_ack_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "ack")).c_str());
        strcpy(this->_sk_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "sk")).c_str());
        //strcpy(this->_sk_ack_topic, String((String(this->_base_topic) + "/" + random_topic + "/" + "skack")).c_str());

        using namespace std::placeholders;
        auto callback = std::bind(&SecMqtt::SecCallback, this, _1, _2, _3);
        setCallback(callback);

        #ifdef DBG_MSG
        Serial.print("conn topic: ");
        Serial.println(this->_conn_topic);
        Serial.print("ack topic: ");
        Serial.println(this->_ack_topic);
        Serial.print("iot pk key: ");
        PrintHEX((unsigned char *)this->_iot_pk_key, this->_iot_pk_key_size);
        #endif

        /* - create key store value and ack topic for all n key stores
         *   e.g.
         *      key store 1 value topic: MK/ClientID-xxxx/value/1
         *      key store 2 value topic: MK/ClientID-xxxx/value/2
         *      ...
         *      key store 1 ack topic: MK/ClientID-xxxx/ack/1
         *      key store 2 ack topic: MK/ClientID-xxxx/ack/2
         *      ...
         * - publish Enc(msrKpubc-ksi||nci, pk_ksi)||iot_pk_key to key store value topic
         * - subscribe to key store acknowledge topic
         */
        for (int i = 0; i < KSN_NUM; i++) {
            sprintf(ksn_list[i].value_topic, "%s/%d", this->_conn_topic, i + 1);
            sprintf(ksn_list[i].ack_topic, "%s/%d", this->_ack_topic, i + 1);

            subscribe(ksn_list[i].ack_topic);

            #ifdef DBG_MSG
            Serial.printf("subscribe to topic: %s\n",ksn_list[i].ack_topic);
            #endif

            if (this->_ibe_mode) {
                unsigned char Uc[ELEMENT_LEN];

                time_info.t_s = micros();
                int Uc_len = ibe_encryption(i, keync[i], 2*BLOCK_SIZE, keync_enc[i], Uc);
                time_info.t_enc = micros() - time_info.t_s;

                #ifdef DBG_MSG
                Serial.println("keync_enc: ");
                PrintHEX(keync_enc[i], 32);

                Serial.printf("Uc lenght: %d\n", Uc_len);
                Serial.println("Uc: ");
                PrintHEX(Uc, Uc_len);
                #endif

                int msg_len = 2*BLOCK_SIZE + Uc_len + this->_iot_pk_key_size;
                unsigned char msg[msg_len];

                memcpy(msg, keync_enc[i], 2*BLOCK_SIZE);
                memcpy(msg + 2*BLOCK_SIZE, Uc, Uc_len);
                memcpy(msg + 2*BLOCK_SIZE + Uc_len, this->_iot_pk_key, this->_iot_pk_key_size);

                time_info.t_s = micros();
                beginPublish(ksn_list[i].value_topic, msg_len, false);
                write((byte *)msg, msg_len);
                endPublish();
                time_info.t_send_pk = micros() - time_info.t_s;

                time_info.t_recvs[i] = micros();
            }
        }

        time_info.t_p11 = micros() - time_info.t_p11s;

        /* wait until received correct nonce from Key Store */
        unsigned long stime = millis();
        unsigned long etime;

        #ifdef DBG_MSG
        Serial.println("Waiting for Key Stores to response ...");
        #endif

        while(this->_secmqtt_state != SECMQTT_CONNECT_GOOD_NONCE) {
            etime = millis();
            if ((etime - stime) > SECMQTT_TIMEOUT) {
                cout << "Phase 1 connection timeout!" << endl;
                this->_secmqtt_state = SECMQTT_CONNECTION_TIMEOUT;
                return ;
            }

            if(this->_secmqtt_state == SECMQTT_CONNECT_BAD_NONCE) {
                Serial.println(this->_secmqtt_state);
                return ;
            }

            loop();
        }

        #ifdef DBG_MSG
        Serial.println("End Phase 1 successfully!");
        #endif

        this->_secmqtt_state = SECMQTT_KS_CONNECTED;
        time_info.t_p2 = micros() - time_info.t_recvs[0];
    }

    SecSessionKeyUpdate();

    return ;
}

// TODO
// aes gcm mode

void SecMqtt::SecPublish(const char* topic, const unsigned char* msg, size_t msg_len) {

    /**
     * Phase 3: Publish message encrypted using session key (Esk(msg))
     */
    #ifdef DBG_MSG
    Serial.println("**************************************************");
    Serial.println("*   Phase 3: Publish message using session key   *");
    Serial.println("**************************************************");
    #endif

    /* check if Key Store connection successful */
    if(this->_secmqtt_state != SECMQTT_KS_CONNECTED) {
        return ;
    }

    /* check if session key needs to be updated */
    if(this->_sk_counter >= MAX_SK_USAGE) {
        SecSessionKeyUpdate();
    }

    unsigned long t_b = micros();

    #ifdef DBG_MSG
    Serial.println("Phase 3: Publishing encrypted message ...");
    #endif

    unsigned char *Eskmsg = (unsigned char *) malloc(msg_len);
    unsigned char iv[BLOCK_SIZE], iv_tmp[BLOCK_SIZE];
    esp_fill_random(iv, BLOCK_SIZE);
    memcpy(iv_tmp, iv, BLOCK_SIZE);

    const char *auth_msg = "TUM IoT Security";
    const char *auth_msg_fake = "TUM IoT Hacker";
    unsigned char tag[BLOCK_SIZE];
    size_t tag_len = BLOCK_SIZE;

    aes_gcm_encryption(msg, msg_len, this->_session_key, iv_tmp, Eskmsg, (const unsigned char*)auth_msg, (size_t)strlen(auth_msg), tag, tag_len);
    #ifdef DBG_MSG
    Serial.print("plantext user message: ");
    PrintHEX((unsigned char*)msg, msg_len);
    Serial.print("Encrypted user message: ");
    PrintHEX(Eskmsg, msg_len);
    #endif

    // TODO
    // add tag and tag_len to the Eskmsg
    beginPublish(topic,  msg_len, true);
    write((byte*)Eskmsg, msg_len);
    endPublish();
    this->_sk_counter += 1;

    #ifdef DBG_MSG
    Serial.printf("End Phase 3! #%d \n", this->_sk_counter);
    #endif

    unsigned long t_e = micros();

    #ifdef TIME_MSG
    Serial.printf("time publish message: %lu (us)\n", t_e - t_b);
    #endif

    #ifdef DBG_MSG
    unsigned char outmsg[msg_len];
    aes_gcm_decryption(Eskmsg, msg_len, this->_session_key, iv_tmp, outmsg, (const unsigned char*)auth_msg_fake, (size_t)strlen(auth_msg_fake),
            tag, tag_len);
    Serial.printf("Decrypted user message: ");
    PrintHEX(outmsg, msg_len);
    #endif

    free(Eskmsg);
}

void SecMqtt::SecSessionKeyUpdate() {

    /**
     * Send the shares genrated based on the message key to Keystores
     *
     *
           ***********************
           *                |    |
           *  En(sk, hc, ht)| iv |
           *                |    |
           * *********************
    */
    unsigned long t_b = micros();

    #ifdef DBG_MSG
    Serial.println("**************************************");
    Serial.println("*   Phase 2: negotiate session key   *");
    Serial.println("**************************************");
    #endif

    int rc = 0;
    this->_sk_counter = 0;
    unsigned char iv[BLOCK_SIZE];
    unsigned char iv_tmp[BLOCK_SIZE];

    sym_key_generator(this->_session_key);

    #ifdef DBG_MSG
    Serial.print("Session Key: ");
    PrintHEX(this->_session_key, BLOCK_SIZE);
    #endif

    if (this->_ksn_mode) {
        time_info.t_s = micros();
        secmqtt_key_split();
        time_info.t_keysplit = micros() - time_info.t_s;
    }

    if (this->_sss_mode) {
        time_info.t_s = micros();
        secmqtt_sss_split();
        time_info.t_keysplit = micros() - time_info.t_s;
    }

    rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)DATA, strlen(DATA), this->_iot_data_hash);
    if (rc != 0) {
        Serial.println("failed to hash DATA!");
    } else {
        #ifdef DBG_MSG
        Serial.println("successfully hash DATA!");
        #endif
    }

    for (int i = 0; i< KSN_NUM; i++) {
        if (this->_ksn_mode) {
            #ifdef DBG_MSG
            Serial.printf("shares: %d\n", i);
            PrintHEX(this->_shares[i], BLOCK_SIZE);
            #endif

            unsigned char msg[BLOCK_SIZE + 2*HASH_LEN];
            unsigned char msg_enc[BLOCK_SIZE + 2*HASH_LEN];
            size_t m_len = BLOCK_SIZE + 2*HASH_LEN + BLOCK_SIZE;
            unsigned char buffer[m_len];

            esp_fill_random(iv, BLOCK_SIZE);
            memcpy(iv_tmp, iv, BLOCK_SIZE);

            memcpy(msg, this->_shares[i], BLOCK_SIZE);
            memcpy(msg + BLOCK_SIZE, this->_iot_pk_key_hash, HASH_LEN);
            memcpy(msg + BLOCK_SIZE + HASH_LEN, this->_iot_data_hash, HASH_LEN);

            time_info.t_s = micros();
            rc = aes_encryption(msg, BLOCK_SIZE + 2*HASH_LEN, ksn_list[i].masterkey, iv_tmp, msg_enc);
            time_info.t_p3enc = micros() - time_info.t_s;
            if (rc != 0) {
                Serial.println("Failed to encrypt the session key!");
                return ;
            } else {
                #ifdef DBG_MSG
                Serial.println("succeed to encrypt session key!");
                #endif
            }

            memcpy(buffer, msg_enc, BLOCK_SIZE + 2*HASH_LEN);
            memcpy(buffer + BLOCK_SIZE + 2*HASH_LEN, iv, BLOCK_SIZE);

            sprintf(ksn_list[i].sk_topic, "%s/%d", this->_sk_topic, i + 1);

            #ifdef DBG_MSG
            Serial.print("publish key shares to KeyStores under sk topic: ");
            Serial.println(ksn_list[i].sk_topic);
            #endif

            time_info.t_s = micros();
            beginPublish(ksn_list[i].sk_topic, m_len, false);
            write((byte *)buffer, m_len);
            endPublish();
            time_info.t_p3send = micros() - time_info.t_s;
        }

        if (this->_sss_mode) {

            #ifdef DBG_MSG
            Serial.printf("sss shares: %d\n", i);
            Serial.printf("%.*s\n", SSS_SIZE, this->_shares_sss[i]);
            #endif

            /* msg has to be multiple of 16 bytes aes cbc mode
             * we add 09 as padding
             * */
            int padding = 16  - (SSS_SIZE + 2*HASH_LEN)%16;
            int msglen = SSS_SIZE + 2*HASH_LEN + padding;
            unsigned char msg[msglen];
            unsigned char msg_enc[msglen];
            size_t m_len = 3*BLOCK_SIZE + 2*HASH_LEN + BLOCK_SIZE;
            unsigned char *buffer = (unsigned char *)malloc(m_len);

            esp_fill_random(iv, BLOCK_SIZE);
            memcpy(iv_tmp, iv, BLOCK_SIZE);

            memcpy(msg, this->_shares_sss[i], SSS_SIZE);
            memcpy(msg + SSS_SIZE, this->_iot_pk_key_hash, HASH_LEN);
            memcpy(msg + SSS_SIZE + HASH_LEN, this->_iot_data_hash, HASH_LEN);
            memset(msg + SSS_SIZE + 2*HASH_LEN, padding, padding);

            time_info.t_s = micros();
            rc = aes_encryption(msg, SSS_SIZE + 2*HASH_LEN + 9, ksn_list[i].masterkey, iv_tmp, msg_enc);
            time_info.t_p3enc = micros() - time_info.t_s;
            if (rc != 0) {
                Serial.println("Failed to encrypt the session key!");
                return ;
            } else {
                #ifdef DBG_MSG
                Serial.println("succeed to encrypt session key!");
                #endif
            }

            memcpy(buffer, msg_enc, 3*BLOCK_SIZE + 2*HASH_LEN);
            memcpy(buffer + 3*BLOCK_SIZE + 2*HASH_LEN, iv, BLOCK_SIZE);

            sprintf(ksn_list[i].sk_topic, "%s/%d", this->_sk_topic, i + 1);

            #ifdef DBG_MSG
            Serial.print("publish key shares to KeyStores under sk topic: ");
            Serial.println(ksn_list[i].sk_topic);
            Serial.printf("Share [%d]\n", i );
            PrintHEX(msg, SSS_SIZE);
            #endif

            time_info.t_s = micros();
            beginPublish(ksn_list[i].sk_topic, m_len, false);
            write((byte *)buffer, m_len);
            endPublish();
            time_info.t_p3send = micros() - time_info.t_s;

            free(buffer);
        }
    }

    unsigned long t_e = micros();

    /* transmit the credential */
    int siglength = 128;
    int maxlen = 2048;

    char *credential = (char *) malloc(maxlen);
    char *condition = "(app_domain==\"KS\")-> { (Process) < 2500) -> _MAX_TRUST";

    sprintf(credential, "KeyNote-Version: 2\nAuthorizer: \"%s\" \nLicensees: \"%s\" \nConditions:%s\nSignature: ", this->_iot_pk_key, ksn_list[0].ibe_id, condition);

    #ifdef DBG_MSG
    Serial.printf("credential: %s\n", credential);
    #endif

    unsigned char signature[siglength];

    /* sign the credential */
    time_info.t_s = micros();
    //rsa_sign((unsigned char *)credential, strlen(credential), signature);
    char * result =  kn_rsa_sign_md5((unsigned char *)credential,strlen(credential),  this->_iot_pr_key, this->_iot_pr_key_size, signature);
    char * kn_sign = kn_encode_signture("",result) ;
    time_info.t_cred_sign = micros() - time_info.t_s;

    #ifdef DBG_MSG
    Serial.println("signed credential: ");
    Serial.println(kn_sign );
    #endif

    int cr_len = siglength + strlen(credential);
    unsigned char cr_buffer[cr_len];

    memcpy(cr_buffer, signature, siglength);
    memcpy(cr_buffer + siglength, credential, strlen(credential));

    beginPublish(CR,  cr_len, false);
    write((byte*)cr_buffer, cr_len);
    endPublish();

    #ifdef DBG_MSG
    Serial.println("signed credential: ");
    PrintHEX(cr_buffer, cr_len);
    #endif

    free(credential);

    unsigned long t_ee = micros();

    time_info.t_cred = t_ee - t_e;
    time_info.t_p3 = t_e - t_b;

    #ifdef TIME_MSG
    Serial.printf("time key split: \t%lu (us)\n", time_info.t_keysplit);
    Serial.printf("time key enc: \t\t%lu (us)\n", time_info.t_p3enc);
    Serial.printf("time key send: \t\t%lu (us)\n", time_info.t_p3send);
    Serial.printf("time sign cred: \t%lu (us)\n", time_info.t_cred_sign);
    Serial.printf("time cred: \t\t%lu (us)\n", time_info.t_cred);
    Serial.printf("time phase 3: \t\t%lu (us)\n", time_info.t_p3);
    Serial.printf("time phase 3 + cred: \t%lu (us)\n", time_info.t_p3 + time_info.t_cred);
    #endif
}

void SecMqtt::SecCallback(char* topic, uint8_t* payload, unsigned int payload_length) {

    int rc = 0;

    #ifdef DBG_MSG
    Serial.print("message received under topic:");
    Serial.println(topic);
    #endif

    const char *delimiter = "/";
    char *token1 = strtok(topic, delimiter);
    char *token2 = strtok(NULL, delimiter);
    char *token3 = strtok(NULL, delimiter);
    char *token4 = strtok(NULL, delimiter);

    if(!strcmp(token3, "ack")) {
        /* payload = hash(encrypted nonce + iv) */

        int ks_id = atoi(token4) - 1;

        time_info.t_recv[ks_id] = micros() - time_info.t_recvs[ks_id];

        unsigned char msg_enc[HASH_LEN];
        unsigned char msg_dec[HASH_LEN];
        unsigned char hpk[HASH_LEN];
        unsigned char iv[BLOCK_SIZE];

        memcpy(msg_enc, payload, HASH_LEN);
        memcpy(iv, payload + HASH_LEN, BLOCK_SIZE);

        #ifdef DBG_MSG
        Serial.print("Received iv: ");
        PrintHEX(iv, BLOCK_SIZE);
        Serial.print("Master key used to decrypt: ");
        PrintHEX(ksn_list[ks_id].masterkey, BLOCK_SIZE);
        Serial.print("Nonce:");
        PrintHEX(ksn_list[ks_id].nonce, BLOCK_SIZE);
        #endif

        time_info.t_s = micros();
        rc = aes_decryption(msg_enc, HASH_LEN, ksn_list[ks_id].masterkey, iv, msg_dec);
        time_info.t_dec = micros() - time_info.t_s;

        #ifdef DBG_MSG
        Serial.print("decrypted message: ");
        PrintHEX(msg_dec, HASH_LEN);
        #endif

        if (rc != 0) {
            cout << "Failed to decrypt the nonce! \n";
        } else {
            for (int i = 0; i < HASH_LEN; i++) {
                hpk[i] = ksn_list[ks_id].nonce[i%16] ^ msg_dec[i];
            }

            #ifdef DBG_MSG
            Serial.print("received   hpk: ");
            PrintHEX(hpk, HASH_LEN);
            Serial.print("calculated hpk: ");
            PrintHEX(this->_iot_pk_key_hash, HASH_LEN);
            #endif

            if(!memcmp(hpk, this->_iot_pk_key_hash, HASH_LEN)) {
                ksn_list[ks_id].verified = true;

                #ifdef DBG_MSG
                Serial.println("hpk and nonce are equal\n");
                #endif

                if (secmqtt_check_all_ksn_nonce_stat()) {
                    this->_secmqtt_state = SECMQTT_CONNECT_GOOD_NONCE;
                }
            } else {
                Serial.println("hpk and nonce are not equal\n");
                ksn_list[ks_id].verified = false;
                this->_secmqtt_state = SECMQTT_CONNECT_BAD_NONCE;
            }
        }
    }
}

void SecMqtt::secmqtt_key_split() {

    /* create KSN_NUM -1 random keys */
    for (int i = 0; i < KSN_NUM - 1; i++) {
        esp_fill_random(this->_shares[i], BLOCK_SIZE);
    }

    /* create the KSN_NUMth shares */
    for (int j = 0; j < BLOCK_SIZE; j++) {
        this->_shares[KSN_NUM-1][j] = this->_session_key[j];
        for (int i = 0; i < KSN_NUM - 1; i++) {
            this->_shares[KSN_NUM-1][j] = this->_shares[KSN_NUM-1][j] ^ this->_shares[i][j];
        }
    }
}

void SecMqtt::secmqtt_sss_split() {

    /* using shamir secret sharing */
    unsigned char str_tmp[BLOCK_SIZE + 1];
    memcpy(str_tmp, this->_session_key, BLOCK_SIZE);
    str_tmp[BLOCK_SIZE] = '\0';

    char *share = generate_share_strings((char *)str_tmp, KSN_NUM, SSS_T);

    for (int i = 0; i < KSN_NUM; i++) {
        memcpy(this->_shares_sss[i], share + i*SSS_SIZE, SSS_SIZE);
    }
}

/* call plattform-specific  TRNG driver */
int SecMqtt::myrand(void *rng_state, unsigned char *output, size_t len) {
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

void SecMqtt::rsa_sign(const unsigned char * input, size_t P_SIZE, unsigned char * sign) {

    mbedtls_pk_context pr;
    mbedtls_pk_init(&pr);
    int error_code_pr;

    error_code_pr =  mbedtls_pk_parse_key(&pr, this->_iot_pr_key, this->_iot_pr_key_size, NULL, NULL);
    if(error_code_pr != 0) {
        cout << "READING PRIVATE KEY NOT SUCCESSFULL, error code: " << error_code_pr << endl;
        mbedtls_pk_free(&pr);
        return ;
    } else {
        /* compute the SHA-256 hash for the input*/
        unsigned char hash[32];
        int hash_ret;

        hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), input, P_SIZE, hash);
        if( hash_ret != 0 ) {
            cout << "Calculating HASH not SUCCESSFULL, error code: " << hash_ret << endl;
            mbedtls_pk_free(&pr);
            return ;
        } else {
            int sign_ret;
            size_t olen = 0;

            sign_ret = mbedtls_pk_sign(&pr, MBEDTLS_MD_SHA256, hash, 0, sign, &olen, myrand, NULL);
            if (sign_ret != 0) {
                cout << "Sign not SUCCESSFULL" << endl;
                mbedtls_pk_free(&pr);
                return;
            } else {
                mbedtls_pk_free(&pr);
                return;
            }
        }
    }
}

void SecMqtt::sym_key_generator(unsigned char* symkey) {

    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    char *pers = "aes generate key";
    int ret;

    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, myrand, &entropy,
        (unsigned char *) pers, strlen( pers ) ) ) != 0 ) {
        printf( " failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
        return;
    }

    if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, symkey, BLOCK_SIZE ) ) != 0 ) {
        printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
        return;
    }
}



/* aes encryption cbc */
int SecMqtt::aes_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output) {

    int rc = 0;

    mbedtls_aes_context aes;
    mbedtls_aes_init( &aes );
    mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, BLOCK_SIZE * 8);
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, input_len, iv, input, output);
    mbedtls_aes_free( &aes );

    return rc;
}

/* aes decrytion cbc */
int SecMqtt::aes_decryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output) {

    int rc = 0;

    mbedtls_aes_context aes;
    mbedtls_aes_init( &aes );
    mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, BLOCK_SIZE * 8);
    rc = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, input_len, iv, input, output);
    mbedtls_aes_free( &aes );

    return rc;
}

int SecMqtt::aes_gcm_encryption(const unsigned char* input, size_t input_len, const unsigned char* key,
        unsigned char* iv, unsigned char* output, const unsigned char* add, size_t add_len, unsigned char* tag, size_t tag_len) {

    int rc = 0;
    size_t iv_len = BLOCK_SIZE;
    unsigned int keybits = BLOCK_SIZE * 8;

    mbedtls_gcm_context aes;
    mbedtls_gcm_init(&aes);
    mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, key, keybits);

    rc = mbedtls_gcm_crypt_and_tag(&aes, MBEDTLS_GCM_ENCRYPT, input_len, iv, iv_len, add, add_len, input, output, tag_len, tag);
    if (rc != 0) {
        Serial.printf("failed \n ! mbedtls_gcm_crypt_and_tag returned %d\n", rc);
        return -1;
    }

    mbedtls_gcm_free(&aes);

    Serial.printf(">>>>>>>>>>>>>>>>>> aes gcm encryption successful!\n");
    return 0;
}

int SecMqtt::aes_gcm_decryption(const unsigned char* input, size_t input_len, const unsigned char* key,
        unsigned char* iv, unsigned char* output, const unsigned char* add, size_t add_len, unsigned char* tag, size_t tag_len) {

    int rc = 0;
    size_t iv_len = BLOCK_SIZE;
    unsigned int keybits = BLOCK_SIZE * 8;

    mbedtls_gcm_context aes;
    mbedtls_gcm_init(&aes);
    mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, key, keybits);

    rc = mbedtls_gcm_auth_decrypt(&aes, input_len, iv, iv_len, add, add_len, tag, tag_len, input, output);
    if (rc != 0) {
        if (rc == MBEDTLS_ERR_GCM_AUTH_FAILED) {
            Serial.printf("failed \n ! mbedtls_gcm_auth_decrypt authentication failed!\n", rc);
            return -1;
        }
        Serial.printf("failed \n ! mbedtls_gcm_auth_decrypt returned %d\n", rc);
        return -1;
    }

    mbedtls_gcm_free(&aes);

    Serial.printf(">>>>>>>>>>>>>>>>>> aes gcm decryption successful!\n");
    return 0;
}
int SecMqtt::get_state() {
    return this->_secmqtt_state;
}

void SecMqtt::secmqtt_set_cred(const unsigned char* cred) {
    this->_cred = cred;
}

void SecMqtt::secmqtt_set_iot_pk_key(const unsigned char* key, int key_size) {

    this->_iot_pk_key = key;
    this->_iot_pk_key_size = key_size;
    int hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key, key_size, this->_iot_pk_key_hash);
    if (hash_ret != 0) {
        Serial.println("failed to hash iot node public key!");
    } else {
        Serial.println("successfully hash iot node public key!");
        #ifdef DBG_MSG
        Serial.print("caculated hpk: ");
        PrintHEX(this->_iot_pk_key_hash, HASH_LEN);
        #endif
    }
}

void SecMqtt::secmqtt_set_iot_pr_key(const unsigned char* key, int key_size) {

    this->_iot_pr_key = key;
    this->_iot_pr_key_size = key_size;
}

void SecMqtt::secmqtt_set_iot_credential(const unsigned char* cre, int cre_size) {
  #ifdef DBG_MSG
  Serial.println("*************************************");
  Serial.println("*  Setting up the credentila  *");
  Serial.println("*************************************");
  Serial.printf("this->_iot_pr_key_size = %d \n",  this->_iot_pr_key_size );
  #endif
    unsigned char * resu_cre ;
    int r_len = 0;
    kn_GenCredential(cre,cre_size, this->_iot_pr_key, this->_iot_pr_key_size, &resu_cre, &r_len);
    this->_iot_credntial = resu_cre;
    this->_iot_credntial_size = r_len;
    #ifdef DBG_MSG
    Serial.printf(" credential lenght = %d\n",this->_iot_credntial_size );
    Serial.println("*************************************");
    Serial.printf(" credential lenght = %s\n",this->_iot_credntial );
    Serial.println("*************************************");
    #endif
}

bool SecMqtt::secmqtt_check_all_ksn_nonce_stat() {

    bool rc = true;
    for (int i = 0; i < KSN_NUM; i++) {
        rc = rc && ksn_list[i].verified;
    }
    return rc;
}

void SecMqtt::secmqtt_set_enc_mode(char *mode) {

    if (!strncmp(mode, "ibe", 3)) {
        this->_ibe_mode = true;

        /* initialize IBE system parameters */
        pairing_init_set_buf(ibe_param_pub.pairing, TYPEA_PARAMS, strlen(TYPEA_PARAMS));
        if (!pairing_is_symmetric(ibe_param_pub.pairing)) pbc_die("pairing must be symmetric");

        element_init_G1(ibe_param_pub.Kpub, ibe_param_pub.pairing);
        element_init_G1(ibe_param_pub.P, ibe_param_pub.pairing);
        element_init_Zr(ibe_param_pri.Km, ibe_param_pub.pairing);

        element_from_bytes(ibe_param_pub.Kpub, kpub_t);
        element_from_bytes(ibe_param_pub.P, p_t);

        /* calculate one-time cost Qid */
        int rc = 0;
        unsigned char hash[HASH_LEN] = {0};

        for (int i = 0; i < KSN_NUM; i++) {
            element_init_G1(ksn_list[i].Qid, ibe_param_pub.pairing);

            rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)ksn_list[i].ibe_id, ksn_list[i].ibe_id_len, hash);
            if (rc != 0) {
                Serial.println("failed to hash key store ibe ID!");
            } else {
                #ifdef DBG_MSG
                Serial.println("successfully hash key store ibe ID!");
                #endif
            }

            element_from_hash(ksn_list[i].Qid, hash, HASH_LEN);
        }
    }
}

void SecMqtt::secmqtt_set_ibe_id(const char*id, int idlen, int ksid) {
    ksn_list[ksid-1].ibe_id_len=idlen;
    memcpy(ksn_list[ksid-1].ibe_id, id, idlen);
    Serial.printf("ibe key size:%d\n", idlen);
    Serial.println(ksn_list[ksid-1].ibe_id);
}

int SecMqtt::ibe_encryption(int ks_id, unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher, unsigned char *Uc) {

    int rc = 0;
    unsigned char *gs = NULL, hash[HASH_LEN] = {0};
    element_t r, U, gid;

    /* Initialize parameters */
    element_init_G1(U, ibe_param_pub.pairing);
    element_init_GT(gid, ibe_param_pub.pairing);
    element_init_Zr(r, ibe_param_pub.pairing);

    element_random(r);
    element_mul_zn(U, ibe_param_pub.P, r);
    element_pairing(gid, ksn_list[ks_id].Qid, ibe_param_pub.Kpub);
    element_pow_zn(gid, gid, r);

    gs = (unsigned char *)malloc(element_length_in_bytes(gid));
    element_to_bytes(gs, gid);

    rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)gs, element_length_in_bytes(gid), hash);
    if (rc != 0) {
        Serial.println("failed to hash key store gs!");
    } else {
        #ifdef DBG_MSG
        Serial.println("successfully hash key store gs!");
        #endif
    }

    int j = 0;
    for (int i = 0; i < plaintext_len; i++) {
        if (j >= HASH_LEN) {
            j = 0;
        }
        cipher[i] = plaintext[i] ^ hash[j];
        j++;
    }

    element_to_bytes(Uc, U);
    int Uc_len = element_length_in_bytes(U);

    free(gs);
    element_clear(r);
    element_clear(U);
    element_clear(gid);

    return Uc_len;
}

void SecMqtt::secmqtt_set_secret_share_mode(char *mode) {

    Serial.printf("set key sharing mode to %s\n", mode);

    if (!strncmp(mode, "sss", 3)) {
        this->_sss_mode = true;
    }

    if (!strncmp(mode, "ksn", 3)) {
        this->_ksn_mode = true;
    }
}

void SecMqtt::PrintHEX(unsigned char* arr, int arr_size) {
    for (int i = 0; i < arr_size; ++i) {
        Serial.print(arr[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}


unsigned char kpub_t[ELEMENT_LEN] = {
0x33, 0x14, 0x25, 0x4B, 0x28, 0x8C, 0xEB, 0xDD, 0x0B, 0x3F, 0xDE, 0xCB, 0xF3, 0x61, 0x97, 0x77, 0x57, 0x42, 0xEE, 0x44, 0x61, 0xB4, 0x16, 0xDB, 0x4F, 0xA9, 0x7F, 0xCE, 0x80, 0x36, 0xD3, 0xCF, 0xB5, 0x1D, 0x3D, 0xD7, 0xC7, 0x78, 0x54, 0x1C, 0x1C, 0xF5, 0x10, 0x98, 0x08, 0x37, 0x78, 0x01, 0xC6, 0xB0, 0x87, 0x27, 0xC0, 0xC8, 0xA2, 0xE9, 0x7B, 0x2A, 0x67, 0x34, 0x2E, 0x90, 0x69, 0x04, 0x2A, 0xB8, 0x61, 0x04, 0xEE, 0x91, 0x9A, 0x3D, 0x26, 0xDA, 0x04, 0x20, 0x08, 0xD6, 0xF7, 0x19, 0x00, 0x15, 0x63, 0xF4, 0x65, 0x2F, 0x28, 0xE3, 0x38, 0xAB, 0xD9, 0x79, 0x60, 0xC3, 0x17, 0x07, 0xB2, 0xA8, 0x02, 0xE4, 0xF1, 0x0C, 0x26, 0xCE, 0x37, 0xA3, 0x57, 0xC2, 0x67, 0xA4, 0xBF, 0xB8, 0xAC, 0xEE, 0x74, 0x8C, 0x61, 0xEC, 0xCA, 0x70, 0x52, 0x25, 0xF9, 0xDC, 0x80, 0x20, 0x0D, 0x17};

unsigned char p_t[ELEMENT_LEN] = {
0x8C, 0x3E, 0xAC, 0x37, 0x06, 0x8D, 0x74, 0x57, 0x1C, 0x3C, 0xB6, 0x18, 0x0B, 0xE5, 0xE8, 0xCD, 0xAC, 0x20, 0xCD, 0x2D, 0x87, 0x87, 0xCE, 0xCF, 0x99, 0x51, 0xE4, 0x44, 0x75, 0x88, 0x31, 0x4D, 0x43, 0xDE, 0xDC, 0xFA, 0x51, 0xC8, 0xD6, 0x64, 0x0F, 0x24, 0x4A, 0x04, 0x2E, 0x26, 0xCE, 0xC1, 0x98, 0xF6, 0x7E, 0x39, 0x91, 0xED, 0x88, 0xA6, 0x1B, 0xF4, 0x9E, 0x83, 0x80, 0xCB, 0x28, 0x57, 0x0E, 0x66, 0x4F, 0x5C, 0x82, 0x85, 0x8C, 0xBF, 0xC5, 0xCC, 0x32, 0x3F, 0x73, 0x55, 0x7F, 0x1A, 0x16, 0xD5, 0xF4, 0xA0, 0x94, 0x1C, 0x75, 0x29, 0x25, 0x46, 0x0E, 0x9E, 0x68, 0x72, 0xD5, 0x87, 0x62, 0x57, 0x3D, 0x8F, 0x19, 0x5D, 0xAB, 0xBB, 0x21, 0xA8, 0x4B, 0x3A, 0x84, 0xB4, 0x62, 0x80, 0xA3, 0x9B, 0xC1, 0x94, 0x40, 0xDC, 0x71, 0xA5, 0x77, 0xF3, 0xD4, 0x0F, 0x47, 0xB2, 0xA2, 0x78};
