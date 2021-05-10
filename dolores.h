#include <stdint.h>
typedef uint8_t byte;

#define MQTT_TOPIC_PREFIX "effects/"

#define BOARD_WIDTH 15
#define BOARD_HEIGHT 10

#define STRIP_LENGTH (BOARD_WIDTH * BOARD_HEIGHT)
#define PAYLOAD_LENGTH (STRIP_LENGTH * 3)

struct led { byte r,g,b ; };

extern struct led leds[STRIP_LENGTH];

extern char node_id[13];

struct led * set_led_values(byte *payload, int payload_size, struct led *leds);
char *make_topic(char *dest, int dest_bytes, const char *suffix);
char * set_node_id(const char * mac_address);
int xy_to_index(int x, int y);
void otaSetup(void);
void otaLoop(void);
int string_has_suffix(char *input, char * suffix);
uint32_t crc32(const uint8_t *data, size_t length);
