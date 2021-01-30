# Do Lo-res

Specifically, lo-resolution images on a 10x15 matrix of Neopixels
attached to an ESP8266. 

When it starts it will send a message to the topic

    effects/D8BFC0C7AA9A/online

where `D8BFC0C7AA9A` is the mac address of your esp8266. Then you can
send bytes of r g b r g b r g b ... to the MQTT topic

    effects/D8BFC0C7AA9A/image


## Running tests

Unit tests for non-arduino functions are run on the host, to improve
development feedback loop times.  If you have Nix, you can do

    nix-build . -A test

Otherwise check `default.nix` for dependencies, install them (if there
are any) with whatever package manager you do have, and run `make test`

## Getting to the Arduino dev environment

    nix-shell . -A arduino

The first time you build it, you will need to add the appropriate
board and packages in your Arduino dev environment. You can do this in the IDE
or you might try some cli incantation approximately like this:

    arduino --pref boardsmanager.additional.urls=http://arduino.esp8266.com/stable/package_esp8266com_index.json --install-boards  esp8266:esp8266   --save-prefs
    arduino --board esp8266:esp8266:d1   --save-prefs



## Building

You can build it interactively in the Arduino IDE, or you could try

    echo "make out/dolores.ino.bin" | nix-shell . -A arduino  

An HTTP OTA function is included, so after the first flash you can run

    curl -F "image=@out/dolores.ino.bin" http://192.168.0.9/update

(substitute the IP address of your esp8266 device as appropriate)


