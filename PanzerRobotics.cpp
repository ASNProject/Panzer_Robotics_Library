// Copyright 2026 ariefsetyonugroho
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     https://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PanzerRobotics.h"
#include <EEPROM.h>
#include <string.h>   

#define EEPROM_SSID_ADDR 0
#define EEPROM_PASS_ADDR 64

// ===== Camera Streaming Setup =====
const char PanzerRobotics::HEADER[] =
"HTTP/1.1 200 OK\r\n"
"Access-Control-Allow-Origin: *\r\n"
"Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";

const char PanzerRobotics::BOUNDARY[] =
"\r\n--123456789000000000000987654321\r\n";

const char PanzerRobotics::CTNTTYPE[] =
"Content-Type: image/jpeg\r\nContent-Length: ";

const int PanzerRobotics::hdrLen = strlen(HEADER);
const int PanzerRobotics::bdrLen = strlen(BOUNDARY);
const int PanzerRobotics::cntLen = strlen(CTNTTYPE);

PanzerRobotics::PanzerRobotics() : server(80) {
    EEPROM.begin(EEPROM_SIZE);
    wiFiState = WIFI_IDLE;
    wifiStartTime = 0;
}

// ================= REST =================

String PanzerRobotics::send(const char* serverUrl, StaticJsonDocument<200>& jsonDoc) {
    if (WiFi.status() != WL_CONNECTED) return "Error: Wifi not connected.";

    HTTPClient http;
    http.begin(serverUrl);
    http.setTimeout(5000);

    http.addHeader("Content-Type", "application/json");

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    int code = http.POST(jsonData);
    String response = (code > 0) ? http.getString() : http.errorToString(code);

    http.end();
    return response;
}

String PanzerRobotics::get(const char* serverUrl) {
    if (WiFi.status() != WL_CONNECTED) return "Error: Wifi not connected.";

    HTTPClient http;
    http.begin(serverUrl);
    http.setTimeout(5000);

    int code = http.GET();
    String response = (code > 0) ? http.getString() : http.errorToString(code);

    http.end();
    return response;
}

String PanzerRobotics::update(const char* serverUrl, StaticJsonDocument<200>& jsonDoc) {

    if (WiFi.status() != WL_CONNECTED) return "Error: Wifi not connected.";

    HTTPClient http;
    http.begin(serverUrl);
    http.setTimeout(5000);

    http.addHeader("Content-Type", "application/json");

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    int code = http.PUT(jsonData);

    String response = (code > 0) ? http.getString() : http.errorToString(code);

    http.end();
    return response;
}

String PanzerRobotics::getById(const char* serverUrl, const char* id) {
    if (WiFi.status() != WL_CONNECTED) return "Error: Wifi not connected.";

    HTTPClient http;
    http.begin(String(serverUrl) + "/" + String(id));
    http.setTimeout(5000);

    int code = http.GET();
    String response = (code > 0) ? http.getString() : http.errorToString(code);

    http.end();
    return response;
}

// ================= CAMERA =================

String PanzerRobotics::esp32cam() {

    camera_config_t config;
    memset(&config, 0, sizeof(config));  // FIX memory safety

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    static bool camInitialized = false;
    if (!camInitialized) {
        cam.init(config);
        camInitialized = true;
    }

    IPAddress ip = WiFi.localIP();

    server.on("/mjpeg/1", HTTP_GET, std::bind(&PanzerRobotics::handleJPGStream, this));
    server.on("/jpg", HTTP_GET, std::bind(&PanzerRobotics::handleJPG, this));
    server.onNotFound(std::bind(&PanzerRobotics::handleNotFound, this));

    static bool serverStarted = false;
    if (!serverStarted) {
        server.begin();
        serverStarted = true;
    }

    return "http://" + ip.toString() + "/mjpeg/1";
}

