#define MAX_MQTT_SERVER_LENGTH 40
#define MAX_MQTT_PORT_LENGTH 6

// Our configuration structure.
//
// Never use a JsonDocument to store the configuration!
// A JsonDocument is *not* a permanent storage; it's only a temporary storage
// used during the serialization phase. See:
// https://arduinojson.org/v6/faq/why-must-i-create-a-separate-config-object/
struct Config {
  char mqtt_server[MAX_MQTT_SERVER_LENGTH];
  char mqtt_port[MAX_MQTT_PORT_LENGTH];
};
