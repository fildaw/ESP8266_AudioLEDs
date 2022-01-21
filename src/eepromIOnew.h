#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>

void readVals(IPAddress &ip, IPAddress &subnet, IPAddress &gateway, uint16_t &port, uint8_t &nLeds, String &ssid, String &psk);
void writeVals(IPAddress ip, IPAddress subnet, IPAddress gateway, uint16_t port, uint8_t nLeds, String ssid, String psk);
bool checkDefaults();
void zeroOutEEPROM(bool commit);