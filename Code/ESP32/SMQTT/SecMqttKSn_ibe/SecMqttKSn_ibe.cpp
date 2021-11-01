/*
 * Copyright (c) 2021 Hangmao Liu <liuhangmao@hotmail.com>
 *                    Mohammad Hamad <mohammad.hamad@tum.de>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

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

    unsigned long ibe_enc[KSN_NUM]= {0};
    unsigned long ibe_pub[KSN_NUM]= {0};
    /* connect client to broker */
    time_info.t_s = micros();
    connect(client_id);
    time_info.t_connect = micros() - time_info.t_s;
    while(!connected()) {}

    /* start Phase I-1*/
    time_info.t_p1s = micros();
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
        Serial.println("*************************************************");
        Serial.println("*  Phase I: Symmetric Master Key Distribution   *");
        Serial.println("*************************************************");
        Serial.println("Phase I-1: Publishing Epk(K1) and nonce");
        Serial.println("*************************************************");
        #endif

        /* setup key store list */
        #ifdef DBG_MSG
        Serial.println("++++++ setting up key stores' list ++++++");
        #endif

        time_info.t_p11s = micros();
        for (int i = 0; i < KSN_NUM; i++) {



            /* generate the symertic master key*/
            sym_key_generator(ksn_list[i].masterkey);
            /*generate nonce */
            esp_fill_random(ksn_list[i].nonce, BLOCK_SIZE);

            /* keyen = symK || nonce */
            memcpy(keync[i], ksn_list[i].masterkey, BLOCK_SIZE);
            memcpy(keync[i] + BLOCK_SIZE, ksn_list[i].nonce, BLOCK_SIZE);

            #ifdef DDBG
            Serial.printf("KS[%d]. masterkey: ", i);
            PrintHEX (ksn_list[i].masterkey, BLOCK_SIZE);
            Serial.printf("KS[%d]. nonce: ", i);
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

        #ifdef DDBG
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
         * - publish Enc(symKPub-ksi||nci, pk_ksi)||iot_pk_key to key store value topic
         * - subscribe to key store acknowledge topic
         */


        for (int i = 0; i < KSN_NUM; i++) {
          /* set the starting time */
           time_info.t_p11_start[i]= micros();

            sprintf(ksn_list[i].value_topic, "%s/%d", this->_conn_topic, i + 1);
            sprintf(ksn_list[i].ack_topic, "%s/%d", this->_ack_topic, i + 1);

            subscribe(ksn_list[i].ack_topic);

            #ifdef DBG_MSG
            Serial.printf("subscribe to topic: %s\n",ksn_list[i].ack_topic);
            #endif

            if (this->_ibe_mode) {
                unsigned char Uc[ELEMENT_LEN];

                //#TODO check 2*BLOCK_SIZE
                unsigned long  starttime = micros();
                int Uc_len = ibe_encryption(i, keync[i], 2*BLOCK_SIZE, keync_enc[i], Uc);
                ibe_enc[i] = micros() - starttime ;

                #ifdef DDBG
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

                unsigned long ibe_pub_s = micros();
                beginPublish(ksn_list[i].value_topic, msg_len, false);
                write((byte *)msg, msg_len);
                endPublish();
                ibe_pub[i] = micros() - ibe_pub_s;
                #ifdef DBG_MSG
                Serial.printf("Key Published to  ks_%d\n",i);
                #endif
                time_info.t_recvs[i] = micros();
            }
        }

        time_info.t_p11 = micros() - time_info.t_p11s;
        time_info.t_p11_publish = Median(ibe_pub,KSN_NUM);
        time_info.t_ibe_enc =Median(ibe_enc,KSN_NUM);
          /* Phase I-1 was done */


        /* wait until received correct nonce from Key Store */
        unsigned long t_p12_s = micros();
        #ifdef DBG_MSG
        Serial.println("*************************************************");
        Serial.println("   Phase I-2: Reciving acknowledgements         ");
        Serial.println("*************************************************");
        Serial.println("   Waiting for Key Stores to response ...        ");
        Serial.println("*************************************************");
        #endif
        unsigned long stime = millis();
        unsigned long etime;
        while(this->_secmqtt_state != SECMQTT_CONNECT_GOOD_NONCE) {
            etime = millis();
            if ((etime - stime) > SECMQTT_TIMEOUT) {
                #ifdef DBG_MSG
                cout << "Phase 1 connection timeout!" << endl;
                #endif
                this->_secmqtt_state = SECMQTT_CONNECTION_TIMEOUT;
                return ;
            }

            if(this->_secmqtt_state == SECMQTT_CONNECT_BAD_NONCE) {
              #ifdef DBG_MSG
                Serial.println(this->_secmqtt_state);
              #endif
                return ;
            }

            loop();
        }

        #ifdef DBG_MSG
        Serial.println("Phase I was finished successfully!!");
        #endif
        this->_secmqtt_state = SECMQTT_KS_CONNECTED;
          //time_info.t_p2 = micros() - time_info.t_recvs[0];
        time_info.t_p12 = micros() - t_p12_s;
        time_info.t_p1= micros() -  time_info.t_p1s ;


    }

    #ifdef TIME_MSG
    Serial.printf("---------------------------------------------------------------------\n");
    Serial.printf("Time to Connect:                                          \t%lu (us)\n", time_info.t_connect);
    Serial.printf("Time to encrypt symmetric master key using IBE (Median):   \t%lu (us)\n", time_info.t_ibe_enc);
    Serial.printf("Time to publish the encrypted symmetric master key(Median):\t%lu (us)\n", time_info.t_p11_publish);
    Serial.printf("Time to finish Phase I-1:                                 \t%lu (us)\n", time_info.t_p11);
    Serial.printf("---------------------------------------------------------------------\n");
    Serial.printf("Time to decrypt the acknowledgement (Median):            \t%lu (us)\n", time_info.t_p12_dec);
    //Serial.printf("Max time to Recive acknowledgment form KS:                \t%lu (us)\n", Max(time_info.t_recv, KSN_NUM) );
    Serial.printf("Time to finish Phase I-2:                                  \t%lu (us)\n", time_info.t_p12);
    Serial.printf("---------------------------------------------------------------------\n");
    Serial.printf("Time to finish Phase I:                                    \t%lu (us)\n", time_info.t_p1);
    Serial.printf("---------------------------------------------------------------------\n");
    Serial.printf("ID\tphase1_s\tEncrypt\tPublish\tend_p11\t Rcv_ack\t endp12\n");
    for (int index = 0; index < KSN_NUM; index++)
      Serial.printf("%d\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n",index, time_info.t_p11_start[index],ibe_enc[index], ibe_pub[index],time_info.t_recvs[index], time_info.t_recv[index], time_info.t_p12_end[index]);
    #endif

    SecSessionKeyUpdate();

    return ;
}