void PanzerRobotics::handleJPGStream() {

    WiFiClient client = server.client();
    if (!client) return;

    client.write(HEADER, hdrLen);
    client.write(BOUNDARY, bdrLen);

    unsigned long streamStart = millis();

    while (client.connected()) {

        if (millis() - streamStart > 600000) break;

        cam.run();
        int s = cam.getSize();
        if (s <= 0) continue;

        client.write(CTNTTYPE, cntLen);

        char buf[32];
        sprintf(buf, "%d\r\n\r\n", s);
        client.write(buf, strlen(buf));

        client.write((char*)cam.getfb(), s);
        client.write(BOUNDARY, bdrLen);

        delay(1);
        yield();
    }

    client.stop();
}

void PanzerRobotics::handleJPG() {

    WiFiClient client = server.client();
    if (!client) return;

    cam.run();
    if (!client.connected()) return;

    int size = cam.getSize();
    if (size <= 0) return;

    const char header[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/jpeg\r\n\r\n";

    client.write(header, strlen(header));
    client.write((char*)cam.getfb(), size);

    client.stop();
}

void PanzerRobotics::handleNotFound() {
    server.send(200, "text/plain", "Server is running!");
}

// ================= OTA =================

bool PanzerRobotics::checkUpdate(const char* updateUrl, double currentVersion) {

    if (WiFi.status() != WL_CONNECTED) return false;

    HTTPClient http;
    http.begin(updateUrl);
    http.setTimeout(5000);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, response)) return false;
    if (!doc.containsKey("version")) return false;

    double newVersion = doc["version"];
    return newVersion > currentVersion;
}

void PanzerRobotics::updateFirmware(const char* updateUrl) {

    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(updateUrl);
    http.setTimeout(5000);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        http.end();
        return;
    }

    String response = http.getString();
    http.end();

    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, response)) return;
    if (!doc.containsKey("url")) return;

    String firmwareUrl = doc["url"];

    ESPhttpUpdate.rebootOnUpdate(true);
    ESPhttpUpdate.update(firmwareUrl);
}

// ================= WIFI =================

void PanzerRobotics::wifiBegin(const char* apSsid, const char* apPass) {
    
    EEPROM.begin(512);

    WiFi.mode(WIFI_AP_STA);

    if (connectWiFiFromEEPROM()) {
        wiFiState = WIFI_CONNECTED;
    } else {
        startAPMode(apSsid, apPass);
        wiFiState = WIFI_AP_MODE;
    }

    setupWeb();
    server.begin();
}

bool PanzerRobotics::connectWiFiFromEEPROM(){

    WiFiConfig config;
    EEPROM.get(0, config);

    if (config.magic != EEPROM_MAGIC) {
        return false;
    }

    if (strlen(config.ssid) == 0 || strlen(config.ssid) > 32) {
        return false;
    }

    WiFi.begin(config.ssid, config.pass);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(100);
        yield();
    }

    return WiFi.status() == WL_CONNECTED;
}

void PanzerRobotics::startAPMode(const char* apSsid, const char* apPass) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPass);
}

// ================= WEB =================

void PanzerRobotics::setupWeb() {

    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", webPage());
    });

    server.on("/status", HTTP_GET, [this]() {
        handleStatus();
    });

    server.on("/save", HTTP_POST, [this]() {

        String ssidStr = server.arg("ssid");
        String passStr = server.arg("pass");

        WiFiConfig config;
        memset(&config, 0, sizeof(config));

        config.magic = EEPROM_MAGIC;

        strncpy(config.ssid, ssidStr.c_str(), sizeof(config.ssid) - 1);
        strncpy(config.pass, passStr.c_str(), sizeof(config.pass) - 1);

        EEPROM.put(0, config);
        EEPROM.commit();

        server.send(200, "text/html", "Saved. Rebooting...");
        server.client().stop();
        delay(1000);
        ESP.restart();
    });

    server.on("/disconnect", HTTP_POST, [this]() {
        disconnectWiFi();
    });
}

void PanzerRobotics::handleClient() {
    static unsigned long lastReconnectAttempt = 0;

    if (WiFi.getMode() == WIFI_STA) {

        if (WiFi.status() != WL_CONNECTED) {
            if (millis() - lastReconnectAttempt > 10000) {
                WiFi.reconnect();
                lastReconnectAttempt = millis();
            }
            wiFiState = WIFI_DISCONNECTED;
        } else {
            wiFiState = WIFI_CONNECTED;
        }
    }
    server.handleClient();
}

