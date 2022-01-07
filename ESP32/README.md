# The Library and Projects for ESP32 Platform
This folder contains: 
- Arduino project to implement and evaluate a publisher that uses MQTT without any security protection (see [Clear-MQTT.ino](Clear-MQTT/Clear-MQTT.ino)).
- Arduino project to implement a publisher that uses MQTT over TLS (see [TLS-MQTT](TLS-MQTT/TLS-MQTT.ino)). More information about setting up the TLS connection between the publisher andd broker is explained [here](TLS-MQTT/README.md).
- Arduino project to test Shamir's secret sharing schema (see [ShamirSS](ShamirSS)).
- **SEEMQTT Arduino Client Library**  (see [SEEMQTT](SEEMQTT)).
- Arduino project to evaluate the **SEEMQTT** library. This program implements an MQTT publisher that communicates with other participants (i.e., KeyStores and Subscribers) according to SEEMQTT protocol. (see [SEEMQTT_TEST.ino](SEEMQTT_TEST/SEEMQTT_TEST.ino)).
- The cross-compiled GMP and PBC libaries (see [esp32_crosscompile](esp32_crosscompile)).
- Arduino project to test and evaluate the performance evaluation of the four primary algorithms of IBE: setup, extract, encryption, and decryption. We use multiple message sizes for evaluating the encryption and decryption (Setup and extract algorithms are independent of the message size). Also, the program measures the one-time overhead (OTO) during the encryption. (see [ibe_esp32.ino](esp32_ibe/ibe_esp32.ino)).

