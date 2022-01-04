## Identity Based Encryption (IBE) for ESP32
IBE folder contains our BF-IBE implementation for the ESP32 platform.

- ibe_esp32: this folder contains
  - ibe_esp32.ino: an arduino program, which is used to test all the IBE operation locally. ESP32 will be used as PKG to perform setup and extract operation. Also, it will be used to perform Encryption and Decryption.
  - esp32_crosscompile: a folder, which contains precompiled GMP and PBC libraries. These libraries are required to run IBE test projects on ESP32.