// TODO
// aes gcm mode

void SecMqtt::SecPublish(const char* topic, const unsigned char* msg, size_t msg_len) {

    /**
     * Encrypte message using  the topic  key (E_EtM(msg)) and publish the encrypted message
     */
    #ifdef DBG_MSG
    Serial.println("**************************************************");
    Serial.println("*   Phase III: Encrypted Message Transmission      *");
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
    Serial.println("Phase III: Publishing encrypted message ...");
    #endif

    unsigned char Eskmsg [msg_len] ={0x0};
    unsigned char iv[BLOCK_SIZE], iv_tmp[BLOCK_SIZE];
    esp_fill_random(iv, BLOCK_SIZE);
    memcpy(iv_tmp, iv, BLOCK_SIZE);

    /* additional data to be used for the AES-GCM*/
    const char *auth_msg = "";
    unsigned char tag[BLOCK_SIZE];
    size_t tag_len = BLOCK_SIZE;

    unsigned long t_p3_enc =micros();
    int result = aes_gcm_encryption(msg, msg_len, this->_session_key, iv_tmp, Eskmsg, (const unsigned char*)auth_msg, (size_t)strlen(auth_msg), tag, tag_len);
    time_info.t_p3_enc = micros() - t_p3_enc;

    #ifdef DDBG
    Serial.print("plantext user message: ");
    PrintHEX((unsigned char*)msg, msg_len);
    Serial.print("Encrypted user message: ");
    PrintHEX(Eskmsg, msg_len);
    Serial.print("Tag: ");
    PrintHEX(tag, tag_len);
    Serial.print("IV: ");
    PrintHEX(iv, BLOCK_SIZE);
    #endif

    /* prepare the transmited messgae
       ******************************
       * tag  | iv |msg_len| enc_msg *
       ******************************
    */
    int mlen  = 2 * BLOCK_SIZE + sizeof(int)+ msg_len;
    int length = msg_len;
    unsigned char mbuffer[mlen] = {0x0};
    memcpy(mbuffer, tag, BLOCK_SIZE);
    memcpy(mbuffer+BLOCK_SIZE, iv, BLOCK_SIZE);
    memcpy(mbuffer+2*BLOCK_SIZE, &length, sizeof(int));
    memcpy(mbuffer+2*BLOCK_SIZE +sizeof(int), Eskmsg, msg_len);

    #ifdef DDBG
    Serial.print("plantext user message: ");
    PrintHEX(mbuffer, mlen);
    #endif
    unsigned long t_p3_enc_s =micros();
    beginPublish(topic,  mlen, false);
    write((byte*)mbuffer, mlen);
    endPublish();
   time_info.t_p3_pub = micros() - t_p3_enc_s;
   this->_sk_counter += 1;


    #ifdef DBG_MSG
    Serial.printf("End Phase 3! #%d \n", this->_sk_counter);
    #endif
    time_info.t_p3_all = micros()-t_b;

    #ifdef TIME_MSG
    Serial.printf("Time to Encrypte the message (EtM): %lu (us) \n", time_info.t_p3_enc);
    Serial.printf("Time to publish the encrypted message: %lu (us)\n",time_info.t_p3_pub );
    Serial.printf("Time to of Phase III: %lu (us)\n",time_info.t_p3_all );
    #endif
}

