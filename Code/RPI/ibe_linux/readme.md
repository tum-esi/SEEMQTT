
## Testing IBE on Linux

To use IBE on Linux machine, user needs to install gmp and pbc libraries.

- #### Install libgmp-dev  and libssl-dev

  ```
  sudo apt install libgmp3-dev
  sudo apt install libssl-dev
  ```

- #### Install pbc

  - `$ wget https://crypto.stanford.edu/pbc/files/pbc-0.5.14.tar.gz
    `

  - install flex and bison if you do not have them installed

	  ```
	  sudo apt-get install flex
	  sudo apt-get install bison
	  ```

  - Unzip the pbcXXXXX.gz file and then cd inside the unzipped folder

  - ```
    $ ./configure
    $ make
    $ sudo make install
    ```
### Run Test file

- Compile the test file
  `$ make`

- Run the test

  `$ ./bfibe`
