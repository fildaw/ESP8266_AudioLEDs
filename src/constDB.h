#include <Arduino.h>
#include <ESP8266WiFi.h>

extern IPAddress ip;
extern IPAddress subnet;
extern IPAddress gateway;
extern uint16_t port;
extern uint8_t nLeds;
extern String ssid;
extern String psk_s;

const IPAddress defaultIP(192,168,1,150);
const IPAddress defaultSubnet(255,255,255,0);
const IPAddress defaultGateway(192,168,1,1);
const uint16_t defaultPort = 7777;
const uint8_t defaultNLeds = 30;
const String defaultSSID = "ABCDEF";
const String defaultPSK = "2fhfhfQ";