void SecMqtt::SecSessionKeyUpdate() {

    time_info.t_p2_s = micros();
    unsigned long t_b = micros();
    #ifdef DBG_MSG
    Serial.println("**************************************");
    Serial.println("*   Phase II: Topic Key Distribution   *");
    Serial.println("**************************************");
    #endif


    unsigned long t_shar_enc[KSN_NUM] = {0};
    unsigned long t_share_pub[KSN_NUM] = {0};
    int rc = 0;
    this->_sk_counter = 0;
    unsigned char iv[BLOCK_SIZE];
    unsigned char iv_tmp[BLOCK_SIZE];
    const char  * aad = "";
    unsigned char tag[BLOCK_SIZE];

    sym_key_generator(this->_session_key);

    #ifdef DDBG
    Serial.print("Session Key: ");
    PrintHEX(this->_session_key, BLOCK_SIZE);
    #endif

    /* secret splitting */
    if (this->_ksn_mode) {
        time_info.t_s = micros();
        secmqtt_key_split();
        time_info.t_keysplit = micros() - time_info.t_s;
    }
      /* Shamir Secret Sharing*/
    if (this->_sss_mode) {
        unsigned long sss_t  = micros();
        secmqtt_sss_split();
        time_info.t_keysplit = micros() - sss_t;
    }

    /* Hashing the topic name  H(t_i) */
    /*TODO Check DATA*/
    rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)DATA, strlen(DATA), this->_iot_data_hash);
    if (rc != 0) {
        #ifdef DBG_MSG
        Serial.println("failed to hash DATA!");
        #endif
    } else {
        #ifdef DBG_MSG
        Serial.println("successfully hash DATA!");
        #endif
    }

    for (int i = 0; i< KSN_NUM; i++) {
        if (this->_ksn_mode) {
            #ifdef DDBG
            Serial.printf("shares: %d\n", i);
            PrintHEX(this->_shares[i], BLOCK_SIZE);
            #endif

            unsigned char msg[BLOCK_SIZE + HASH_LEN];
            unsigned char msg_enc[BLOCK_SIZE + HASH_LEN];
            size_t m_len = 3 *BLOCK_SIZE + HASH_LEN  ; //IV + tag + mag_enc
            unsigned char buffer[m_len];

            esp_fill_random(iv, BLOCK_SIZE);
            memcpy(iv_tmp, iv, BLOCK_SIZE);

            memcpy(msg, this->_shares[i], BLOCK_SIZE);
            memcpy(msg + BLOCK_SIZE, this->_iot_data_hash, HASH_LEN);

            unsigned long t_enc_s = micros();
            rc = aes_gcm_encryption(msg,BLOCK_SIZE + HASH_LEN,ksn_list[i].masterkey,iv_tmp,msg_enc, (unsigned char *)aad, strlen(aad), tag, BLOCK_SIZE);
            t_shar_enc[i] = micros() - t_enc_s;

            if (rc != 0) {
              #ifdef DBG_MSG
                Serial.println("Failed to encrypt the session key!");
              #endif
                return ;
            } else {
                #ifdef DBG_MSG
                Serial.println("succeed to encrypt session key!");
                #endif
            }

            memcpy(buffer, msg_enc, BLOCK_SIZE + HASH_LEN);
            memcpy(buffer + BLOCK_SIZE + HASH_LEN, iv, BLOCK_SIZE);
            memcpy(buffer + 2*BLOCK_SIZE + HASH_LEN, tag, BLOCK_SIZE);

            sprintf(ksn_list[i].sk_topic, "%s/%d", this->_sk_topic, i + 1);

            #ifdef DDBG
            Serial.print("publish key shares to KeyStores under sk topic: ");
            Serial.println(ksn_list[i].sk_topic);
            #endif

            unsigned long  t_pub_s = micros();
            beginPublish(ksn_list[i].sk_topic, m_len, false);
            write((byte *)buffer, m_len);
            endPublish();
            t_share_pub[i] = micros() - t_pub_s;
        }

        if (this->_sss_mode) {

            #ifdef DDBG
            Serial.printf("sss shares: %d\n", i);
            Serial.printf("%.*s\n", SSS_SIZE, this->_shares_sss[i]);
            #endif

            /*
             * msg has to be multiple of 16 bytes aes cbc mode
             * we add 09 as padding
             */
            int padding = 16  - (SSS_SIZE + HASH_LEN)%16;
            int msglen = SSS_SIZE + HASH_LEN + padding;
            unsigned char msg[msglen];
            unsigned char msg_enc[msglen];


            esp_fill_random(iv, BLOCK_SIZE);
            memcpy(iv_tmp, iv, BLOCK_SIZE);

            memcpy(msg, this->_shares_sss[i], SSS_SIZE);
            memcpy(msg + SSS_SIZE, this->_iot_data_hash, HASH_LEN);
            memset(msg + SSS_SIZE + HASH_LEN, padding, padding);

            unsigned long t_enc_s = micros();
            rc = aes_gcm_encryption(msg, msglen,ksn_list[i].masterkey,iv_tmp,msg_enc, (unsigned char *)aad, strlen(aad), tag, BLOCK_SIZE);
            t_shar_enc[i] = micros() - t_enc_s;
            //time_info.t_p2_share_enc = micros() - time_info.t_s;
            if (rc != 0) {
              #ifdef DBG_MSG
                Serial.println("Failed to encrypt secret share key!");
              #endif
                return ;
            }
            else
            {
              #ifdef DDBG
              Serial.println("succeed to encrypt secret share key!");
              Serial.printf("Encrypted message:\n");
              PrintHEX(msg_enc, msglen);
              Serial.printf("Tag:\n");
              PrintHEX(tag, BLOCK_SIZE);
              Serial.printf("IV:\n");
              PrintHEX(iv, BLOCK_SIZE);
              #endif
            }
            /**
                   **********************************************
                   * E_EtM(alpha_i, H(t_i))|iv|tag|H(iot_pk_key)*
                   * ********************************************
            */
            //size_t m_len = 3*BLOCK_SIZE + HASH_LEN + BLOCK_SIZE;
            size_t m_len  = msglen  + BLOCK_SIZE + BLOCK_SIZE+ HASH_LEN ;
            unsigned char buffer[m_len] = {0x0};

            //memcpy(buffer, msg_enc, 3*BLOCK_SIZE + 2*HASH_LEN);
            memcpy(buffer, msg_enc, msglen);
            memcpy(buffer + msglen, iv, BLOCK_SIZE);
            memcpy(buffer + msglen +BLOCK_SIZE , tag, BLOCK_SIZE);
            memcpy(buffer + msglen +BLOCK_SIZE+ BLOCK_SIZE, this->_iot_pk_key_hash, HASH_LEN);

            sprintf(ksn_list[i].sk_topic, "%s/%d", this->_sk_topic, i + 1);
            #ifdef DBG_MSG
            Serial.print(" share  was publihed to KeyStores\n");
            #endif
            #ifdef DDBG
            Serial.println(ksn_list[i].sk_topic);
            Serial.printf("Share [%d]\n", i );
            PrintHEX(msg, (int)SSS_SIZE);
            Serial.printf(" the buffer:\n");
            PrintHEX(buffer, (int)m_len);
            #endif

            unsigned long  t_pub_s = micros();
            beginPublish(ksn_list[i].sk_topic, m_len, false);
            write((byte *)buffer, m_len);
            endPublish();
            t_share_pub[i] = micros() - t_pub_s;

        }
    }

    unsigned long t_e = micros();

    /* transmit the credential */
    int cr_len = this->_iot_credntial_size;
    unsigned char cr_buffer[cr_len];
    memcpy(cr_buffer, this->_iot_credntial, cr_len);

    unsigned long cre_s = micros();
    beginPublish(CR,  cr_len, false);
    write((byte*)cr_buffer, cr_len);
    endPublish();
    time_info.t_cred = micros() - cre_s;
    time_info.t_p2 =micros() - time_info.t_p2_s; // asdd the time to sign the credential


    #ifdef DDBG
    Serial.printf(" signed credential: %s\n",cr_buffer );
    #endif

    #ifdef DBG_MSG
    Serial.printf("Phase II was finished successfully!\n");
    #endif

    time_info.t_p2_share_publsih =Median(t_share_pub,KSN_NUM);
    time_info.t_p2_share_enc= Median(t_shar_enc, KSN_NUM);

    #ifdef TIME_MSG
    Serial.printf("time to split: \t%lu (us)\n", time_info.t_keysplit);
    Serial.printf("time key encrypt the share -AES-GCM: \t\t%lu (us)\n", time_info.t_p2_share_enc);
    Serial.printf("time key publish the encrypted share: \t\t%lu (us)\n", time_info.t_p2_share_publsih);
    Serial.printf("time sign the credential: \t%lu (us)\n", time_info.t_cred_sign);
    Serial.printf("time to publish the credential: \t\t%lu (us)\n", time_info.t_cred);
    Serial.printf("time Phase II: \t\t%lu (us)\n", time_info.t_p2);
    Serial.printf("time phase II + cred: \t%lu (us)\n", time_info.t_p2 + time_info.t_cred_sign);
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
        time_info.t_s = micros();
        int ks_id = atoi(token4) - 1;

        //time_info.t_recv[ks_id] = micros() - time_info.t_recvs[ks_id];
        time_info.t_recv[ks_id] =micros();

        unsigned char msg_enc[HASH_LEN];
        unsigned char msg_dec[HASH_LEN];
        unsigned char hpk[HASH_LEN];
        unsigned char iv[BLOCK_SIZE];

        memcpy(msg_enc, payload, HASH_LEN);
        memcpy(iv, payload + HASH_LEN, BLOCK_SIZE);

        #ifdef DDBG
        Serial.print("Received iv: ");
        PrintHEX(iv, BLOCK_SIZE);
        Serial.print("Master key used to decrypt: ");
        PrintHEX(ksn_list[ks_id].masterkey, BLOCK_SIZE);
        Serial.print("Nonce:");
        PrintHEX(ksn_list[ks_id].nonce, BLOCK_SIZE);
        #endif

        time_info.t_s = micros();
        rc = aes_decryption(msg_enc, HASH_LEN, ksn_list[ks_id].masterkey, iv, msg_dec);


        #ifdef DDBG
        Serial.print("decrypted message: ");
        PrintHEX(msg_dec, HASH_LEN);
        #endif

        if (rc != 0) {
            cout << "Failed to decrypt the nonce! \n";
        }
        else
         {
            for (int i = 0; i < HASH_LEN; i++) {
                hpk[i] = ksn_list[ks_id].nonce[i%16] ^ msg_dec[i];
            }

            #ifdef DDBG
            Serial.print("received   hpk: ");
            PrintHEX(hpk, HASH_LEN);
            Serial.print("calculated hpk: ");
            PrintHEX(this->_iot_pk_key_hash, HASH_LEN);
            #endif

            if(!memcmp(hpk, this->_iot_pk_key_hash, HASH_LEN)) {
                ksn_list[ks_id].verified = true;

                #ifdef DBG_MSG
                Serial.println("acknowledgement was received and successfully evaluated!\n");
                #endif

                if (secmqtt_check_all_ksn_nonce_stat()) {
                    this->_secmqtt_state = SECMQTT_CONNECT_GOOD_NONCE;
                }


            }
            else
             {
               #ifdef DBG_MSG
                Serial.println("acknowledgement was received. But it was not correct!\n");
                #endif
                ksn_list[ks_id].verified = false;
                this->_secmqtt_state = SECMQTT_CONNECT_BAD_NONCE;
            }

        }
        time_info.t_p12_dec = micros() - time_info.t_s;
        time_info.t_p12_end[ks_id] = micros();
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
    free (share);
}

