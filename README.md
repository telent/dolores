# Do Lo-res

Specifically, lo-resolution images on a 10x15 matrix of Neopixels
attached to an ESP8266. 

Send bytes of r g b r g b r g b ... to the MQTT topic

    effects/D8BFC0C7AA9A/image

where `D8BFC0C7AA9A` is the mac address of your esp8266

## Running tests

Unit tests for non-arduino functions are run on the host, to improve
development feedback loop times.  If you have Nix, you can do

    nix-build . -A test

Otherwise check `default.nix` for dependencies, install them (if there
are any) with whatever package manager you do have, and run `make test`

## Getting to the Arduino dev env

    nix-shell . -A arduino
