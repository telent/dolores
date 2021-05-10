// do lores images on a neopixel matrix

#include <math.h>

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "dolores.h"

#include "secrets.h"

#define LEDS_PIN 4 // https://chewett.co.uk/blog/1066/pin-numbering-for-wemos-d1-mini-esp8266/

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int brightness = 50;

void setup_serial() {
  Serial.begin(115200);
  while  (!Serial) {;}
  Serial.println("HEY");
}

// https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/

// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The
// access methods read and write 4 bytes at a time, so the RTC data
// structure should be padded to a 4-byte multiple.
struct wifi_settings {
  uint32_t crc32;   // 4 bytes
  uint8_t channel;  // 1 byte,   5 in total
  uint8_t bssid[6]; // 6 bytes, 11 in total
  uint8_t padding;  // 1 byte,  12 in total
} wifi_settings;

static struct wifi_settings * read_wifi_settings() {
  static bool rtc_valid = false;
  if(!rtc_valid) {
    if(ESP.rtcUserMemoryRead(0, (uint32_t*)&wifi_settings,
			     sizeof (wifi_settings))) {
      // Calculate the CRC; skip the first 4 bytes as that's the
      // checksum itself.
      uint32_t crc = crc32(((uint8_t*)&wifi_settings) + 4,
			   sizeof (wifi_settings) - 4);
      if( crc == wifi_settings.crc32 ) {
	rtc_valid = true;
      }
    }
  }
  return rtc_valid ? &wifi_settings : (struct wifi_settings *) 0;
}

static void write_wifi_settings() {
  uint32_t previous_crc = wifi_settings.crc32;
  // Write current connection info back to RTC
  wifi_settings.channel = WiFi.channel();
  memcpy(wifi_settings.bssid, WiFi.BSSID(), 6); 
  wifi_settings.crc32 = crc32(((uint8_t*) &wifi_settings) + 4,
			      (sizeof wifi_settings) - 4);
  if(wifi_settings.crc32 != previous_crc) {
    ESP.rtcUserMemoryWrite(0,
			   (uint32_t*) &wifi_settings,
			   sizeof wifi_settings);
  }
}

bool connect_wifi(struct wifi_settings * settings){
  delay(10);

  Serial.print("Connecting to ");
  Serial.println(WIFISSID);
  int retries = 0;

  WiFi.mode(WIFI_STA);

  if(settings) {
    WiFi.begin(WIFISSID, WIFIPASSWORD,
	       settings->channel, settings->bssid, true );
  } else {
      WiFi.begin(WIFISSID, WIFIPASSWORD);
  }
  while (WiFi.status() != WL_CONNECTED) {
    if((retries > 100) && settings) {
      WiFi.disconnect(); delay(10);
      WiFi.forceSleepBegin(); delay(10);
      WiFi.forceSleepWake(); delay(10);
      return false;
    }
    if(retries > 600) {
      WiFi.disconnect(true);
      delay(1);
      WiFi.mode(WIFI_OFF);
      ESP.deepSleep(60 * 5 * 1000 * 1000, WAKE_RF_DISABLED );
      return false; // notreached
    }
    delay(50);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void setup_mqtt() {
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(mqtt_receive_cb);
}

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

#define TOPIC_PREFIX "effects/"
void mqttReconnect() {
  char topic[80];
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    mqttClient.setBufferSize(PAYLOAD_LENGTH + 128);
    if (mqttClient.connect(node_id, MQTT_USER, MQTT_PASSWORD )) {
      Serial.print("connected\nmax buffer size ");
      Serial.println(mqttClient.getBufferSize());
      mqttClient.publish(make_topic(topic, sizeof topic, "/online"),
			 WiFi.localIP().toString().c_str());
      mqttClient.subscribe(make_topic(topic, sizeof topic, "/#"));
      Serial.print("topic "); Serial.println(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  setup_serial();
  connect_wifi(read_wifi_settings()) || connect_wifi(0);
  write_wifi_settings();
  set_node_id(WiFi.macAddress().c_str());
  setup_mqtt();
  otaSetup();
  strip.begin();
  set_led_values(0, 0, leds);
  update_strip_from_leds(leds);
  digitalWrite(BUILTIN_LED, HIGH);
  mqttReconnect();
}

void loop() {
  if(!mqttClient.loop()) mqttReconnect();
  strip.show();
  otaLoop();
  yield();
}
