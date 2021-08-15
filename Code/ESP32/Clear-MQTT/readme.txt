Install mosquitto (MQTT broker)
We use the Mosquitto as our MQTT broker. Run command to install the Mosquitto on your device.
sudo apt-get update
sudo apt-get install mosquitto
The mosquitto broker should be successfully installed and started now. Run command to check mosquitto broker status.
service mosquitto status 

Install mosquitto client
In order to do Publish/Subscribe, we need MQTT client on our device. Run command to install mosquitto client on your device.
sudo apt-get install mosquitto-clients
Non we are able to Publish message to a topic or receive a message by Subscribe to a topic. Run command in seperate terminal window to test MQTT client.
mosquitto_sub -h localhost -t "test"
mosquitto_pub -h localhost -t "test" -m "hello world!"

Mosquitto Username and Password Authentication
In user_password.txt file is an example password file. The file has two users:
esp32:topsecret
pc:topsecret
Use "sudo ./mosquitto_pw_start.sh" to start mosquitto broker with Username and Password Authentication.
You should not be able to publish message now using:
mosquitto_pub -h localhost -t "test" -m "hello world!"
Try:
mosquitto_sub -h localhost -p 1883 -t test -u esp32 -P topsecret
mosquitto_pub -h localhost -p 1883 -t test -m hello -u pc -P topsecret
You can also use "mosquitto_passwd -b passwordfile username password" to add new user to the broker.