char *  SecMqtt::secmqtt_sss_split(unsigned char * key, int n , int t)
{
  /* using shamir secret sharing */
  unsigned char str_tmp[BLOCK_SIZE + 1];
  memcpy(str_tmp, key, BLOCK_SIZE);
  str_tmp[BLOCK_SIZE] = '\0';
  unsigned long s_t = micros();
  char *share = generate_share_strings((char *)str_tmp, n, t);
  //Serial.printf("XXXXXXXXXXX Split time is %lu XXXXXXXXXXXX\n", micros()-s_t);

  return share;
}

char * SecMqtt::secmqtt_sss_combine(const char * shares, int t)
{
   return extract_secret_from_share_strings(shares, t);
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
       #ifdef DBG_MSG
        cout << "READING PRIVATE KEY NOT SUCCESSFULL, error code: " << error_code_pr << endl;
      #endif
        mbedtls_pk_free(&pr);
        return ;
    } else {
        /* compute the SHA-256 hash for the input*/
        unsigned char hash[32];
        int hash_ret;

        hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), input, P_SIZE, hash);
        if( hash_ret != 0 ) {
          #ifdef DBG_MSG
            cout << "Calculating HASH not SUCCESSFULL, error code: " << hash_ret << endl;
          #endif
            mbedtls_pk_free(&pr);
            return ;
        } else {
            int sign_ret;
            size_t olen = 0;

            sign_ret = mbedtls_pk_sign(&pr, MBEDTLS_MD_SHA256, hash, 0, sign, &olen, myrand, NULL);
            if (sign_ret != 0) {
                #ifdef DBG_MSG
                cout << "Sign not SUCCESSFULL" << endl;
                #endif
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
        #ifdef DBG_MSG
        printf( " failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
        #endif
        return;
    }

    if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, symkey, BLOCK_SIZE ) ) != 0 ) {
        #ifdef DBG_MSG
          printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
        #endif
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


/*
 * Function: aes_gcm_encryption
 * ----------------------------
 *   Perform authenticated encryption using AES-GEM mode
 *
 *   input:  plaintext
 *   input_len: plaintext lenght
 *   key: Secret key
 *   iv: initializtion vector
 *   output: ciphertext
 *   tag: message authentication code
 *   tag_len: tag lenght
 *
 *   returns: 0 (succeed), -1 (Failed)
 */
int SecMqtt::aes_gcm_encryption(const unsigned char* input, size_t input_len, const unsigned char* key, unsigned char* iv, unsigned char* output, const unsigned char* add, size_t add_len, unsigned char* tag, size_t tag_len) {

    int rc = 0;
    size_t iv_len = BLOCK_SIZE;
    unsigned int keybits = BLOCK_SIZE * 8;

    mbedtls_gcm_context aes;
    mbedtls_gcm_init(&aes);
    mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, key, keybits);

    rc = mbedtls_gcm_crypt_and_tag(&aes, MBEDTLS_GCM_ENCRYPT, input_len, iv, iv_len, add, add_len, input, output, tag_len, tag);
    if (rc != 0) {
        #ifdef DBG_MSG
        Serial.printf("failed \n ! mbedtls_gcm_crypt_and_tag returned %d\n", rc);
        #endif
        return -1;
    }

    mbedtls_gcm_free(&aes);

    #ifdef DDB
    Serial.println(" AES-GCM encryption was  perfromed successfulyl!  ");
    #endif
    return 0;
}
/*
 * Function: aes_gcm_decryption
 * ----------------------------
 *   Perform authenticated encryption using AES-GEM mode
 *
 *   input:  ciphertext
 *   input_len: ciphertext lenght
 *   key: Secret key
 *   iv: initializtion vector
 *   output: plaintext
 *   tag: message authentication code
 *   tag_len: tag lenght
 *
 *   returns: 0 (succeed), -1 (Failed)
 */
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
          #ifdef DBG_MSG
            Serial.printf("failed \n ! mbedtls_gcm_auth_decrypt authentication failed!\n", rc);
          #endif
            return -1;
        }
        #ifdef DBG_MSG
        Serial.printf("failed \n ! mbedtls_gcm_auth_decrypt returned %d\n", rc);
        #endif
        return -1;
    }

    mbedtls_gcm_free(&aes);
    #ifdef DDBF
    Serial.println(" AES-GCM decryption  was  perfromed successfulyl!  ");
    #endif
    return 0;
}
int SecMqtt::get_state() {
    return this->_secmqtt_state;
}
// this is used for test purpose only
int SecMqtt::set_state(int state) {
    this->_secmqtt_state = state;
}
void SecMqtt::secmqtt_set_cred(const unsigned char* cred) {
    this->_cred = cred;
}

