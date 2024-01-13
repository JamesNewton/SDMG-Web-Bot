# SDMG-Web-Bot

This is an updated version of 
https://github.com/JamesNewton/BattleBot-Control/tree/patch-2
which started out as
https://github.com/jmalins/BattleBot-Control

This new version is updated in a few "interesting" ways:
1. It supports recording and playback of your manual control. 
Press the red button, move the controls, click the recordin off, then under the gear, use one of the Save buttons and then the Animate buttons to play it back.
You can also edit the recording in the text box on Settings panel. The format is <tt>[_interval_, [*delay*, *left*:*right*:*weapon*]]</tt>.
The inner ,[] section can repeat as often as you like. They are saved in the file system as /1.JSON, /2.JSON, etc... and can also be edited, downloaded or uploaded in the file editor. If you create a /0.JSON file, it will be played back automatically on startup.
<br>![Control Page](https://user-images.githubusercontent.com/419392/136255712-a1a32200-db01-432b-a2fa-a7d1682e08b1.png)
<br>![Control Settings page](https://user-images.githubusercontent.com/419392/136254679-caab4dbd-3fd5-4eb1-a2a9-d1af3abfd033.png)

2. All the other advances from the prior repo, the onboard editor, the ping and GPIO sensors, etc... are still in there.
https://github.com/JamesNewton/BattleBot-Control/tree/patch-2#development-setup

3. It compiles for both esp-8266 AND ESP32 under <a href="https://platformio.org/install/ide?install=vscode">PlatformIO</a>. 
And under Arduino IDE without some effort; see the <a href="https://github.com/JamesNewton/SDMG-Web-Bot/tree/ArduinoIDEv1">ArduinoIDEv1</A> branch. 
The key is that the ESP32 has far more processing ower and will (eventually) support https web service which may allow access to the sensor 
(including the camera) on the cell phone. <a href="https://hackaday.io/project/184720-web-smart-phone-screen-blink-bot">Another method of getting access
to phone sensors is in the works</a>.

### Web programming

You can program this with just Chrome! No IDE, no programs, no nothing. Just 
1. download the [release files](https://github.com/JamesNewton/SDMG-Web-Bot/releases) in the "esptool.zip" archive and extract them locally.
2. open the manifest.json file and look at the "parts" section. 
3. go to:
https://espressif.github.io/esptool-js/
and check the ports, then plug in the robot to the PC. It takes a USB A to micro A cable; and that needs to be a "data" type cable, not charge only version. When connected, the servos will spin and that can be a problem, because the USB port won't supply enough power. So disconnect the servos from the board temporarily.
When you plug in, you should see a new port show up. Select that port and connect. Don't change the baud rate. Verify it detects the chip, and loads the stub. 
4. Copy in the offset for the first file (it takes decimal) listed in the manifest, and select the matching file. 
5. for each following file, press Add file, copy in the offset and select the file. 
6. Hit program, hold breath. 
7. When complete Reset or power cycle. 
8. Smile. (or cry and contact me for help)

### Development

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


## Additional features:
- The USB connector prints debug data. Connect via e.g. PuTTY or screen to the com port at 115,200 baud, No parity, 8 data bits, 1 stop bit. 
- It will also accept commands to play back animation .JSON files. e.g. <tt>/1</tt> will cause /1.JSON to play back. 
- The onboard edior is available by adding `/edit` to the URL. 
  <BR>![editor](https://user-images.githubusercontent.com/419392/135738555-d4a901bc-d6ba-4154-9623-ee87019079da.png)

  - To create a file, enter the file name (always start with "/") then press Create.
  - To upload a file press Upload, select the file locally, and it will be added to the robot. 
  - To edit a file, select it on the left, then edit on the right, and be sure to click Save
  - To rename or copy a file, create the new file, then slect the old file, select it's contents, copy them, and then select the new file and paste the content into it and save. 
  - To delete a file, right click it and select Delete.
- Can connect to local WiFi router by adding a file (can create in onboard editor) called wifi.txt with the routers wifi SSID (name) and then a ":" and the password. e.g. <tt>mywifi:mypasssword</tt>. On restart, it wll connect to local wii and take the DHCP assigned wifi address. You can find that address by monitoring the serial debug data, from the router, or using an app like advanced-ip-scanner (Windows) or fing (iOS, Android) 
- The `loop` function in the /robot.js file can be used to add code which will be called regularly when the robot is being controlled. This is a great place to add code to read sensors, filter input, or other interesting code. 
  
 ## Data Flow
  ![dataflow](https://user-images.githubusercontent.com/419392/135737811-5097d13e-1ff1-4e0d-95b7-87b00e22b9c2.png)
  
  
## See Also
- Connecting servos to the NodeMCU's and connecting external battery power is much easier via this little <a href="https://hackaday.io/project/181972-massmind-rc-servo-adapter">RC Servo adapter PCB</a>
<br>![RC Servo Adapter, NodeMCU, Breadboard, Servo](https://user-images.githubusercontent.com/419392/136256607-2089903e-0541-40fc-8341-240238bef7ae.png)
- <a href="https://docs.google.com/presentation/d/1CeP5DSfR2xFZyUt2vdPWOcWJbTkmyCsVTNZQbRQhnUI/edit?usp=sharing">Steampunk Presentation on using the bot as an animation platform</a> at <a href="http://gaslightgathering.org/">Gaslight Gathering 2021</a>
- <a href="https://github.com/JamesNewton/AdvancedRoboticsWithJavascript/wiki#week-4-day-1-the-bot">Edcuational Course using the SDMG Bot</a>
- Class on <a href="https://jamesnewton.github.io/class/presentESP8266blink.html#1">Arduino (esp)</a>, <a href="https://jamesnewton.github.io/class/presentESP8266servo.html#1">Servos</a>, and <a href="https://github.com/JamesNewton/BattleBot-Control/tree/patch-2#battlebot-control-servo">the Bot</a> at SDCoLab 2019
- <a href="https://goo.gl/photos/vmKWgHmUPouctquz8">Images / Videos of the "Cardboard Combat" origins</a> with <a href="https://www.sdmakersguild.org/events/cardboard-combat-bots/">San Diego Makers Guild</a>
- <a href="https://www.sdmakersguild.org/steambot-build-day-at-gaslight-gathering-april-13-2019/">Steampunk version</a> built at <a href="http://gaslightgathering.org/">Gaslight Gathering 2019</a> (also 2021)
- https://www.youtube.com/watch?v=SbmjmV-a6yE demo of the ping sensor working
- https://youtu.be/kWHx6a_4-iI?t=1523 Presentation about the bot for the Seattle Robotics Society

    
