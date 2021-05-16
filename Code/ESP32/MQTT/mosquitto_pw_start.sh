#!/bin/bash

# Stop current mosquitto service and clear configuration file
service mosquitto stop
killall mosquitto
rm -f /etc/mosquitto/conf.d/*.conf

# Start mosquitto broker with new configuration file
mosquitto -c mosquitto_pw.conf
