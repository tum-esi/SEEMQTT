# MQTT over SSL/TLS
We provide here an example of MQTT communication over SSL. Both **one-way** and **two-way** SSL are supported. To enable SSL, you will need to obtain a **valid** or generate a **self-signed** SSL certificate. Here we present a way to quick generate a **self-signed** SSL certificate.
## Self-signed certificate generation
In **cert_generator** folder we have three configuration file templates with suffix **.conf** for generating **self-signed** certificate. A **CA** (Certificate Authority) will be first generated using the configuration in **ca_cert.conf** file. Then generated **CA** will be used to sign certificates for server and client. Generated **.key**, **.csr** and **.crt** files will be placed in **CA**, **server_cert** and **client_cert** folder. **ca_cert.conf** is shown below:
```
[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
prompt = no

[req_distinguished_name]
countryName = DE
stateOrProvinceName = Bavaria
localityName = Munich
organizationalUnitName  = IoT TU Munich
commonName = @server_ip@

[ v3_req ]
# Extensions to add to a certificate request
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment
subjectAltName = @alt_names

[alt_names]
DNS.1 = localhost
IP.1 = 127.0.0.1
IP.2 = 192.168.1.101
```
An automatic generation of all certificates for **CA**, **server** and **client** can be done by:
```
sudo ./cert_generator.sh 
```
**Note**: you need to change the **server_ip** and  **client_ip** in cert_generator.sh file according to your IP setting before you run it.
## Run Mosquitto server and client over SSL
After successfully generating certificates, you can test MQTT communication over SSL/TLS by restarting Mosquitto **server**.
```
sudo ./mosquitto_ssl_start.sh
```
This will restart the Mosquitto broker with configuration file **mosquitto_ssl.conf**, which enables **one-way** SSL on port **8883** and **two-way** SSL on port **8884**.
For **one-way** SSL only the **server** certificate needs to be validated by **client**:
```
mosquitto_sub -h server_ip -p 8883 -t "test" --cafile CA/ca.crt
mosquitto_pub -h server_ip -p 8883 -t "test" -m "hello" --cafile CA/ca.crt
```
You should be able to **Publish/Subscribe** to the topic "test" with configuring a **CA** certificate! Congratulations!

For **two-way** SSL not only the **server** certificate but also the **client** certificate need to be validated. You should not be able to **Publish/Subscribe** to topic "test" now without providing a client certificate.
```
mosquitto_sub -h server_ip -p 8884 -t "test" --cafile CA/ca.crt
mosquitto_pub -h server_ip -p 8884 -t "test" -m "hello" --cafile CA/ca.crt
```
Both failed on port **8884**, which is now performing a **two-way** SSL.
Next you can try again with providing a client certificate.
```
mosquitto_sub -h server_ip -p 8883 -t "test" --cafile CA/ca.crt --key client_cert/client.key --cert client_cert/client.crt
mosquitto_pub -h server_ip -p 8883 -t "test" -m "hello" --cafile CA/ca.crt --cert server_cert/server.crt --key server_cert/server.key
```
It works. Congratulations!

## Reform Certificate and Key files for Arduino Project
After generating certificates and keys it might be useful to reform the layout of them for a quick import to your Arduino project.
```
sudo ./cert_reform.sh CA/ca.crt ca_cert tls_time_measurement/tls_time_measurement.ino
```
**cert_reform.sh** script takes 3 input parameters.
```
    Parameter 1: file that needs to be reformed (e.g. CA/ca.crt)
    Parameter 2: variable that needs to be updated in arduinio project (ca_cert, client_cert or client_private_key)
    Parameter 3: arduino project that needs to be updated
```