void SecMqtt::secmqtt_set_iot_pk_key(const unsigned char* key, int key_size) {

    this->_iot_pk_key = key;
    this->_iot_pk_key_size = key_size;
    int hash_ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key, key_size, this->_iot_pk_key_hash);
    if (hash_ret != 0) {
      #ifdef DBG_MSG
        Serial.println("failed to hash iot node public key!");
        #endif
    } else {
        #ifdef DBG_MSG
        Serial.println("successfully hash iot node public key!");
        #endif
        #ifdef DDBG
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
  #ifdef DDBG
  Serial.println("*  Setting up the credentila  *");
  #endif
  unsigned char * resu_cre ;
  int r_len = 0;
  unsigned long st= micros();
  kn_GenCredential(cre,cre_size, this->_iot_pr_key, this->_iot_pr_key_size, &resu_cre, &r_len);
  time_info.t_cred_sign = micros()-st;
  this->_iot_credntial = resu_cre;
  this->_iot_credntial_size = r_len;
  #ifdef DDBG
  Serial.println(" The credential was generated.*");
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

        /* calculate one-time overhead Qid */
        int rc = 0;
        unsigned char hash[HASH_LEN] = {0};

        for (int i = 0; i < KSN_NUM; i++) {
            unsigned long qid_s = micros();
            element_init_G1(ksn_list[i].Qid, ibe_param_pub.pairing);

            rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (unsigned char *)ksn_list[i].ibe_id, ksn_list[i].ibe_id_len, hash);

            if (rc != 0) {
              #ifdef DBG_MSG
                Serial.println("failed to hash key store ibe ID!");
              #endif

            } else {
                #ifdef DBG_MSG
                Serial.println("successfully hash key store ibe ID!");
                #endif

            }

            element_from_hash(ksn_list[i].Qid, hash, HASH_LEN);
            element_init_GT(ksn_list[i].Gid, ibe_param_pub.pairing);
            element_pairing(ksn_list[i].Gid, ksn_list[i].Qid, ibe_param_pub.Kpub);
            time_info.t_ibe_oto = micros() -qid_s;
            #ifdef TIME_MSG
              Serial.printf("Time to calculat Qid of ks[%d]:     \t%lu (us)\n", i+1,time_info.t_ibe_oto );
            #endif
        }
    }
}

