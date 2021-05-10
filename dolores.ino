// do lores images on a neopixel matrix

#include <math.h>

#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "dolores.h"
PubSubClient setup_mqtt(MQTT_CALLBACK_SIGNATURE);

#include "secrets.h"

#define LEDS_PIN 4 // https://chewett.co.uk/blog/1066/pin-numbering-for-wemos-d1-mini-esp8266/


void setup_serial() {
  Serial.begin(115200);
  while  (!Serial) {;}
  Serial.println("HEY");
}


int brightness = 0;

void mqtt_receive_cb(char* topic, byte* payload, unsigned int length) {
  digitalWrite(BUILTIN_LED, LOW);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");
  if(string_has_suffix(topic, "image")) {
    set_led_values(payload, length, leds);
    update_strip_from_leds(leds);
  } else if(string_has_suffix(topic, "brightness")) {
    char * brightness_s = (char *) alloca(length+1);
    memcpy((void *)brightness_s, (void*) payload, length);
    brightness_s[length] = '\0';
    brightness = atoi(brightness_s);
    Serial.print("brightness = ");
    Serial.println(brightness);
    update_strip_from_leds(leds);
  }
  digitalWrite(BUILTIN_LED, HIGH);
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LENGTH, LEDS_PIN, NEO_GRB + NEO_KHZ800);

void update_strip_from_leds(led *leds) {
  strip.setBrightness(brightness);
  for(int i = 0; i< STRIP_LENGTH; i++) {
    if(brightness > 0) {
      struct led col = leds[i];
      strip.setPixelColor(i, strip.Color(col.r, col.g, col.b));
    } else {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
  }
  strip.show();
}

static PubSubClient mqtt_client;
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  setup_serial();
  connect_wifi();
  mqtt_client = setup_mqtt(mqtt_receive_cb);
  mqtt_client.setBufferSize(PAYLOAD_LENGTH + 128);
  otaSetup();
  strip.begin();
  set_led_values(0, 0, leds);
  update_strip_from_leds(leds);
  digitalWrite(BUILTIN_LED, HIGH);
  mqtt_reconnect();
  char topic[80];
  mqtt_client.subscribe(make_topic(topic,  sizeof topic, "/#"));
}

void loop() {
  if(!mqtt_client.loop()) mqtt_reconnect();
  strip.show();
  otaLoop();
  yield();
}
