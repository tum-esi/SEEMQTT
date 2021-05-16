# Secure MQTT 

## Notes on build process
### Install Mosquitto server and client on Linux
#### server
    sudo apt-get update
    sudo apt-get install mosquitto
#### client
The client will be used for test.

    sudo apt-get install mosquitto-clients

### Install Arduino IDE
We install Arduino IDE (version 1.8.13) on Linux machine. Follow the instructions to install Arduino IDE on your PC (Windows/Mac OS X/Linux/Portable IDE/ChromeOS):  
https://www.arduino.cc/en/Guide/HomePage

#### Install ESP8266 and ESP32 Board Add-Ons in Arduino IDE
We use ESP8266 and ESP32 Boards in our project. Follow the instructions in this tutorial to add add-ons for ESP8266 and ESP32 Boards in Arduino IDE:  
https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

#### Required libraries (using Arduino IDE)
We need to install **PubSubClient** library. 
Here is  a description about how to install a library into your Arduino IDE: https://www.arduino.cc/en/guide/libraries
