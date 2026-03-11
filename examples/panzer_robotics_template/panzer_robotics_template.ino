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

PanzerRobotics panzer;

void setup() {
    Serial.begin(115200);

    panzer.wifiBegin("PANZER_SETUP", "1234567890");

    //=====================================================================================//
    //==Jika ingin mengambil informasi WiFi bisa gunakan getWiFiInfo seperti berikut ini:==//
    //=====================================================================================//
    delay(2000);
    WiFiInfo info = panzer.getWiFiInfo();

    Serial.println("===== WIFI INFO =====");
    Serial.println(info.status == WIFI_CONNECTED ? "CONNECTED" :
                   info.status == WIFI_AP_MODE ? "AP MODE" : "DISCONNECTED");

    Serial.println("SSID     : " + info.ssid);
    Serial.println("IP       : " + info.ip);
    Serial.println("Gateway  : " + info.gateway);
    Serial.println("Subnet   : " + info.subnet);
    Serial.println("RSSI     : " + String(info.rssi));
    Serial.println("AP IP    : " + info.ap_ip);

}

void loop() {
    panzer.handleClient();
}