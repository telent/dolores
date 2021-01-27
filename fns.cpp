#include <string.h>

#include "dolores.h"

int xy_to_index(int x, int y)
{
  // alternate rows are wired "backwards", thus:

  // 0   1  2  3  4  5  6  7
  // 15 14 13 12 11 10  9  8
  // 16 17 18 19 20 21 22 23

  y = BOARD_HEIGHT - y -1;
  int base = y*BOARD_WIDTH;
  if (y%2) {
    // odd-numbered row runs right to left
    return base + BOARD_WIDTH - x -1;
  } else {
    return base + x;
  }
}

char node_id[13] = "000000000000";

char * set_node_id(const char * mac_address)
{
  unsigned int i;
  char *p = node_id;
  for(i=0; i < strlen(mac_address); i+=3) {
    *p++ = mac_address[i];
    *p++ = mac_address[i+1];
  }
  *p++ = '\0';
  return node_id;
}

char *make_topic(char *dest, int dest_bytes, const char *suffix)
{
  int prefix_length = sizeof(MQTT_TOPIC_PREFIX) + sizeof(node_id);
  char * topic = dest;
  strcpy(topic, MQTT_TOPIC_PREFIX);
  strcat(topic, node_id);
  strncat(topic, suffix, dest_bytes - prefix_length);
  topic[dest_bytes - 1] = '\0';
  return topic;
}

struct led leds[STRIP_LENGTH];

struct led * set_led_values(byte *payload, int payload_size, struct led *leds) {
  int loc = 0, x=0, y=0;
  for(int i=0; i < BOARD_WIDTH * BOARD_HEIGHT * 3; i+=3) {
    loc = xy_to_index(x,y);
    if(i + 2 < payload_size) {
      leds[loc].r = payload[i];
      leds[loc].g = payload[i+1];
      leds[loc].b = payload[i+2];
    } else {
      leds[loc].r = 0;
      leds[loc].g = 0;
      leds[loc].b = 0;
    }
    x++;
    if(x>= BOARD_WIDTH) {
      x=0;
      y++;
    }
  }
  return leds;
}
