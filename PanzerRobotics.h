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

#pragma once
#ifndef PanzerRobotics_h
#define PanzerRobotics_h

#define PANZER_LIBRARY_VERSION "1.0.0"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "OV2640.h"
#include <ESP32httpUpdate.h>

// =======================================================
// ================= CAMERA GPIO CONFIG ==================
// =======================================================

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// =======================================================
// ================= EEPROM CONFIG =======================
// =======================================================

#define EEPROM_SIZE   128
#define EEPROM_MAGIC  0xA5A5A5A5

struct WiFiConfig {
    uint32_t magic;
    char ssid[33];
    char pass[65];
};

// =======================================================
// ================= WIFI STATE ==========================
// =======================================================

enum WiFiState {
    WIFI_IDLE = 0,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_AP_MODE,
    WIFI_DISCONNECTED
};

struct WiFiInfo {
    WiFiState status;
    String ssid;
    String ip;
    String gateway;
    String subnet;
    String ap_ip;
    int rssi;
};

enum SensorType
{
    SENSOR_FLOAT,
    SENSOR_INT,
    SENSOR_BOOL,
    SENSOR_STRING
};

typedef float (*FloatSensorCallback)();
typedef int (*IntSensorCallback)();
typedef bool (*BoolSensorCallback)();
typedef String (*StringSensorCallback)();
typedef void (*ActionCallback)();

typedef void (*ReceiveCallback)(JsonObject data);

typedef void (*SwitchCallback)(
    const char* key,
    const String value
);

typedef void (*ButtonCallback)(
    const char* key
);



// =======================================================
// ================= MAIN CLASS ==========================
// =======================================================

class PanzerRobotics {

public:

    constexpr static uint8_t MAX_SENSOR = 32;
    constexpr static uint8_t MAX_OUTPUT = 32;
    constexpr static uint8_t MAX_ACTION = 32;

    static const char HEADER[];
    static const char BOUNDARY[];
    static const char CTNTTYPE[];
    static const int hdrLen;
    static const int bdrLen;
    static const int cntLen;

    PanzerRobotics();
    ~PanzerRobotics() = default;   

    // CORE
    void begin();
    void begin(HardwareSerial &serial);
    void update();

    void enableDebug(bool enable = true);

    // DEVICE
    void setDeviceName(const char* name);
    void setFirmware(const char* version);
    void setAuthor(const char* author);
    void setModel(const char* model);
    void setSerialNumber(const char* serialNumber);

    // DASHBOARD
    void beginDashboard();
    void setSendInterval(uint32_t interval);

    // REST
    String send(const char* serverUrl, StaticJsonDocument<200>& jsonDoc);
    String get(const char* serverUrl);
    String update(const char* serverUrl, StaticJsonDocument<200>& jsonDoc);
    String getById(const char* serverUrl, const char* id);

    // OTA
    bool checkUpdate(const char* updateUrl, double currentVersion);
    void updateFirmware(const char* updateUrl);

    // Camera
    String esp32cam();
    void handleClient();
    void handleJPGStream();
    void handleJPG();

    // WiFi
    void wifiBegin(const char* apSsid, const char* apPass);
    WiFiInfo getWiFiInfo();
    bool isWiFiConnected();

    // SENSOR & ACTION
    void addSensor(const char* key, FloatSensorCallback callback);
    void addSensor(const char* key, IntSensorCallback callback);
    void addSensor(const char* key, BoolSensorCallback callback);
    void addSensor(const char* key, StringSensorCallback callback);
    void removeSensor(const char* key);

    // OUTPUT
    void addSwitch(const char* key, uint8_t pin);
    void writeSwitch(const char* key, const String& value);
    String getButton(const char* key);

    // ACTION
    void addAction(const char* key, ActionCallback callback);

    // Alias Dashboard
    inline void addButton(const char* key, ActionCallback callback)
    {
        addAction(key, callback);
    }

    // LOGGER
    void info(const String& message);
    void warning(const String& message);
    void error(const String& message);

    void onReceive(
        ReceiveCallback callback
    );

    void onSwitch(
        SwitchCallback callback
    );

    void onButton(
        ButtonCallback callback
    );

    // ==========================
    // RASPBERRY PI DATA
    // ==========================

    String getDataRaspi(const char* key);
    bool hasDataRaspi(const char* key);
    String getRaspiData();
    void clearRaspiData();

private:
    struct SensorItem
    {
        const char* key;
        SensorType type;

        union
        {
            FloatSensorCallback floatCallback;
            IntSensorCallback intCallback;
            BoolSensorCallback boolCallback;
            StringSensorCallback stringCallback;
        };
    };

    struct OutputItem
    {
        const char* key;
        uint8_t pin;
        bool state = false;
        String value = "";
    };

    struct ActionItem
    {
        const char* key;
        ActionCallback callback;
    };

    SensorItem sensors[MAX_SENSOR];

    OutputItem outputs[MAX_OUTPUT];

    ActionItem actions[MAX_ACTION];

    uint8_t sensorCount = 0;

    uint8_t outputCount = 0;

    uint8_t actionCount = 0;

    const char* deviceName = "ESP32";

    const char* firmwareVersion = "1.0.0";

    const char* author = "";

    const char* model = "";

    const char* serialNumber = "";

    bool dashboardEnabled = false;

    bool debugEnabled = false;

    uint32_t sendInterval = 1000;

    uint32_t lastSend = 0;

    HardwareSerial* serialPort = &Serial;

    // ===================================
    // RASPBERRY PI DATA
    // ===================================

    StaticJsonDocument<1024> raspiData;
    bool raspiDataAvailable = false;

    WebServer server;
    OV2640 cam;

    WiFiState wiFiState;
    unsigned long wifiStartTime;
    WiFiConfig wifiConfig;

    bool connectWiFiFromEEPROM();
    void startAPMode(const char* apSsid, const char* apPass);

    void setupWeb();
    String webPage();
    void handleStatus();
    void disconnectWiFi();
    void handleNotFound();

    void sendDeviceInfo();

    void sendSensorPacket();

    void receivePacket();

    void processControl(JsonObject data);

    void processAction(const String& action);

    ReceiveCallback receiveCallback = nullptr;

    SwitchCallback switchCallback = nullptr;

    ButtonCallback buttonCallback = nullptr;
};

#endif