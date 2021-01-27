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

void setup_serial() {
  Serial.begin(115200);
  while  (!Serial) {;}
  Serial.println("HEY");
}

void connect_wifi() {
  delay(10);

  Serial.print("Connecting to ");
  Serial.println(WIFISSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFISSID, WIFIPASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(mqtt_receive_cb);
}

void mqtt_receive_cb(char* topic, byte* payload, unsigned int length) {
  digitalWrite(BUILTIN_LED, LOW);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((int)payload[i]);
    Serial.print(" ");
  }
  Serial.println("");

  set_led_values(payload, length, leds);
  digitalWrite(BUILTIN_LED, HIGH);
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LENGTH, LEDS_PIN, NEO_GRB + NEO_KHZ800);

void update_strip_from_leds(led *leds) {
  for(int i = 0; i< STRIP_LENGTH; i++) {
    struct led col = leds[i];
    strip.setPixelColor(i, strip.Color(col.r, col.g, col.b));
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
      mqttClient.publish(make_topic(topic, sizeof topic, "/online"), "\1");
      mqttClient.subscribe(make_topic(topic, sizeof topic, "/image"));
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
  connect_wifi();
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
