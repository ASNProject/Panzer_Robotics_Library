// Copyright 2026 M S I
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

// Copyright 2026 M S I
//
// Dashboard Example
//

#include <PanzerRobotics.h>

PanzerRobotics Panzer;

#define RELAY1 2
#define RELAY2 4

// ======================================================
// SENSOR
// ======================================================

float readTemperature()
{
    return random(200, 380) / 10.0;
}

float readHumidity()
{
    return random(450, 900) / 10.0;
}

float readGas()
{
    return random(100, 500);
}

// ======================================================
// BUTTON ACTION
// ======================================================

void restartESP()
{
    Serial.println("Restart Button Pressed");
}

void captureImage()
{
    Serial.println("Capture Button Pressed");
}

void setup()
{
    Serial.begin(115200);

    // ==============================================
    // UART1
    // RX = GPIO27
    // TX = GPIO25
    // ==============================================

    Serial1.begin(
        115200,
        SERIAL_8N1,
        25,     // RX
        27      // TX
    );

    // ==============================================
    // PANZER ROBOTICS
    // ==============================================

    Panzer.begin(
        Serial1
    );


    randomSeed(micros());

    Panzer.enableDebug();

    Panzer.beginDashboard();

    Panzer.setSendInterval(1000);

    // ==========================================
    // Device Information
    // ==========================================

    Panzer.setDeviceName("ESP32 Dashboard");

    Panzer.setFirmware("1.0.0");

    Panzer.setAuthor("ASNProject");

    Panzer.setModel("ESP32-WROOM");

    Panzer.setSerialNumber("ESP32-0001");

    // ==========================================
    // Register Sensors
    // ==========================================

    Panzer.addSensor("temperature", readTemperature);
    Panzer.addSensor("humidity", readHumidity);
    Panzer.addSensor("gas", readGas);

    // ==========================================
    // Register Switch
    // ==========================================

    Panzer.addSwitch("relay1", RELAY1);
    Panzer.addSwitch("relay2", RELAY2);

    // ==========================================
    // Register Button
    // ==========================================

    Panzer.addButton("restart", restartESP);
    Panzer.addButton("capture", captureImage);

    // ==========================================
    // Receive Every Packet
    // ==========================================

    Panzer.onReceive([](JsonObject data)
    {
        Serial.println("========== RECEIVE ==========");

        serializeJsonPretty(data, Serial);

        Serial.println();
    });

    // ==========================================
    // Switch Changed
    // ==========================================

    Panzer.onSwitch([](const char* key, String value)
    {
        Serial.print("Switch : ");
        Serial.print(key);

        Serial.print(" = ");

        Serial.println(value);
    });

    // ==========================================
    // Button Pressed
    // ==========================================

    Panzer.onButton([](const char* key)
    {
        Serial.print("Button : ");

        Serial.println(key);
    });
}

void loop()
{
    Panzer.update();

    // ==========================================
    // Read Current Dashboard Value
    // ==========================================

    String relay1 = Panzer.getButton("relay1");

    String relay2 = Panzer.getButton("relay2");

    Serial.print("Relay1 : ");
    Serial.println(relay1);

    Serial.print("Relay2 : ");
    Serial.println(relay2);

    // =========================================
    // GET DATA FROM RASPBERRY PI
    // =========================================

    String status = Panzer.getDataRaspi("status");

    if (status.length() > 0)
    {
        Serial.print("Raspberry Pi Status : ");
        Serial.println(status);
    }

}