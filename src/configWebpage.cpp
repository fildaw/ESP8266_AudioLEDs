#include <configWebpage.h>

ESP8266WebServer server(80);

void handleSaveConfig() {
    if (server.method() != HTTP_POST) {
        server.send(405, "text/plain", "Method Not Allowed");
    } else {
        String message = "POST form was:\n";
        String errorS;
        bool error = false;
        for (uint8_t i = 0; i < server.args(); i++) {
            message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        for (uint8_t i = 0; i < server.args() && !error; i++) {
            if (server.argName(i).equals("ip")) {
                if (!ip.fromString(server.arg(i))) {
                    error = true;
                    errorS = "Bad ip address! (should be \"XXX.XXX.XXX.XXX\")";
                }
            } else if (server.argName(i).equals("subnet")) {
                if (!subnet.fromString(server.arg(i))) {
                    error = true;
                    errorS = "Bad subnet address! (should be \"XXX.XXX.XXX.XXX\")";
                }
            } else if (server.argName(i).equals("gateway")) {
                if (!gateway.fromString(server.arg(i))) {
                    error = true;
                    errorS = "Bad gateway address! (should be \"XXX.XXX.XXX.XXX\")";
                }
            } else if (server.argName(i).equals("port")) {
                int p = server.arg(i).toInt();
                if (p < 1 || p > 65535) {
                    error = true;
                    errorS = "Bad port! (should be between 1 and 65535)";
                }
                port = p;
            } else if (server.argName(i).equals("nleds")) {
                int n = server.arg(i).toInt();
                if (n < 3 || n > 255) {
                    error = true;
                    errorS = "Bad port! (should be between 3 and 255)";
                }
                nLeds = n;
            } else if (server.argName(i).equals("ssid")) {
                if (server.arg(i).length() < 1 || server.arg(i).length() > 32) {
                    error = true;
                    errorS = "Bad SSID (too many or too low characters)";
                }
                ssid = server.arg(i);
            } else if (server.argName(i).equals("psk")) {
                if (server.arg(i).length() < 8 || server.arg(i).length() > 63) {
                    error = true;
                    errorS = "Bad PSK (should be between 8 and 63 characters)";
                }
                psk_s = server.arg(i);
            }
        }
        Serial.println(message);
        if (error) {
            server.send(200, "text/plain", "Error occured: " + errorS);
            ledPulse(PULSE_SLOW, true, -1);
        } else {
            writeVals(ip, subnet, gateway, port, nLeds, ssid, psk_s);
            server.send(200, "text/plain", "Success! Values saved. Now restart ESP8266");
            ledPulse(PULSE_FAST, true, -1);
        }
    }
}

void showConfig() {
    String configHTML = configWebpageHTML;
    configHTML.replace("$SSID$", ssid);
    configHTML.replace("$PSK$", psk_s);
    configHTML.replace("$IP$", ip.toString());
    configHTML.replace("$SUBNET$", subnet.toString());
    configHTML.replace("$GATEWAY$", gateway.toString());
    configHTML.replace("$PORT$", String(port));
    configHTML.replace("$NLEDS$", String(nLeds));
    server.send(200, "text/html", configHTML);
}

void handleRoot() {
    server.send(200, "text/html", "<h1>Hello from ESP8266!</h1>");
}

void bootstrap() {
    Serial.println("Sending file...");
    File file = SPIFFS.open("/bootstrap.min.css.gz", "r");
    Serial.println(file.name());
    size_t sent = server.streamFile(file, "text/css");
}

void bootstrapmin() {
    Serial.println("Sending file...");
    File file = SPIFFS.open("/bootstrap.min.js.gz", "r");
    Serial.println(file.name());
    size_t sent = server.streamFile(file, "application/javascript");
}

void setupWebserver() {
    
    server.on("/", handleRoot);
    server.on("/config", showConfig);
    server.on("/save-config", handleSaveConfig);
    server.on("/bootstrap.min.css", bootstrap);
    server.on("bootstrap.min.css", bootstrap);
    server.on("/bootstrap.min.js", bootstrapmin);
    server.on("bootstrap.min.js", bootstrapmin);

    server.begin();
}

void waitForRequest() {
    server.handleClient();
}