#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NeoPixelBus.h>
#include <eepromIOnew.h>
#include <configMode.h>
#include <ledPulse.h>

#include <EEPROM.h>
#if defined(ESP8266)
#include <WiFiUdp.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "This is not a ESP8266 or ESP32!"
#endif

// Set to the number of LEDs in your LED strip
#define NUM_LEDS 60 // MAX 256
// Maximum number of packets to hold in the buffer. Don't change this.
#define BUFFER_LEN 1024
// Toggles FPS output (1 = print FPS over serial, 0 = disable output)
#define PRINT_FPS 1

char packetBuffer[BUFFER_LEN];
WiFiUDP udpReceiver;
//NeoPixelBus settings
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266(set to 3 by default for DMA)
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> ledstrip(NUM_LEDS, PixelPin);

#if PRINT_FPS
    uint16_t fpsCounter = 0;
    uint32_t secondTimer = 0;
#endif

uint8_t N = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
  while (!Serial);
  delay(100);
  EEPROM.begin(512);
  if (!checkDefaults()) {
    Serial.print("No values found, writing defaults...");
    writeVals(defaultIP, defaultSubnet, defaultGateway, defaultPort, defaultNLeds, defaultSSID, defaultPSK);
  } else {
    Serial.println("Some values are already written!");
  }
  readVals(ip, subnet, gateway, port, nLeds, ssid, psk_s);
  Serial.println("Found SSID: " + ssid);
  Serial.println("Found PSK: " + psk_s);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(0, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, psk_s);
  Serial.println("");
  // Connect to wifi and print the IP address over serial
  int timeD = 0;
  while (WiFi.status() != WL_CONNECTED && timeD < 10) {
      delay(500);
      Serial.print(".");
      timeD++;
  }
  if (timeD < 10) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ledPulse(PULSE_FAST, false, 2);
  } else {
    Serial.println("Connection to wifi failed");
    ledPulse(PULSE_SLOW, false, 2);
  }
  
  udpReceiver.begin(port);
  
  ledstrip.Begin();//Begin output
  ledstrip.Show();//Clear the strip for use
  Serial.println("Sketch started!");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  // put your main code here, to run repeatedly:
  if (digitalRead(0) == LOW) {
    Serial.println("Entering config mode...");
    configMode();
  }
  
  // Read data over socket
  int packetSize = udpReceiver.parsePacket();
  // If packets have been received, interpret the command
  if (packetSize) {
      int len = udpReceiver.read(packetBuffer, BUFFER_LEN);
      for(int i = 0; i < len; i+=4) {
          packetBuffer[len] = 0;
          N = packetBuffer[i];
          RgbColor pixel((uint8_t)packetBuffer[i+1], (uint8_t)packetBuffer[i+2], (uint8_t)packetBuffer[i+3]);//color
          ledstrip.SetPixelColor(N, pixel);//N is the pixel number
      } 
      ledstrip.Show();
      #if PRINT_FPS
          fpsCounter++;
          Serial.print("/");//Monitors connection(shows jumps/jitters in packets)
      #endif
  }
  #if PRINT_FPS
      if (millis() - secondTimer >= 1000U) {
          secondTimer = millis();
          Serial.printf("FPS: %d\n", fpsCounter);
          fpsCounter = 0;
      }   
  #endif
}
