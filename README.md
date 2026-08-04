<!--
 Copyright 2026 ariefsetyonugroho
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     https://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-->

## Panzer Robotics Library 

Panzer Robotics Library: A comprehensive library designed specifically for the ESP32 microcontroller, offering a wide range of functions to streamline and simplify the development process. This library enhances productivity by providing pre-built solutions for common tasks, enabling developers to focus on higher-level design and functionality. With an emphasis on encapsulation, it ensures well-organized and maintainable code. Its reusability allows developers to easily implement the library in multiple projects without the need for code duplication, while its scalability ensures that the library can be effortlessly extended to meet the evolving needs of any project.. 

### Feature:
- [x] ESP32 Rest API

    - [x] POST data to api
    - [x] GET data from api
    - [x] UPDATE data to api (for api id on body json)
    - [x] GET data from api by id
- [x] Stream ESP32CAM
- [x] Update Firmware with OTA (Github Version)
- [x] WiFi Manager
- [x] Dashboard Communication

    - [x] Send sensor data to Raspberry Pi
    - [x] Receive control data from Raspberry Pi
    - [x] Automatic JSON packet generation
    - [x] Configurable send interval
    - [x] Debug communication

- [x] Sensor Manager

    - [x] Register sensor callback
    - [x] Remove sensor
    - [x] Automatic sensor reading
    - [x] Dynamic sensor registration (up to 32 sensors)

- [x] Switch Manager

    - [x] Register switch output
    - [x] Automatic GPIO control from Dashboard
    - [x] Read latest switch value
    - [x] Dynamic switch registration (up to 32 outputs)

- [x] Button Manager

    - [x] Register button action
    - [x] Execute action from Dashboard
    - [x] Read latest button value
    - [x] Dynamic button registration (up to 32 actions)

- [x] Callback Event

    - [x] onReceive()
    - [x] onSwitch()
    - [x] onButton()

- [x] Device Information

    - [x] Device Name
    - [x] Firmware Version
    - [x] Author
    - [x] Model
    - [x] Serial Number

- [x] Logger

    - [x] Info
    - [x] Warning
    - [x] Error

### Use this library
- Download this project 
- Extract project
- Copy folder to Arduino -> Libraries

### Compatibility

| Board | Status |
|:------|:------:|
| ESP32 Dev Module | ✅ |
| ESP32-WROOM | ✅ |
| ESP32-WROVER | ✅ |
| ESP32-CAM | ✅ |
| ESP32-S2 | ✅ |
| ESP32-S3 | ✅ |
| ESP8266 | ❌ |


### Dashboard Example

```cpp
#include <PanzerRobotics.h>

PanzerRobotics Panzer;

void setup()
{
    Panzer.begin();

    Panzer.beginDashboard();

    Panzer.addSensor("temperature", readTemperature);

    Panzer.addSwitch("relay1", RELAY1);

    Panzer.addButton("restart", restartESP);
}

void loop()
{
    Panzer.update();
}
```

### Installation Arduino Library Needed
- HTTPClient
- ArduinoJson
- WebServer
- WiFiClient
- ESP32httpUpdate

### Communication Protocol

#### ESP32 → Raspberry Pi

```json
{
    "type": "sensor",
    "device": "ESP32",
    "data": {
        "temperature": 28.4,
        "humidity": 75.2
    }
}
```

#### Raspberry Pi → ESP32

```json
{
    "type": "control",
    "key": "relay1",
    "value": "true"
}
```

<br />
Enjoy!!!
<br />
<br />
<br />
<br />

###### Author & Licence by

author: ASNProject<br />
email: asnproject02@gmail.com
website: panzerrobotics.com
