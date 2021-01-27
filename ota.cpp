// ==========================================================

/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>

#include "dolores.h"
#include "secrets.h"

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void otaSetup() {

  MDNS.begin(node_id);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", node_id);
}

void otaLoop(void) {
  httpServer.handleClient();
  MDNS.update();
}
