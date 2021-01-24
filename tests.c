#include "fns.ino"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int test_xy_to_index()
{
  assert(xy_to_index(0,0)==0);
  assert(xy_to_index(8,0)==8);
  assert(xy_to_index(14,0)==14);
  /* for(int y=0;y<8; y++) */
  /*   for(int x=0; x< 15;x++) */
  /*     printf("%d %d= %d \n", x, y, xy_to_index(x,y)); */
  assert(xy_to_index(0,1)==29);
  assert(xy_to_index(10,1)==19);
  assert(xy_to_index(2,2)==32);
}

static int test_set_node_id()
{
  assert(! strcmp("0123456789ab", set_node_id("01:23:45:67:89:ab")));
  assert(! strcmp("0123456789ab", node_id));
}

static int test_make_topic()
{
  char topic[sizeof sizeof(MQTT_TOPIC_PREFIX) + sizeof(node_id) + 11];
  puts(make_topic(topic, sizeof topic, "/mytopic"));
  assert(! strcmp("effects/0123456789ab/mytopic",
		  make_topic(topic, sizeof topic, "/mytopic")));
  assert(! strcmp("effects/0123456789ab/supercalif",
		  make_topic(topic, sizeof topic, "/supercalifragilisticexpialidocious")));
}

int main() {
  test_xy_to_index();
  test_set_node_id();
  test_make_topic();
}
