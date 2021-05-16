# SMQTT
SMQTT folder includes source code for our MQTT KeyStore implementation.

# SSL-MQTT
SSL-MQTT folder includes source code for testing MQTT communication over SSL/TLS. All required certificates are provided in this folder.

# Start Mosquitto server
Using mosquitto_all_start.sh script will make Mosquitto server listen to port **1883**, port **8883** and port **8884**.
```
sudo ./mosquitto_all_start.sh
```
Port **1883**: traffic through this port is **unencrypted** and it performs no authentification on this port. This port is used to test our MQTT KeyStore implementation.  
Port **8883**: traffic through this port is **encrypted** and it performs a **one-way** SSL/TLS authentification, which requires server to provide its own certificate and clients will validate server certificate to make sure that it connects to a trusted server.  
Port **8884**: traffic through this port is **encrypted** and it requires not only the server certificate but also the client certificate. A **two-way** SSL/TLS is performed.
