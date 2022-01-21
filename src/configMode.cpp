#include <configMode.h>

void configMode() {
  IPAddress localIp(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  WiFi.disconnect();
  WiFi.softAPConfig(localIp, gateway, subnet);
  WiFi.softAP(CONFIG_SSID, CONFIG_PSK, 2, false, 2);
  setupWebserver();
  digitalWrite(LED_BUILTIN, LOW);
  while (true) {
    waitForRequest();
  }
}