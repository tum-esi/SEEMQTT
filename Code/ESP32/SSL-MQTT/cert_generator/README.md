# Certificate Generator

## Create keys + certificates for CA, Server and Client
Two variables **server_ip** and **client_ip** in **cert_generator.sh** file need to be changed according to your own setup before running it.
```
  sudo ./cert_generator.sh
```
Three folders (**CA**, **server_cert** and **client_cert**) will be created , which include generated **.key**, **.csr** and **.crt** files. **ca_cert.conf**, **server_cert.conf** and **client_cert.conf** are template for generating **.csr** (Certificate Signing Request) file. You can change them according to your own needs. Only these two variables **server_ip** and **client_ip** should only be changed in cert_generator.sh script.