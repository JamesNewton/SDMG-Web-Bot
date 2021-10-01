# SDMG-Web-Bot

This is an updated version of 
https://github.com/JamesNewton/BattleBot-Control/tree/patch-2
which started out as
https://github.com/jmalins/BattleBot-Control

This new version is hacked in a few "interesting" ways:
1. It compiles for both esp-8266 AND ESP32 under <a href="https://platformio.org/install/ide?install=vscode">PlatformIO</a>. 
(Probably won't work under Arduino IDE without some effort). 
The key is that the ESP32 has far more processing ower and will (eventually) support https web service which will allow access to the sensor 
(including the camera) on the cell phone.

You will need to install:
- Platforms: "Espressif 8266" and "Espressif 32"
- Boards: "nodemcuv2" for the NodeMCU (8266) and "node32s" for the DevKitC 
<BR>Tested with:
<BR><a href="https://www.amazon.com/gp/product/B081CSJV2V">"HiLetgo ESP8266 NodeMCU CP2102 ESP-12E"</a>
<BR><a href="https://www.amazon.com/gp/product/B0718T232Z">"HiLetgo ESP-WROOM-32 ESP32 ESP-32S Development Board"</a>
- Libraries:
<BR>-	teckel12/NewPing@^1.9.1
<br>-	me-no-dev/ESP Async WebServer @ ^1.2.3
<br>- lorol/LittleFS_esp32 @ ^1.0.6
<br>- madhephaestus/ESP32Servo @ ^0.9.0 
(note <a href="https://github.com/madhephaestus/ESP32Servo/issues/23">issue with configuration corruption</a>)

2. It supports recording and playback of your manual control. 
Press the red button, move the controls, click the recordin off, then under the gear, use one of the Save buttons and then the Animate buttons to play it back.
You can also edit the recording in the text box on Settings panel. The format is <tt>[_interval_, [*delay*, *left*:*right*:*weapon*]]</tt>.
The inner ,[] section can repeat as often as you like. 
3. All the other advances from the prior repo, the onboard editor, the ping and GPIO sensors, etc... are still in there. 

See also:
- <a href="https://docs.google.com/presentation/d/1CeP5DSfR2xFZyUt2vdPWOcWJbTkmyCsVTNZQbRQhnUI/edit?usp=sharing">Steampunk Presentation on using the bot as an animation platform</a>
