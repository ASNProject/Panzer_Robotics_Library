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

#include <WiFi.h>
#include <PanzerRobotics.h>

// Wi-Fi Credentials
const char* ssid = "your-ssid"; // Change this with your SSID
const char* password = "your-xxxxxx"; // Change this with your password SSID

PanzerRobotics panzer;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA); // User WIFI STA
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung");

  String link = "Streaming link: " + panzer.esp32cam();
  Serial.println(link);
}

void loop() {
    panzer.handleClient();
}