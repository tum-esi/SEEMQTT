#!/bin/bash

# Stop current mosquitto service and clear configuration file
service mosquitto stop
killall mosquitto
rm -f /etc/mosquitto/conf.d/*.conf

# Prepare ca.crt, ca.key and server.crt for starting mosquitto broker with SSL/TLS
cp CA/ca.crt /etc/mosquitto/ca_certificates/
cp server_cert/server.crt server_cert/server.key /etc/mosquitto/certs/

# Prepare configuration file for mosquitto broker
cp mosquitto_ssl.conf /etc/mosquitto/conf.d/

# Start mosquitto broker with new configuration file
service mosquitto start
