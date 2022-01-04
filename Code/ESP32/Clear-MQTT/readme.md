## Basic MQTT
Clear-MQTT is a simple MQTT example application, which uses the default port 1883 for non-encrypted communication.
### Install mosquitto (MQTT broker)
We use the Mosquitto as our MQTT broker. Run command to install the Mosquitto on your device.
```bash
sudo apt-get update
sudo apt-get install mosquitto
```
The mosquitto broker should be successfully installed and started now. Run command to check mosquitto broker status.
```bash
service mosquitto status
```

### Install mosquitto client
In order to do Publish/Subscribe, we need MQTT client on our device. Run command to install mosquitto client on your device.
```bash
sudo apt-get install mosquitto-clients
```
Non we are able to Publish message to a topic or receive a message by Subscribe to a topic. Run command in seperate terminal window to test MQTT client.
```bash
mosquitto_sub -h localhost -t "test"
mosquitto_pub -h localhost -t "test" -m "hello world!"
```

### Mosquitto Username and Password Authentication
In user_password.txt file is an example password file. The file has two users:
```bash
esp32:topsecret
pc:topsecret
```
Use:
```bash
sudo ./mosquitto_pw_start.sh
```
to start mosquitto broker with username and password authentication.\
You should not be able to publish message now using:
```bash
mosquitto_pub -h localhost -t "test" -m "hello world!"
```
Try:
```bash
mosquitto_sub -h localhost -p 1883 -t test -u esp32 -P topsecret
mosquitto_pub -h localhost -p 1883 -t test -m hello -u pc -P topsecret
```
You can also use:
```bash
mosquitto_passwd -b passwordfile username password
```
to add new user to the broker.

