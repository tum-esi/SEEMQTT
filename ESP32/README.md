# The Library and Projects for the ESP32 Platform
This folder contians: 
- Arduino project to implement and evalute publishing messages using defualt MQTT without any security protection (see [Clear-MQTT.ino](Clear-MQTT/Clear-MQTT.ino))
- Arduino project to implement a publisher  that uses MQTT over TLS (see [Clear-MQTT](Clear-MQTT)). 
- Arduino project to test Shamire's secret sharing schema (see [ShamirSS](ShamirSS))
- **SEEMQTT Arduino Client Library**  (see [SEEMQTT](SEEMQTT))
- The cross-compiled GMP and PBC libaries (see [esp32_crosscompile](esp32_crosscompile))
- Ardunio project to test and evaluate the performance evaluation of the four primary algorithms of IBE: setup, extract, encryption and decryption. We use multiple message sizes for evaluating the encryption and decryption (Setup and extract are independent of the message size). Also, the program measures the one-time overhead (OTO) during the encryption. (see [ibe_esp32.ino](esp32/ibe_esp32.ino)
