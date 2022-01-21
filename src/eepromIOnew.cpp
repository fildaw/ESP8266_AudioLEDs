#include <eepromIOnew.h>

const uint8_t ssidTerminator = 0xAB;
const uint8_t pskTerminator = 0xAC;

const uint8_t magic0 = 0xAF; // Determines if values were already saved (ie decide to save defaults)
const uint8_t magic1 = 0xFE;
const uint8_t magicOffset = 200;

// PRIVATE:
IPAddress readIP(int &offset) {
    uint8_t ipTab[4] = {};
    for (int i = 0; i < 4; i++) {
        ipTab[i] = EEPROM.read(offset);
        offset++;
    }
    return IPAddress(ipTab[0],ipTab[1],ipTab[2],ipTab[3]);
}

void writeIP(int &offset, IPAddress ip) {
    for (int i = 0; i < 4; i++) {
        EEPROM.write(offset, ip[i]);
        offset++;
    }
}

uint16_t readTwoBytes(int &offset) {
    uint8_t f = EEPROM.read(offset);
    offset++;
    uint8_t s = EEPROM.read(offset);
    offset++;

    return ((uint16_t)f << 8) | s;
}

void writeTwoBytes(int &offset, int16_t val) {
    uint8_t f = (uint8_t)((val & 0xFF00) >> 8);
    uint8_t s = (uint8_t)(val & 0x00FF);
    EEPROM.write(offset, f);
    offset++;
    EEPROM.write(offset, s);
    offset++;
}

String readString(int &offset, uint8_t terminator) {
    String s = "";
    char read = EEPROM.read(offset);
    while (read != terminator) {
        s += read;
        offset++;
        read = EEPROM.read(offset);
    }
    offset++;

    return s;
}

void writeString(int &offset, String s, uint8_t terminator, int limit) {
    for (int i = 0; i < s.length() && i < limit; i++) {
        EEPROM.write(offset, s[i]);
        offset++;
    }
    EEPROM.write(offset, terminator);
    offset++;
}

//  PUBLIC:
bool checkDefaults() {
  if (EEPROM.read(magicOffset) == magic0 && EEPROM.read(magicOffset+1) == magic1) {
    return true;
  }

  return false;
}

void zeroOutEEPROM(bool commit) {
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  if (commit) {
      EEPROM.commit();
  }
}

void readVals(IPAddress &ip, IPAddress &subnet, IPAddress &gateway, uint16_t &port, uint8_t &nLeds, String &ssid, String &psk) {
    Serial.println("Reading vals from EEPROM...");
    int addr = 0;
    
    ip = readIP(addr);
    subnet = readIP(addr);
    gateway = readIP(addr);
    port = readTwoBytes(addr);
    nLeds = EEPROM.read(addr); addr++;
    ssid = readString(addr, ssidTerminator);
    psk = readString(addr, pskTerminator);
    Serial.println("Values successfully read from EEPROM!");
}

void writeVals(IPAddress ip, IPAddress subnet, IPAddress gateway, uint16_t port, uint8_t nLeds, String ssid, String psk) {
    Serial.println("Writing data...");
    Serial.println("Zeroing EEPROM...");
    zeroOutEEPROM(false);
    Serial.println("EEPROM erased.");
    int addr = 0;

    writeIP(addr, ip);
    writeIP(addr, subnet);
    writeIP(addr, gateway);
    writeTwoBytes(addr, port);
    EEPROM.write(addr, nLeds); addr++;
    writeString(addr, ssid, ssidTerminator, 32);
    writeString(addr, psk, pskTerminator, 63);

    //Write magic
    EEPROM.write(magicOffset, magic0);
    EEPROM.write(magicOffset+1, magic1);

    EEPROM.commit();
    Serial.println("Values successfully written!");
}