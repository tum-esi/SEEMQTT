# Identity Based Encryption (IBE) for ESP32
IBE folder 

- ibe_esp32: This folder contains two sub project 
  - ibe_ESP32: to test all the IBE process locally 
  - ibe_mqtt: to connect with mqtt broker and receive the IBE public parameters. 
- ibe_linux: this folder contains:
  - **bonehfranklin.c**:  the IBE implementation. (where I can find the input parameters?)
  - **ibemqtt**: 
  - **ibemqttsub**: 

​	

## Setup IBE on Linux 

- #### Install libgmp-dev  and libssl-dev

  ```
  sudo apt install libgmp3-dev
  sudo apt install libssl-dev
  
  ```

- #### install pbc 

- `$ wget https://crypto.stanford.edu/pbc/files/pbc-0.5.1 
  `

- install flex and bison if you do not have them installed 

- `$ sudo apt-get install flex`

-  `$ sudo apt-get install bison`

- Unzip the pbcXXXXX.gz file and then cd inside the unzipped folder 

- ```
  $ ./configure
  $ make
  $ sudo make install
  ```