void SecMqtt::secmqtt_set_ibe_id(const char*id, int idlen, int ksid) {
    ksn_list[ksid-1].ibe_id_len=idlen;
    memcpy(ksn_list[ksid-1].ibe_id, id, idlen);
    #ifdef DDBG
    Serial.printf("ibe key size:%d\n", idlen);
    Serial.println(ksn_list[ksid-1].ibe_id);
    #endif
}

int SecMqtt::ibe_encryption(int ks_id, unsigned char *plaintext, \
                    size_t plaintext_len , unsigned char *cipher, unsigned char *Uc) {

    int rc = 0;
    unsigned char *gs = NULL, hash[HASH_LEN] = {0};
    element_t r, U;

    unsigned long t_ibe_s = micros();
    element_init_G1(U, ibe_param_pub.pairing);
    element_init_Zr(r, ibe_param_pub.pairing);
    element_random(r);
    element_mul_zn(U, ibe_param_pub.P, r);
    element_pow_zn(ksn_list[ks_id].Gid, ksn_list[ks_id].Gid, r);

    gs = (unsigned char *)malloc(element_length_in_bytes(ksn_list[ks_id].Gid));
    element_to_bytes(gs, ksn_list[ks_id].Gid);

    rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)gs, element_length_in_bytes(ksn_list[ks_id].Gid), hash);
    if (rc != 0) {
      #ifdef DBG_MSG
        Serial.println("failed to hash key store gs!");
      #endif
    } else {
        #ifdef DDBG
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
    //element_clear(gid);
    return Uc_len;
}

