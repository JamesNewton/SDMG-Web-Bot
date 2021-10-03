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
- Can connect to local WiFi router by adding a file (can create in onboard editor) called wifi.txt with the routers wifi SSID (name) and then a ":" and the password. e.g. <tt>mywifi:mypasssword</tt>. On restart, it wll connect to local wii and take the DHCP assigned wifi address. You can find that address by monitoring the serial debug data.
- The `loop` function in the /robot.js file can be used to add code which will be called regularly when the robot is being controlled. This is a great place to add code to read sensors, filter input, or other interesting code. 
  
 ## Data Flow
  ![dataflow](https://user-images.githubusercontent.com/419392/135737811-5097d13e-1ff1-4e0d-95b7-87b00e22b9c2.png)
  
  
See also:
- Connecting servos to the NodeMCU's and connecting external battery power is much easier via this little <a href="https://oshpark.com/shared_projects/FvZB8gX9">RC Servo adapter PCB</a>
- <a href="https://docs.google.com/presentation/d/1CeP5DSfR2xFZyUt2vdPWOcWJbTkmyCsVTNZQbRQhnUI/edit?usp=sharing">Steampunk Presentation on using the bot as an animation platform</a>
