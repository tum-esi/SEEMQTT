#Key Generator

This tool is used to generate a public and private key compatible to the KeyNote credentials. 
The tool will generate public and  private keys in both  .PEM  format and KeyNote format (i.e., Hex or Base64). 
You need to use this tool to generate public and private keys for the Publisher, CAs, and Subscribers. 

You must copy the keys of the Publisher to the  Arduino project and form the credential (without signature).   You must create the other credentials (e.g., CR^{CA}_{Sub}) and sign them using KeyNote. 

## Compile and run 

- Compile the test file

  `$ make`

- generate the keys  

  `$ ./KeyGen pkKname prKname keylenght `

  keylenght can be either 1024, 2048 or 3072

