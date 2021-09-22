# Identity Based Encryption (IBE) for ESP32
We present here a practical IBE implementation, which requires GMP and PBC libraries, for ESP32 board. Cross-Compilation of GMP and PBC libraries is required to first be done. 
User can cross compile this libraries or can simply used precompiled libraries. 
## Cross-Compilation 
### 1. Cross-Compilation GMP library (version 6.2.0) for ESP32
GMP is a free library for arbitrary precision arithmetic, operating on signed integers, rational numbers, and floating-point numbers.
1. you need to download the GMP library source code from here: https://gmplib.org
2. after unzip the library, create a text file  and save it as e.g. cross-compile-esp32.txt.
3. copy the next code ( A wrapper around ./configure (https://github.com/ikalchev/kpabe-yct14-cpp/issues/2).) into the file and change the paths according to your own settings. 
4.  After running the text file we just generated, a **config.h** file will be generated.

```
#!/bin/bash
# Don't forget:
# - Change HAS_OBSTACK_VNPRITNF and HAVE_NL_LANGINFO to 0 from config.h
# - (optional) Change HAS_ALLOCA and HAS_ALLOCA_H to 0 (we want to avoid stack allocation on the esp)
# - Change HAVE_RAISE to 0 (see invalid.c)
./configure \
  --enable-static \
  --disable-shared \
  --host=xtensa \
  --prefix=/usr/local/lib/xtensa-esp32-elf \
  AR=/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/bin/xtensa-esp32-elf-ar \
  ARFLAGS=rcs \
  RANLIB=/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/bin/xtensa-esp32-elf-ranlib \
  CC=/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc \
  CFLAGS="-Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-sign-compare -Os -g3 -nostdlib -Wpointer-arith -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -mlongcalls -ffunction-sections -fdata-sections -fstrict-volatile-bitfields" \
  CXX=/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++ \
  CXXFLAGS="-fno-rtti -fno-exceptions -std=gnu++11" \
  LIBS="" \
  LDFLAGS="-L/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/lib -L/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/ld -nostdlib -Wl,-static -Wl,-gc-sections -Wl,-EL -Wl,--undefined=uxTopUsedPriority"
```
```
chmod +x cross-compile-esp32.txt && ./cross-compile-esp32.txt
```
3. Change HAVE_OBSTACK_VPRINTF, HAVE_NL_LANGINFO and HAVE_RAISE to 0 from **config.h**. **Note**: in our case, a error occurs during linking stage when we use pre-compiled libgmp.a library in Arduino (1.8.13). As a workaround we copy the lines below from invalid.c to errno.c.
```
#if ! HAVE_RAISE
#define raise(sig)   kill (getpid(), sig)
#endif
```
4. Run ```make && make install``` to generate static library **libgmp.a**, which will be located in our case under folder **/usr/local/lib/xtensa-esp32-elf/lib**.
5. Done

### 2. Cross-Compilation PBC library (version 0.5.14) for ESP32
The PBC (Pairing-Based Cryptography) library is a free C library (released under the GNU Lesser General Public License) built on the GMP library that performs the mathematical operations underlying pairing-based cryptosystems.
1. PBC library source code download: https://crypto.stanford.edu/pbc/download.html
2. A wrapper around ./configure (https://github.com/ikalchev/kpabe-yct14-cpp/issues/2). Change the paths according to your own settings and save it as e.g. cross-compile-esp32.txt. After running the script, a **config.h** file will be generated.
```
#!/bin/bash
# The many includes in CPPFLAGS are all because of arith/init_random.c, as it is modified to use the random number generator from the esp platform.
# Don't forget:
# - Change HAVE_ALLOCA and HAVE_ALLOCA_H to 0 (we don't want to use stack allocation on the esp)

TOOLCHAIN="/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/bin/xtensa-esp32-elf-"

./configure \
  --enable-static \
  --disable-shared \
  --host=xtensa \
  --prefix=/usr/local/lib/xtensa-esp32-elf \
  AR="$TOOLCHAIN"ar \
  ARFLAGS=rcs \
  AS="$TOOLCHAIN"as \
  RANLIB="$TOOLCHAIN"ranlib \
  NM="$TOOLCHAIN"nm \
  CC="$TOOLCHAIN"gcc \
  CFLAGS="-Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-sign-compare -Os -g3 -nostdlib -Wpointer-arith -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -mlongcalls -ffunction-sections -fdata-sections -fstrict-volatile-bitfields" \
  CPPFLAGS="-mlongcalls -fno-exceptions -I/usr/local/lib/xtensa-esp32-elf/include -I/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/include -I/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/esp32/ -I/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/driver/ -I/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/soc/ -I/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include/freertos/" \
  LIBS="" \
  LDFLAGS="-L/usr/local/lib/xtensa-esp32-elf/lib/ -L/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/lib -L/home/mao/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/ld -L/home/mao/Desktop/esp/xtensa-esp32-toolchain/xtensa-esp32-elf/lib -nostdlib -Wl,--gc-sections -Wl,-EL -Wl,--undefined=uxTopUsedPriority"
```
```
chmod +x cross-compile-esp32.txt && ./cross-compile-esp32.txt
```
3. Change HAVE_ALLOCA and HAVE_ALLOCA_H to 0 from **config.h**.

4. Run ```make && make install``` to generate static library **libpbc.a**, which will be located in our case under folder **/usr/local/lib/xtensa-esp32-elf/lib**.
5. Done

### 3. Import pre-compiled libraries of step 1 and 2 into Arduino
In order to use our pre-compiled GMP and PBC libraries (**libgmp.a** and **libpbc.a**) in Arduino, we need to construct the folders in correct format for them under Arduino/libraries. \
See also the specification from Arduino: https://arduino.github.io/arduino-cli/library-specification/
#### 3.1 Add pre-compiled GMP library in Arduino
1. Constructed GMP library folder 
```
Arduino/libraries/gmp
├── library.properties
└── src
    ├── esp32
    │   └── libgmpesp32.a
    └── gmp.h
```
2. Copy header files and pre-compiled GMP library from **/usr/local/lib/xtensa-esp32-elf/** to Arduino library folder and **libgmp.a** is renamed as **libgmpesp32.a** in order to differ from the GMP library under linux machine.
3. library.properties file. **Note**: set precompiled flag to true.
```
name=GMP Library ESP32
version=1.0.0
author=Hangmao Liu
maintainer=Hangmao Liu
sentence=Arduino libary
paragraph=
category=demo
url=www.google.com
architectures=*
includes=gmp.h
precompiled=true
ldflags=-lgmpesp32
```

#### 3.2 Add pre-compiled PBC library in Arduino
1. Constructed PBC library folder
```
Arduino/libraries/pbc/
├── library.properties
└── src
    ├── esp32
    │   └── libpbcesp32.a
    ├── pbc_a1_param.h
    ├── pbc_a_param.h
    ├── pbc_curve.h
    ├── pbc_d_param.h
    ├── pbc_e_param.h
    ├── pbc_field.h
    ├── pbc_fieldquadratic.h
    ├── pbc_f_param.h
    ├── pbc_fp.h
    ├── pbc_g_param.h
    ├── pbc.h
    ├── pbc_hilbert.h
    ├── pbc_i_param.h
    ├── pbc_memory.h
    ├── pbc_mnt.h
    ├── pbc_multiz.h
    ├── pbc_pairing.h
    ├── pbc_param.h
    ├── pbc_poly.h
    ├── pbc_random.h
    ├── pbc_singular.h
    ├── pbc_ternary_extension_field.h
    ├── pbc_test.h
    ├── pbc_utils.h
    └── pbc_z.h
```
2. Copy header files and pre-compiled PBC library from **/usr/local/lib/xtensa-esp32-elf/** to Arduino library folder and **libpbc.a** is renamed as **libpbcesp32.a** in order to differ from the PBC library under linux machine.
3. library.properties file. **Note**: set precompiled flag to true.
```
name=PBC_Library(Pairing Based Cryptography) ESP32
version=1.0.0
author=Hangmao Liu
maintainer=Hangmao Liu
sentence=Arduino libary
paragraph=
category=demo
url=www.google.com
architectures=*
includes=pbc.h
precompiled=true
ldflags=-lpbcesp32
```
#### 3.3 Change platform.txt in Arduino in order to use pre-compiled library
See also the discussion for more details: https://forum.arduino.cc/index.php?topic=653746.0 \
**platform.txt**: file is located in **~/.arduino15/packages/esp32/hardware/esp32/1.0.4** (under norml user not root).
1. Add this line anywhere in platform.txt
```
compiler.libraries.ldflags=
```
2. Add {compiler.libraries.ldflags} to recipe.c.combine.pattern
```
recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} {compiler.libraries.ldflags} -Wl,--start-group {object_files} "{archive_file_path}" {compiler.c.elf.libs} -Wl,--end-group -Wl,-EL -o "{build.path}/{build.project_name}.elf"

```
3. Add this tow flages the end of the file 
```
compiler.c.elf.libs=-lgcc -lesp32 -lphy -lesp_http_client -lmbedtls -lrtc -lesp_http_server -lbtdm_app -lspiffs -lbootloader_sup    port -lmdns -lnvs_flash -lfatfs -lpp -lnet80211 -ljsmn -lface_detection -llibsodium -lvfs -ldl_lib -llog -lfreertos -lcxx -lsmar    tconfig_ack -lxtensa-debug-module -lheap -ltcpip_adapter -lmqtt -lulp -lfd -lfb_gfx -lnghttp -lprotocomm -lsmartconfig -lm -leth    ernet -limage_util -lc_nano -lsoc -ltcp_transport -lc -lmicro-ecc -lface_recognition -ljson -lwpa_supplicant -lmesh -lesp_https_    ota -lwpa2 -lexpat -llwip -lwear_levelling -lapp_update -ldriver -lbt -lespnow -lcoap -lasio -lnewlib -lconsole -lapp_trace -les    p32-camera -lhal -lprotobuf-c -lsdmmc -lcore -lpthread -lcoexist -lfreemodbus -lspi_flash -lesp-tls -lwpa -lwifi_provisioning -l    wps -lesp_adc_cal -lesp_event -lopenssl -lesp_ringbuf -lfr  **-lgmpesp32** **-lpbcesp32** -lstdc++
```

### Note
In our case, an error occurs during linking stage.
```
in libc_nano.a undefined reference to "_PathLocale"
in libc_nano.a undefined reference to "__mlocale_changed"
in libc_nano.a undefined reference to "__nlocale_changed"
```
As a workaround we define those 3 variables in gmp/errno.c file, since we have already modified it.
```
int _PathLocale = 0;
int __mlocale_changed = 0;
int __nlocale_changed = 0;
```

## Using pre-compiled GMP and PBC libraries
User can find a pre-compiled version of both libraries in the next folder: **esp32_crosscompile**
To use these libraris, user must: 
### 1. Add pre-compiled libraries into Arduino libraries folder
Locating the Arduino libraries folder:
- in case of using Linux:  **Arduino/libraries/**
- in case of using Windows 10: **C:\Users\usename\Documents\Arduino\libraries**

### 2. Change platform.txt in Arduino in order to use pre-compiled library
Locating the platform.txt:
- in case of using Linux: **~/.arduino15/packages/esp32/hardware/esp32/1.0.4**(under norml user not root)
- in case of using Windows 10: **C:\Users\usename\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\**

After locating the file: 
1. Add this line anywhere in platform.txt
```
compiler.libraries.ldflags=
```
2. Add {compiler.libraries.ldflags} to recipe.c.combine.pattern
```
recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} {compiler.libraries.ldflags} -Wl,--start-group {object_files} "{archive_file_path}" {compiler.c.elf.libs} -Wl,--end-group -Wl,-EL -o "{build.path}/{build.project_name}.elf"

```
3. Add this tow flages the end of the file 
```
compiler.c.elf.libs=-lgcc -lesp32 -lphy -lesp_http_client -lmbedtls -lrtc -lesp_http_server -lbtdm_app -lspiffs -lbootloader_sup    port -lmdns -lnvs_flash -lfatfs -lpp -lnet80211 -ljsmn -lface_detection -llibsodium -lvfs -ldl_lib -llog -lfreertos -lcxx -lsmar    tconfig_ack -lxtensa-debug-module -lheap -ltcpip_adapter -lmqtt -lulp -lfd -lfb_gfx -lnghttp -lprotocomm -lsmartconfig -lm -leth    ernet -limage_util -lc_nano -lsoc -ltcp_transport -lc -lmicro-ecc -lface_recognition -ljson -lwpa_supplicant -lmesh -lesp_https_    ota -lwpa2 -lexpat -llwip -lwear_levelling -lapp_update -ldriver -lbt -lespnow -lcoap -lasio -lnewlib -lconsole -lapp_trace -les    p32-camera -lhal -lprotobuf-c -lsdmmc -lcore -lpthread -lcoexist -lfreemodbus -lspi_flash -lesp-tls -lwpa -lwifi_provisioning -l    wps -lesp_adc_cal -lesp_event -lopenssl -lesp_ringbuf -lfr  **-lgmpesp32** **-lpbcesp32** -lstdc++
```
