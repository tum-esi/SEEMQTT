## Performance test for SEEMQTT
test_seemqtt.ino program is used to evaluate the performance of SEEMQTT library. This program starts an MQTT publish client on our IoT device (ESP32), which communicates with other participants according to SEEMQTT protocol.
### Features
- SecConnect: connects the publisher to the broker and set up the master symmetric key with every KeyStore.
    A successful negotiation with all KeyStores will change the internal status to SECMQTT_KS_CONNECTED.
- SecPublish: publishes authenticated encrypted message.