WiFiInfo PanzerRobotics::getWiFiInfo() {

    WiFiInfo info;

    info.status = wiFiState;

    if (WiFi.status() == WL_CONNECTED) {
        info.ssid = WiFi.SSID();
        info.ip = WiFi.localIP().toString();
        info.gateway = WiFi.gatewayIP().toString();
        info.subnet = WiFi.subnetMask().toString();
        info.rssi = WiFi.RSSI();
    } else {
        info.ssid = "";
        info.ip = "";
        info.gateway = "";
        info.subnet = "";
        info.rssi = 0;
    }

    info.ap_ip = WiFi.softAPIP().toString();

    return info;
}

bool PanzerRobotics::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String PanzerRobotics::webPage() {

    String mode = (WiFi.status() == WL_CONNECTED) ? "Station (Connected)" : "Access Point";
    String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
    String ssid = WiFi.SSID();
    int rssi = WiFi.RSSI();

    String html = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Panzer Robotics WiFi Manager</title>
        <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f6f8;
            margin: 0;
            padding: 20px;
        }
        .container {
            max-width: 500px;
            margin: auto;
            background: #ffffff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0,0,0,0.1);
        }
        h2 {
            text-align: center;
        }
        input[type=text], 
        input[type=password],
        button {
            width: 100%;
            padding: 10px;
            margin: 6px 0 12px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-sizing: border-box;  
            font-size: 16px;
        }
        button:hover {
            background-color: #1565c0;
        }
        .status {
            background: #f1f1f1;
            padding: 10px;
            border-radius: 5px;
            margin-bottom: 15px;
            font-size: 14px;
        }
        .disconnect {
            background-color: #d32f2f;
            margin-top: 10px;
        }
        .disconnect:hover {
            background-color: #b71c1c;
        }
        footer {
            text-align: center;
            font-size: 12px;
            margin-top: 15px;
            color: #777;
        }
        </style>
        </head>
        <body>

        <div class="container">
        <h2>Panzer Robotics WiFi Setup</h2>

        <div class="status">
        <b>Mode:</b> )rawliteral";

            html += mode;

            html += R"rawliteral(<br>
        <b>SSID:</b> )rawliteral";

            html += ssid;

            html += R"rawliteral(<br>
        <b>IP Address:</b> )rawliteral";

            html += ip;

            html += R"rawliteral(<br>
        <b>RSSI:</b> )rawliteral";

            html += String(rssi);

            html += R"rawliteral( dBm
        </div>

        <form method="POST" action="/save">
        <label>SSID</label>
        <input type="text" name="ssid" required>

        <label>Password</label>
        <input type="password" name="pass">

        <button type="submit">Save & Reboot</button>
        </form>

        <form method="POST" action="/disconnect">
        <button type="submit" class="disconnect">Disconnect WiFi</button>
        </form>

        <footer>
        Panzer Robotics © 2026
        </footer>

        </div>

        <script>
            let isTyping = false;

            document.querySelectorAll("input").forEach(input => {
                input.addEventListener("focus", () => isTyping = true);
                input.addEventListener("blur", () => isTyping = false);
            });

            setInterval(function(){
                if (!isTyping) {
                    location.reload();
                }
            }, 15000);
        </script>

        </body>
        </html>
        )rawliteral";

    return html;
}

void PanzerRobotics::handleStatus() {

    StaticJsonDocument<256> doc;

    doc["connected"] = (WiFi.status() == WL_CONNECTED);
    doc["ssid"] = WiFi.SSID();
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
    doc["ap_ip"] = WiFi.softAPIP().toString();

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

void PanzerRobotics::disconnectWiFi() {

    WiFi.disconnect(true, true);
    delay(500);

    WiFi.mode(WIFI_AP);
    delay(200);

    startAPMode("Panzer_Setup", "1234567890"); 

    wiFiState = WIFI_AP_MODE;

    server.send(200, "text/plain", "Switched to AP Mode");
}