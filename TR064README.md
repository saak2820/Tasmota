# ! i'm just playing around. nothing serious



![Tasmota logo](/tools/logo/TASMOTA_FullLogo_Vector.svg)

Alternative firmware for [ESP8266](https://en.wikipedia.org/wiki/ESP8266) based devices with **easy configuration using webUI, OTA updates, automation using timers or rules, expandability and entirely local control over MQTT, HTTP, Serial or KNX**.
_Written for PlatformIO with limited support for Arduino IDE._

[![GitHub version](https://img.shields.io/github/release/arendst/Tasmota.svg)](https://github.com/arendst/Tasmota/releases/latest)
[![GitHub download](https://img.shields.io/github/downloads/arendst/Tasmota/total.svg)](https://github.com/arendst/Tasmota/releases/latest)
[![License](https://img.shields.io/github/license/arendst/Tasmota.svg)](LICENSE.txt)
[![Chat](https://img.shields.io/discord/479389167382691863.svg)](https://discord.gg/Ks2Kzd4)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/arendst/Tasmota)




## Using TR-064 Protocol from https://github.com/Aypac

# Arduino-TR-064-SOAP-Library [![Build Status](https://travis-ci.com/Aypac/Arduino-TR-064-SOAP-Library.svg?branch=master)](https://travis-ci.com/Aypac/Arduino-TR-064-SOAP-Library)
Arduino library to facilitate the use of the TR-064 protocol ([Aypacs "simple" explanation](https://github.com/Aypac/Arduino-TR-064-SOAP-Library/wiki/How-does-the-TR-064-protocol-work%3F), the [definition](https://www.broadband-forum.org/technical/download/TR-064.pdf) and an official [description](https://avm.de/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf)), most commonly used by Fritz!Box.

## Made Configuration :

![Tasmota logo](/tools/screenshots/FritzMenu.PNG)

## Using Scriptor to send the TR-064 Commands:

https://tasmota.github.io/docs/Scripting-Language/

```

>D 33
sv="X_VoIP:1"

>B
=>sendfritzbox[%sv%,"X_AVM-DE_DialNumber",{{"NewX_AVM-DE_PhoneNumber","**9"}}]
=>Publish esp-klingel/klingel Ring Ring
delay((8000)
=>sendfritzbox[%sv%,"X_AVM-DE_DialHangup"]

```

works so far...
