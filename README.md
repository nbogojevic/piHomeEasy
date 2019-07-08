#Control HomeEasy/Chacon/Byebyestandby devices

This tool can switch on and off the remote-controlled plugs using HomeEasy protocol.

More information about protocol can be found at:
- http://homeeasyhacking.wikia.com/wiki/Home_Easy_Hacking_Wiki
- http://playground.arduino.cc/Code/HomeEasy
- http://blog.idleman.fr/?p=1970

##Prerequisites

### Hardware

- Raspberry PI (A or B) and Raspberry Pi 2 Model B
- FS1000A 433MHz Wireless Transmitter Module for Arduino. Module can be found on ebay or various on-line electronics store. There are versions with mounted pins for Dupont cables and those were you need to solder cable. Make your choice, I prefer those with mounted pins.
- Dupont female-to-female wire cable (3 wires) 

### Software

The [wiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/) library is necessery for building and running the tool. You can install it using:

```bash
cd ~
sudo apt-get install git-core
git clone git://git.drogon.net/wiringPi
cd wiringPi
sudo ./build
```

## Building hardware interface
Follow these instructions on your own risk.

- Turn off Raspberry PI and unplug all cables. Access the electronics board.
- Locate [GPIO port](http://elinux.org/RPi_Low-level_peripherals#General_Purpose_Input.2FOutput_.28GPIO.29) on Raspberry PI.
- Connect one Dupont wire to pin 1 on Raspberry PI (it is 3.3 volt pin) to VCC pin on wireless transmitter module
- Connect another Dupont wire to pin 6 on Raspberry PI (it is ground pin) to GND pin on wireless transmitter module
- Connect third Dupont wire to pin 11 on Raspberry PI (it is GPIO 17 pin) to DATA pin on wireless transmitter module
- That's it, you can now plug in the cables into Raspberry PI and start it.


## Build and Install the tool

- Install wiringPi library as described.
- Get *piHomeEasy* source code from GitHub and move inside the project directory.
- To build the tool run: `make`
- To install the tool into */usr/local/bin* run:`sudo make install`

## Running the tool

To obtain help run:

	$ piHomeEasy
  
The help is as follows:
 
    Usage: piHomeEasy <pin number> <emitter id> <receiver id> <on|off>

      <pin number>  wiringPi pin number as follows:
                      number  GPIO|GPIO  number
                                 1|2
                      8          3|4
                      9          5|6
                      7          7|8         15
                                 9|10        16
                      0         11|12         1
                      2         13|14
                      3         15|16         4
                                17|18         5
                      12        19|20
                      9         21|22         6
                      14        23|24        10
                                25|26        11
      <emitter id>  Unique id of the emitter: a number between 0 and 67108863.
                      Example: 12325262
      <receiver id> Receiver id: a number between 0 and 15, or -1 for group command.
                      Group command is received by all receivers paired with the emitter id.
      <on|off>      Command to send:
                      on  to turn on
                      off to turn off

To control devices, as tool accesses hardware, it needs to be run as root. 

### Associating the devices

Each device has to be associated with Raspberry PI. Start your device in pairing mode and use tool to send on command (for example `sudo piHomeEasy 0 31415 1 on`). Your device will be paired with emitter and receiver ids that you used (31415 and 1 in our example).

Note the ids you used, as you will need them to control or unpair the device. To unpair the device, follow the instructions for device. Usually you need to start device in pairing mode and send off command. Use same ids that you used when pairing (in the above example, it would be `sudo piHomeEasy 0 31415 1 off`). 

### Control devices

Device is controlled using ids that have been used to pair the device.

To switch the device **on** use:
    
    $ sudo piHomeEasy 0 31415 1 on 

To switch the device **off** use:
    
    $ sudo piHomeEasy 0 31415 1 off 
    
If you associated several devices with the same emitter id, you can control all of them by passing -1 instead of receiver ids. This is called the group command.

To switch **on** all devices in group, use:
    
    $ sudo piHomeEasy 0 31415 -1 on 

To switch **off** all devices in group, use:
    
    $ sudo piHomeEasy 0 31415 -1 off 

When switched on or off as a group, devices can still be be controlled individually. Group command is equivalent of a broadcast.
 
## Device range

Range of transmitter module without antenna is couple of meters. With antenna it can be up to 30m in the open. Antenna should be 17.3 cm long and soldered to ANT contact.

Range can be improved by powering module with larger voltage (module is usally capable of handling up to 12V). It can be external power source or possibly 5V source from Raspberry PI (**not tested**).

## Thanks

- [idleman](http://blog.idleman.fr) for excelent tutorials in french 
- Arduino community for original [code and analysis](http://playground.arduino.cc/Code/HomeEasy)


