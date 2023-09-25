# SDMG-Web-Bot

Arduino IDE branch

This is an updated version of 
<BR> https://github.com/JamesNewton/BattleBot-Control/tree/patch-2
which started out as
<BR> https://github.com/jmalins/BattleBot-Control

This new version is updated in a few "interesting" ways:
1. It supports recording and playback of your manual control. 
Press the red button, move the controls, click the recordin off, then under the gear, use one of the Save buttons and then the Animate buttons to play it back.
You can also edit the recording in the text box on Settings panel. The format is <tt>[_interval_, [*delay*, *left*:*right*:*weapon*]]</tt>.
The inner ,[] section can repeat as often as you like. They are saved in the file system as /1.JSON, /2.JSON, etc... and can also be edited, downloaded or uploaded in the file editor. If you create a /0.JSON file, it will be played back automatically on startup.
<br>![Control Page](https://user-images.githubusercontent.com/419392/136255712-a1a32200-db01-432b-a2fa-a7d1682e08b1.png)
<br>![Control Settings page](https://user-images.githubusercontent.com/419392/136254679-caab4dbd-3fd5-4eb1-a2a9-d1af3abfd033.png)

2. All the other advances from the prior repo, the onboard editor, the ping and GPIO sensors, etc... are still in there.
https://github.com/JamesNewton/BattleBot-Control/tree/patch-2#development-setup

3. It compiles for the ESP8266 under the Arduino IDE as setup on this branch. See the main branch for both esp-8266 AND ESP32 under <a href="https://platformio.org/install/ide?install=vscode">PlatformIO</a>. 

## Development

### Setup the Arduino IDE (version 1 NOT version 2)

To work on this with the Arduno IDE we can not install the 2.x version. Sadly, the latest version can't load the web files (yet) according to the Arduino Team:
<BR> https://forum.arduino.cc/t/ide-2-1-and-spiffs/1129840/4

Also, do not install the Arduino IDE from an App center or Software center as those versions will not work correctly. 

[ ] Go to the Downloads page on the Arduino website: https://www.arduino.cc/en/software#legacy-ide-18x. Scroll down to "Legacy IDE (1.8.X)".

[ ] Click on the version for the Operating System (OS) on your computer from the blue-green "DOWNLOAD OPTIONS" box to the right of the "Legacy IDE (1.8.X)" heading.
 
[ ] Install the program by following the instructions specific to your OS version at
<BR> https://docs.arduino.cc/software/ide-v1. If that doesn't work, share the exact error message in the week's forum.
On a Mac, double-click the zip file in the Finder. This will unzip to one file named “Arduino.app”. Drag the file to your Applications Folder to install it. 
On Windows, download and run the Windows.exe, or the Windows App installer, depending on your OS version. 

For more detailed instructions, view the article at 
<BR> https://docs.arduino.cc/software/ide-v1. 

[ ] Open the Arduino IDE program. Locate the Arduino IDE. 

[ ] In the Arduino IDE, open Preferences. Go to File > Preferences (Windows) or Arduino > Settings (macOS).

[ ] Make note of the "Sketchbook location" in the Arduino IDE File > Preferences dialog. This is the folder where your programs will go, especially the new program for the bot. It's good to write that location down or open it in your file manager by pressing "Browse".

[ ] In the section called "Additional Boards Manager URLs" add:
<BR> http://arduino.esp8266.com/stable/package_esp8266com_index.json
<BR> by copying and pasting it from this page. 

[ ] Click OK to close the Preferences dialog.

[ ] Select Tools > Board > Boards Manager.

[ ] In the Boards Manager dialog, replace the prompt "Filter your search…" with "ESP8266" (no need to press enter–just wait) to find the latest version (3.1.2).

If you don't see that, go back and check that the URL is still installed on step 6, and that it is exactly copied.

[ ] Install the "esp8266" option (Tools > Boards > Boards Manager) by clicking on the "INSTALL" button in the lower right corner of the esp8266 entry. Note that the button won't show up until you move the mouse over the entry.

The progress bar along the bottom will show you when the install is finished

[ ] Close the Boards Manager dialog with the Close button in the lower right. 

[ ] Verify that the install worked by navigating to the menu Tools > Board menu and look for a submenu named "ESP8266".

NOTE: This is not under Tools > Board > Boards Manager. It will be a new option in the list of boards under Tools > Board.

If you don't see it there, re-check the steps related to the Boards Manager.

[ ] Select Tools > Board > ESP 8266 Boards (3.1.2) > Node MCU v1.0 (ESP-12E module)

Check by pulling down the Tools menu again; you should see many new options, including CPU Frequency and FLASH size, and Board should show "Node MCU v1.0 (ESP-12E module)".

[ ] Download this file:
<BR> https://github.com/jshaw/NewPingESP8266/archive/refs/heads/master.zip

[ ] Under Sketch > Include Library > Add .zip library, go to the Downloads folder and select the master.zip file you just downloaded. 

Once that finishes, you can check under Sketch > Include Library and then scroll down to ensure "NewPingESP8266-master" shows up.

[ ] Go to Tools > Manage Libraries and wait for it to update. In the box to the right of Topic, type in “ESPAsyncWebSrv” to filter the list of libraries. Wait for it to display the filtered list. Click Install to install the matching entry. 

If the program prompts you to install other dependencies, select Install all. If not, repeat step 17 for each of these libraries:
- ESPAsyncTCP
- AsyncTCP

Note that another library matches the filter, but it has a different name. The important part is that the entry with the matching name says "INSTALLED" in green.

### Load the SDMG Web Bot program

[ ] Download this file:
<BR> https://github.com/JamesNewton/SDMG-Web-Bot/archive/refs/heads/ArduinoIDEv1.zip 
Extract it under that sketch folder you found earlier in the File > Preferences (Windows) or Arduino > Settings (macOS) dialog.

Make sure it ends up looking like this under the Arduino sketch folder:

`…Arduino/SDMG-Web-Bot/SDMG-Web-Bot.ino`

[ ] Restart the Arduino IDE. e.g. Close the program, and then open it up again. The installation of the LittleFS into the tools folder should work automatically as a result.
 
[ ] Check that the FS Upload add-on is working by pulling down the Tools menu and checking that "ESP8266 LittleFS Data Upload" is available. If it's not, be sure you restarted the program, and if you did, continue on to install the ESP8266FS file system upload add-on. Directions:
<BR> https://github.com/earlephilhower/arduino-esp8266littlefs-plugin#installation 

Be sure to close, and then restart the IDE (as per the installation docs). 
 
[ ] And now, with a bit of luck, you can go to File > Open, browse to the sketchbook folder, and then to the bot folder under that, and open the .ino file. You should now see the program in the editor window. 

[ ] Select Sketch > Verify/Compile and it might compile. If not, post any error messages you get in this week’s forum.

[ ] Next we need to connect the robot to the computer, using a USB data cable (not a charging cable; if you have multiple options, use the thickest one). If the device doesn't show up in the computer, try a different cable. 

NOTE: macOS users must install serial port drivers. Follow the provided instructions. The NodeMCU typically has a CP210x USB to serial chip, but to save money the manufacturer might have used a CH340 or CH341 chip. Windows will generally automatically find the driver. But if it doesn't, identify the chip on your board by looking at the markings on the chip and install the driver for it. The chip should be just in front of the USB port. Here are links to the drivers from the Manufacturers: 

- CP2102, CP2104, CP2110, etc.<BR> https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads 

- CH340, CH341, etc.<BR> https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all

[ ] Once it's connected, in the Arduino IDE, select Tools > Port and choose the port that appears when you connect the board. It can help to unplug the bot, check the list, then plug the bot back in and re-check the list.

[ ] Select Sketch > Upload and wait for a while. The progress bar will go along the right side between the editor and the message output panel. 

If you get an error message about the port, check that the correct port is selected under Tools > Port. You may want to unplug and reconnect the board to see which port disappears and reappears. 
 
[ ] Select Tools > ESP8266 LittleFS Data Upload and wait for the programming of the data to finish. The progress bar will go along the right side between the editor and the message output panel. 

Once that's finished, you can try to connect to the bot as usual and you should find some new features on the main drive interface page. 

### Known Issues (and a few solutions)

On older operating systems, (e.g. Ubuntu 18, Windows 8, etc.) you might get an error when compiling like:
```
/home/jamesn/snap/arduino/85/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/tools/mkbuildoptglobals.py
Minimal supported version of Python is 3.7
```
This is because the IDE was installed from the "snap" based Ubuntu Application store, or via the snap installer in Windows. Remove it and re-install via the executable linked above directly from Arduino.cc. 

Some other issues related to the Arduino IDE in Ubuntu: <BR> https://goldayan.in/blog/iot_starter/ 

## Platform IO

Refer to the main branch

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
  
  
See also:
- Connecting servos to the NodeMCU's and connecting external battery power is much easier via this little <a href="https://hackaday.io/project/181972-massmind-rc-servo-adapter">RC Servo adapter PCB</a>
<br>![RC Servo Adapter, NodeMCU, Breadboard, Servo](https://user-images.githubusercontent.com/419392/136256607-2089903e-0541-40fc-8341-240238bef7ae.png)
- <a href="https://docs.google.com/presentation/d/1CeP5DSfR2xFZyUt2vdPWOcWJbTkmyCsVTNZQbRQhnUI/edit?usp=sharing">Steampunk Presentation on using the bot as an animation platform</a> at <a href="http://gaslightgathering.org/">Gaslight Gathering 2021</a>
- <a href="https://github.com/JamesNewton/AdvancedRoboticsWithJavascript/wiki#week-4-day-1-the-bot">Edcuational Course using the SDMG Bot</a>
- Class on <a href="https://jamesnewton.github.io/class/presentESP8266blink.html#1">Arduino (esp)</a>, <a href="https://jamesnewton.github.io/class/presentESP8266servo.html#1">Servos</a>, and <a href="https://github.com/JamesNewton/BattleBot-Control/tree/patch-2#battlebot-control-servo">the Bot</a> at SDCoLab 2019
- <a href="https://goo.gl/photos/vmKWgHmUPouctquz8">Images / Videos of the "Cardboard Combat" origins</a> with <a href="https://www.sdmakersguild.org/events/cardboard-combat-bots/">San Diego Makers Guild</a>
- <a href="https://www.sdmakersguild.org/steambot-build-day-at-gaslight-gathering-april-13-2019/">Steampunk version</a> built at <a href="http://gaslightgathering.org/">Gaslight Gathering 2019</a> (also 2021)
- https://www.youtube.com/watch?v=SbmjmV-a6yE demo of the ping sensor working
- https://youtu.be/kWHx6a_4-iI?t=1523 Presentation about the bot for the Seattle Robotics Society

    
