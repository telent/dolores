typedef unsigned char uint8_t;
typedef unsigned char byte;

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dolores.h"

static void test_xy_to_index()
{
  assert(xy_to_index(0,9)==0);
  assert(xy_to_index(8,9)==8);
  assert(xy_to_index(14,9)==14);
  assert(xy_to_index(0,8)==29);
  assert(xy_to_index(10,8)==19);
  assert(xy_to_index(2,7)==32);
}

static void test_set_node_id()
{
  assert(! strcmp("0123456789ab", set_node_id("01:23:45:67:89:ab")));
  assert(! strcmp("0123456789ab", node_id));
}

static void test_make_topic()
{
  char topic[sizeof sizeof(MQTT_TOPIC_PREFIX) + sizeof(node_id) + 11];

  assert(! strcmp("effects/0123456789ab/mytopic",
		  make_topic(topic, sizeof topic, "/mytopic")));
  assert(! strcmp("effects/0123456789ab/supercali",
		  make_topic(topic, sizeof topic, "/supercalifragilisticexpialidocious")));
}

static void test_set_led_values()
{
  struct led leds[STRIP_LENGTH] = {
    {1,1,1},
    {2,2,2},
    {3,3,3},
    {4,4,4},
    {5,5,5},
    {6,6,6},
    {7,7,7},
    {8,8,8},
    {9,9,9},
    {10,10,10},
  };
  unsigned int canary = 0xdeadcafe;
  byte payload[STRIP_LENGTH]= {
    1,2,3,
    4,5,6,
    7,8,9,
    1,1,1,
    67,68,69,
  };
  set_led_values(payload, sizeof payload, leds);

  // check data in payload is copied to strip
  assert(leds[xy_to_index(2,0)].r==7);
  assert(leds[xy_to_index(2,0)].g==8);
  assert(leds[xy_to_index(2,0)].b==9);
  assert(leds[xy_to_index(4,0)].r==67);
  assert(leds[xy_to_index(4,0)].g==68);
  assert(leds[xy_to_index(4,0)].b==69);
  // if payload is smaller than led array, remaining leds are zeroed
  assert(leds[xy_to_index(5,0)].r==0);
  assert(leds[xy_to_index(5,0)].g==0);
  assert(leds[xy_to_index(5,0)].b==0);

  // check we limit at STRIP_LENGTH if payload is mahoosive
  int big_payload_size = sizeof (byte) * STRIP_LENGTH * 4 + 25;
  byte * payload2 = (byte *) malloc(big_payload_size);
  memset(payload2, 42, big_payload_size);
  set_led_values(payload2, big_payload_size, leds);
  assert(canary==0xdeadcafe);
}

int main() {
  test_xy_to_index();
  test_set_node_id();
  test_make_topic();
  test_set_led_values();
}
