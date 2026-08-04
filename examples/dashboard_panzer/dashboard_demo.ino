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

#include <PanzerRobotics.h>

PanzerRobotics Panzer;

// ======================================================
// Pin Configuration
// ======================================================

#define RELAY1 2
#define RELAY2 4

// ======================================================
// Sensor Callbacks
// ======================================================

float readTemperature()
{
    return random(200, 380) / 10.0;
}

float readHumidity()
{
    return random(450, 900) / 10.0;
}

// ======================================================
// Button Callbacks
// ======================================================

void restartESP()
{
    Serial.println("Restart Button Pressed");

    // ESP.restart();
}

// ======================================================
// Receive Callbacks
// ======================================================

void onReceive(JsonObject data)
{
    Serial.println("Packet Received");

    serializeJsonPretty(data, Serial);
    Serial.println();
}

void onSwitch(const char* key, bool value)
{
    Serial.print("Switch : ");
    Serial.print(key);
    Serial.print(" = ");
    Serial.println(value ? "ON" : "OFF");
}

void onButton(const char* key)
{
    Serial.print("Button : ");
    Serial.println(key);
}

void setup()
{
    Panzer.begin();

    Panzer.enableDebug();

    Panzer.beginDashboard();

    Panzer.setSendInterval(1000);

    // ================= Register Sensors =================

    Panzer.addSensor("temperature", readTemperature);
    Panzer.addSensor("humidity", readHumidity);

    // ================= Register Switches =================

    Panzer.addSwitch("relay1", RELAY1);
    Panzer.addSwitch("relay2", RELAY2);

    // ================= Register Buttons =================

    Panzer.addButton("restart", restartESP);

    // ================= Register Events =================

    Panzer.onReceive(onReceive);

    Panzer.onSwitch(onSwitch);

    Panzer.onButton(onButton);
}

void loop()
{
    Panzer.update();
}