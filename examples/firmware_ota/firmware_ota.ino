// Copyright 2025 ariefsetyonugroho
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Example Firmware OTA using Panzer Robotics Library       +
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1. Create Github Page to host firmware.json and firmware binary
// 2. Value firmware.json example:
/*
{
  "version": 1.1,
  "url": "https://asnproject.github.io/firmware/firmware_v1.1.bin",
}
*/
// 3. Update FIRMWARE_VERSION and UPDATE_URL accordingly

#include <WiFi.h>
#include <PanzerRobotics.h>

// Wi-Fi Credentials
const char* ssid = "your-ssid"; // Change this with your SSID
const char* password = "your-xxxxxx"; // Change this with your password SSID

PanzerRobotics panzer;

#define FIRMWARE_VERSION 1.0 // Always Update 
#define UPDATE_URL "https://asnproject.github.io/firmware/firmware.json" // URL to check for updates

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung");
  
  // OTA Check
  if (panzer.checkUpdate(UPDATE_URL, FIRMWARE_VERSION)) {
    panzer.updateFirmware(UPDATE_URL);
  } else {
    Serial.println("No firmware update available.");
  }
}

void loop() {

}
