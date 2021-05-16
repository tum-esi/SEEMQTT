#!/bin/bash

# Port 1883 without any authentification
# Port 8883 authentification with SSL/TLS, only validate server certificate
# Port 8884 authentification with SSL/TLS, validate both server and client certificates

# Stop current mosquitto broker and clear configuration file
service mosquitto stop
killall mosquitto
sleep 3
 
rm -f /etc/mosquitto/conf.d/*.conf

# Start mosquitto broker with new configuration file
mosquitto -c mosquitto_all.conf