void SecMqtt::secmqtt_set_secret_share_mode(char *mode) {

    #ifdef DDBG
    Serial.printf("set key sharing mode to %s\n", mode);
    #endif

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

unsigned long  SecMqtt:: Median(unsigned long arr[], int n)
{
  int i, j ;
  unsigned long temp;
  for (i= 0 ; i<n ; i++)
  {
    for (j= 0 ; j<n-i-1; j++)
    {
      if (arr[j]> arr[j+1])
      {
        temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
  int index = (int)((n+1)/2) -1 ;
  return arr[index];
}

unsigned long  SecMqtt:: Max(unsigned long arr[], int n)
{
  int i, j ;
  unsigned long temp;
  for (i= 0 ; i<n ; i++)
  {
    for (j= 0 ; j<n-i-1; j++)
    {
      if (arr[j]> arr[j+1])
      {
        temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
  return arr[n-1];
}

unsigned char kpub_t[ELEMENT_LEN] = {
0x33, 0x14, 0x25, 0x4B, 0x28, 0x8C, 0xEB, 0xDD, 0x0B, 0x3F, 0xDE, 0xCB, 0xF3, 0x61, 0x97, 0x77, 0x57, 0x42, 0xEE, 0x44, 0x61, 0xB4, 0x16, 0xDB, 0x4F, 0xA9, 0x7F, 0xCE, 0x80, 0x36, 0xD3, 0xCF, 0xB5, 0x1D, 0x3D, 0xD7, 0xC7, 0x78, 0x54, 0x1C, 0x1C, 0xF5, 0x10, 0x98, 0x08, 0x37, 0x78, 0x01, 0xC6, 0xB0, 0x87, 0x27, 0xC0, 0xC8, 0xA2, 0xE9, 0x7B, 0x2A, 0x67, 0x34, 0x2E, 0x90, 0x69, 0x04, 0x2A, 0xB8, 0x61, 0x04, 0xEE, 0x91, 0x9A, 0x3D, 0x26, 0xDA, 0x04, 0x20, 0x08, 0xD6, 0xF7, 0x19, 0x00, 0x15, 0x63, 0xF4, 0x65, 0x2F, 0x28, 0xE3, 0x38, 0xAB, 0xD9, 0x79, 0x60, 0xC3, 0x17, 0x07, 0xB2, 0xA8, 0x02, 0xE4, 0xF1, 0x0C, 0x26, 0xCE, 0x37, 0xA3, 0x57, 0xC2, 0x67, 0xA4, 0xBF, 0xB8, 0xAC, 0xEE, 0x74, 0x8C, 0x61, 0xEC, 0xCA, 0x70, 0x52, 0x25, 0xF9, 0xDC, 0x80, 0x20, 0x0D, 0x17};

unsigned char p_t[ELEMENT_LEN] = {
0x8C, 0x3E, 0xAC, 0x37, 0x06, 0x8D, 0x74, 0x57, 0x1C, 0x3C, 0xB6, 0x18, 0x0B, 0xE5, 0xE8, 0xCD, 0xAC, 0x20, 0xCD, 0x2D, 0x87, 0x87, 0xCE, 0xCF, 0x99, 0x51, 0xE4, 0x44, 0x75, 0x88, 0x31, 0x4D, 0x43, 0xDE, 0xDC, 0xFA, 0x51, 0xC8, 0xD6, 0x64, 0x0F, 0x24, 0x4A, 0x04, 0x2E, 0x26, 0xCE, 0xC1, 0x98, 0xF6, 0x7E, 0x39, 0x91, 0xED, 0x88, 0xA6, 0x1B, 0xF4, 0x9E, 0x83, 0x80, 0xCB, 0x28, 0x57, 0x0E, 0x66, 0x4F, 0x5C, 0x82, 0x85, 0x8C, 0xBF, 0xC5, 0xCC, 0x32, 0x3F, 0x73, 0x55, 0x7F, 0x1A, 0x16, 0xD5, 0xF4, 0xA0, 0x94, 0x1C, 0x75, 0x29, 0x25, 0x46, 0x0E, 0x9E, 0x68, 0x72, 0xD5, 0x87, 0x62, 0x57, 0x3D, 0x8F, 0x19, 0x5D, 0xAB, 0xBB, 0x21, 0xA8, 0x4B, 0x3A, 0x84, 0xB4, 0x62, 0x80, 0xA3, 0x9B, 0xC1, 0x94, 0x40, 0xDC, 0x71, 0xA5, 0x77, 0xF3, 0xD4, 0x0F, 0x47, 0xB2, 0xA2, 0x78};
