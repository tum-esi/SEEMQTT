# The KeyNote Trust-Management System 
You can download the original KeyNote trust management library from here: https://www.cs.columbia.edu/~angelos/keynote.html. 

##New Features 

Few changes were performed on the original library. These changes include:

- Supporting "SHA256" while performing credential signature 
- Adding the "Delegate" section on the credential to determine whether the credential can be further delegated or not. 
- Making the library compatible with the latest version of the OpenSSL library. 

## How to use the library  

- The structure of the credential  

  ```
  authorizer: authorizer_public_key 
  licensees:  licensee_public_key  
  condtions: con1 -> trusted 
  signature:
  ```

- Sign the credential

  ```
  $ ./keynote  sign  sig-rsa-sha256-hex:  credential authorizer_private_key >> credential
  ```

- Verify the credential's signature

  ```
  ./keynote sigver credential  
  ```

- Testing the KeyNote policy

  in case the  licensee wants to send  a request  the authorizer,   the  authorizer uses that credential, its local policy, assertion file,  and the licensee's public key to evaluate that request.

  ```
  ./keynote verify -r untrusted ,trusted \
  -e assrt.txt \
  -l policy \
  -k licensee_public_key \
  credential
  ```

  
