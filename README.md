# SNMAPI #

This project is an API for the Playstation 3. You need a jailbroken PS3 to use this API. It provides features like setting and getting the memory of every process (like the current game). This API is based on a TCP socket and internal LV2 calls.

## Getting started ##
These instructions will you help to setup a working copy on your local machine. 

### Setup for compiling ###
TODO

### Installing ###
1. Rename `dev_flash/sys/internal/sys_audio.sprx` to `sys_SNMAPI.sprx`. 
2. Copy the `sys_audio.sprx` you just compiled from your PC to `dev_flash/sys/internal/`. 

Note that you need to mount `dev_flash` in order to make it writeable. 

### Finding the Playstation 3 ###
- During the start-up phase the PS3 sets up a UDP Server which broadcasts the string `SNMAPI_PS3` on port **1337** over the Wi-Fi. A client can listen on this port and resolve the IP Address of the PS3.

- After retrieving the IP, a client is able to connect to the PS3 via TCP on port **1337**. Once a connection is established, commands can be sent to the PS3. 

### Commands ###
**Get Version**  
Command ID: 1  
Payload: None  
Return Type: A string like `4.80 DEX` or `4.70 CEX`

**Notify**  
Command ID: 2  
Payload: 
```
typedef struct
{
    uint8_t texture;
    char message[128];
}snmapi_notify_struct;
```
Return Type: Int, `0` for success and `-1` for error

...


## Authors ##
- Anton Scharnowski - Development and Research
- Jannik Wortmann - Development and Research

## Acknowledgements ##

