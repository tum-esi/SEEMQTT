# Identity Based Encryption (IBE) for ESP32
We present here a practical IBE implementation, which requires GMP and PBC libraries, for ESP32 board. Cross-Compilation of GMP and PBC libraries is required to first be done.
User can cross compile this libraries or can simply used precompiled libraries.
##  Performence TEST 
The ibe_esp32.ino is a project to test and evaluate the performance evaluation of the four primary operations of IBE:  setup, extract, encryption and decryption.
We use multiple message sizes for evaluating the encryption and decryption (Setup and extract are independent of the message size). Also, the program measures the one-time overhead (OTO) during the encryption.  
