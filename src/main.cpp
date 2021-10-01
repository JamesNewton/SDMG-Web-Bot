 /* 
  Cardboard BattleBot Control Firmware
  Copyright (c) 2016-17 Jeff Malins. All rights reserved.
  20180803 to support continuous rotation servos instead of DC motors while retaining the same user interface. 
  20190207 add ACE.js editor for upload / edit of files on the bot via browser, add playback of .json files
  20190504 add support for PING sensor and IO pins. 
*/

// Tools / Board / NodeMCU 1.0 (ESP-12E Module)
// Tools / Port / (whatever it shows up as)
// Tools / Flash Size / 4M (3M SPIFFS)

#include <Arduino.h> //platformio
#include <FS.h>
#ifdef ESP32
#include "ESP32DevKitC-Hardware.h"
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>
//#include <LITTLEFS.h>
//#define SPIFFS LITTLEFS
#elif defined(ESP8266) 
#include "NodeMCU-Hardware.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <Servo.h>
#include <LittleFS.h>
#define SPIFFS LittleFS
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <NewPing.h> //platformio
//#include <NewPingESP8266.h> //arduino IDE
#include <stdlib.h>

#include "animals.h"

/********************************************************************************
 * Network Configuration                                                        *
 ********************************************************************************/
 
#define HTTP_PORT       80
#define HOST_NAME       "sdmgbot"
#define AP_SSID_BASE    "robot-"
#define USE_ANIMAL      true

/********************************************************************************
 * Globals                                                                      *
 ********************************************************************************/
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). 
//Maximum sensor distance is rated at 400-500cm.
long lastping;

// set to true to print commands to the serial monitor for debugging //
#define PRINT_TO_SERIAL_MONITOR  false

//Trim servos
#define TRIM_ADJ 10
int leftTrim;
int rightTrim;

// states //
enum RobotState {
  STATE_START = 1,
  STATE_SETUP,
  STATE_CONNECT, //trying to connect to WiFi router
  STATE_IDLE, //stopped getting commands, don't move unless playback
  STATE_DRIVING,
  STATE_DRIVING_WITH_TIMEOUT
};

String playback;
File playfile;
int playdo;

// function prototypes //
void enterState(RobotState);
void runStateMachine(void);
void updateHardware(String);
bool getWiFiForceAPMode();
void setWheelPower(int,int);
void setWeaponPower(int);
void webSocketMessage(String);
void setStatusLED(bool);

#define DBG_OUTPUT_PORT Serial
#define DBG_BAUD_RATE   115200

/********************************************************************************
 * WiFi Setup                                                                   *
 *  Implement flexible WiFi setup. The default is create an access point for    *
 *  the controller device to connect to. This is inconvenient for development,  *
 *  however. If a file called `wifi.config` is present in the file system,      *
 *  then the controller will instead connect to an existing WiFi network.       *
 *                                                                              *
 *  The format of `wifi.config` is one line:                                    *
 *  SSID:password                                                               *
 ********************************************************************************/

#define WIFI_CONFIG_FILE "/wifi.txt"

#define NAME_CONFIG_FILE "/botname.txt"

// configure and connect to wifi //
void setupWiFi() {
  // check for config file //
  String ssid, password;
  if(SPIFFS.exists(WIFI_CONFIG_FILE)) {
    DBG_OUTPUT_PORT.println("WiFi configuration found");
 
    File file = SPIFFS.open(WIFI_CONFIG_FILE, "r");
    String contents = file.readString();
    int index = contents.indexOf(":");
    ssid     = contents.substring(0, index);
    password = contents.substring(index + 1);
    password.trim(); // file may have trailing whitespace //
    file.close();
  }

  bool stationMode = !!ssid.length() && !getWiFiForceAPMode();
  if(stationMode) {
    // connect to WiFi network //
    WiFiMode_t oldMode = WiFi.getMode();
    WiFi.mode(WIFI_STA);
    DBG_OUTPUT_PORT.printf("Connecting to \'%s\' as MAC:", ssid.c_str());
    byte mac[6];
    WiFi.macAddress(mac);
    for(int i = 0; i < 6; i++) { //element 5 is NOT first.
      DBG_OUTPUT_PORT.print( String(mac[i], HEX));
      DBG_OUTPUT_PORT.print(":");
      }
    DBG_OUTPUT_PORT.print("\n");
    if(oldMode != WIFI_STA || String(WiFi.SSID()) != ssid) {
      DBG_OUTPUT_PORT.println("Resetting connection");
      WiFi.begin(ssid.c_str(), password.c_str());
    }
    enterState(STATE_CONNECT);
    unsigned long connectTimeout = millis() + 10000; // 10 seconds //
    while (WiFi.status() != WL_CONNECTED) {
      yield(); // need to yield or we get a WDT reset //
      runStateMachine();
      if(millis() > connectTimeout) {
        DBG_OUTPUT_PORT.println("Connect timed out, falling back to AP mode");
        stationMode = false; // after timeout, fallback
        break;
      }
    }
    enterState(STATE_SETUP);
  }

  if(stationMode) {
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.print("Connected! IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.localIP());
  } else {
    // access point mode //
    byte mac[6];
    WiFi.softAPmacAddress(mac);
    ssid = AP_SSID_BASE;
    if(USE_ANIMAL) {
      if(SPIFFS.exists(NAME_CONFIG_FILE)) {
        DBG_OUTPUT_PORT.println("Name configuration found");
        File file = SPIFFS.open(NAME_CONFIG_FILE, "r");
        ssid = file.readString();
        file.close();
        }
      else {
        // lookup an animal from table based on MAC bytes //
        int animalNum = (mac[5] << 8 | mac[4]) % ANIMAL_COUNT;
        ssid += FPSTR(ANIMAL_TABLE[animalNum]);
        }
      } 
    else {
      // use MAC address in hex //
      for(int i = 5; i >= 0; i--) {
        ssid += String(mac[i], HEX);
       }
      }

    DBG_OUTPUT_PORT.println("Starting AP, SSID: " + ssid);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str());
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.print("Started! IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.softAPIP());
  }

  // start mDNS //
  MDNS.begin(HOST_NAME);
  MDNS.addService("http", "tcp", HTTP_PORT);
  
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(HOST_NAME);
  DBG_OUTPUT_PORT.println(".local/ to access user interface");
}

/********************************************************************************
 * REST Event Handler                                                           *
 *  Respond to REST interface from the client.                                  *
 ********************************************************************************/
 
void handleControlPut(AsyncWebServerRequest *request) {
  /*DBG_OUTPUT_PORT.println("START control put");
  int parms = request->params();
  for(int i = 0; i < parms; i++) {
    AsyncWebParameter* p = request->getParam(i);
    DBG_OUTPUT_PORT.println("  param: " + p->name() + " " + p->value());
  }
  DBG_OUTPUT_PORT.println("END control put");*/
  
  // should have a POST body //
  if(request->hasParam("body")) { //using PUT but getting param from URL (?)
    enterState(STATE_DRIVING_WITH_TIMEOUT);
    
    String body = request->getParam("body")->value();
    // DBG_OUTPUT_PORT.println("handleControlPut: " + body);
    switch(body.charAt(0)) {
    case '/': //playback a sequence
       DBG_OUTPUT_PORT.print("Playback:");
       if(SPIFFS.exists(body+".json")) {
        playfile = SPIFFS.open(body+".json", "r");
        }
       else {
        DBG_OUTPUT_PORT.println(body+".json not found");
        }
      break;
    case 'T': //trim left / right servos
      DBG_OUTPUT_PORT.print("Trim");
      if ('L' == body.charAt(1)) {
        DBG_OUTPUT_PORT.print(" Left");
        if ('-' == body.charAt(2)) {
          DBG_OUTPUT_PORT.print(" Down ");
          leftTrim -= TRIM_ADJ;
          }
        else {
          DBG_OUTPUT_PORT.print(" Up ");
          leftTrim += TRIM_ADJ;
          }
        DBG_OUTPUT_PORT.println(leftTrim);
        }
      if ('R' == body.charAt(1)) {
        DBG_OUTPUT_PORT.print(" Right");
        if ('-' == body.charAt(2)) {
          DBG_OUTPUT_PORT.print(" Down ");
          rightTrim -= TRIM_ADJ;
          }
        else {
          DBG_OUTPUT_PORT.print(" Up ");
          rightTrim += TRIM_ADJ;
          }
        DBG_OUTPUT_PORT.println(rightTrim);
        }
      setWheelPower(0,0); //test the trim
      break;
    default: 
      int index = body.indexOf(":"), index2 = body.indexOf(":", index + 1);
      int i = body.substring(0, index).toInt();
      int j = (index2 >= 0)? 
        body.substring(index + 1, index2).toInt(): 
        body.substring(index + 1).toInt();  
      int k = (index2 >= 0)? 
        body.substring(index2 + 1).toInt(): 
        0;
    
      setWheelPower(i, j);
      setWeaponPower(k);
      break;
    }
    request->send(200, "text/plain", "ok");
  } else { //body is required
    request->send(400);
  }  
}

/********************************************************************************
 * WebSocket Event Handler                                                      *
 *  Implement the WebSocket interface. These functions handle connect,          *
 *  disconnect and data packets from the client.                                *
 ********************************************************************************/

// websocket server instance //
AsyncWebSocketClient *_activeClient;

void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch(type) {
    case WS_EVT_CONNECT:
      // we connected, print debug message //
      {
        IPAddress ip = client->remoteIP();
        DBG_OUTPUT_PORT.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", client->id(), ip[0], ip[1], ip[2], ip[3], server->url());
      }
      // send success message to client //
      client->printf("Connected: %u", client->id());
      client->ping();
      // enable driving //
      _activeClient = client;
      enterState(STATE_DRIVING_WITH_TIMEOUT);
      setWheelPower(0, 0);
      setWeaponPower(0);
      break;
    case WS_EVT_DISCONNECT:
      DBG_OUTPUT_PORT.printf("[%u] Disconnected!\n", client->id());
      _activeClient = NULL;
      enterState(STATE_IDLE);
      break;
    case WS_EVT_ERROR:
      DBG_OUTPUT_PORT.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*) arg), (char*) data);
      break;
    case WS_EVT_PONG:
      DBG_OUTPUT_PORT.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)? (char*)data: "");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo * info = (AwsFrameInfo*) arg;
      // only handle a single data packet, for now //
      if(info->final && info->index == 0 && info->len == len) {
        if(info->opcode == WS_TEXT) {
          data[len] = 0;
          //DBG_OUTPUT_PORT.printf("[%u] command: %s\n", client->id(), data);
          {
            enterState(STATE_DRIVING_WITH_TIMEOUT);
  
            String cmd((char *) data);
            updateHardware(cmd);
          }
        }
      }
      break;
  }
}

/********************************************************************************
 * Hardware Control                                                             *
 *  Handle control of robot hardware based on calls to the web API.             *
 ********************************************************************************/

// map motor driver channels //
#define PIN_R_PWM   PIN_PWM_A   // A is right      //
//#define PIN_R_DIR   PIN_DIR_A   // Not needed, could be used for other IO

#define PIN_L_PWM   PIN_PWM_B   // B is left       //
//#define PIN_L_DIR   PIN_DIR_B   // Not needed, could be used for other IO

#define WHEEL_BACK_USEC 1000
#define WHEEL_CENTER_USEC 1500
#define WHEEL_FWD_USEC 2000

// use D5 for WIFI override //
#define PIN_WIFI_AP_MODE  PIN_D5

#define ESC_MIN_USEC    1000 //400
#define ESC_MAX_USEC    2000 //2400

Servo weaponESC;
Servo leftWheel;
Servo rightWheel;

//NewPingESP8266 sonar(PIN_PING, PIN_ECHO, MAX_DISTANCE); //Arduino IDE
NewPing sonar(PIN_PING, PIN_ECHO, MAX_DISTANCE); //platformIO

// configure the hardware //
void setupHardware() {
  lastping = 0;
  leftTrim = 0;
  rightTrim = 0;
  // motor control pins to output //
  leftWheel.attach(PIN_R_PWM);
  leftWheel.writeMicroseconds(WHEEL_CENTER_USEC + leftTrim);
  rightWheel.attach(PIN_L_PWM);
  rightWheel.writeMicroseconds(WHEEL_CENTER_USEC + rightTrim);

  // LED to output //
  pinMode(PIN_LED2, OUTPUT);
  setStatusLED(false);

  // PING //
  pinMode(PIN_PING, OUTPUT); // TRIGGER
  pinMode(PIN_ECHO, INPUT);  // ECHO
  
  // WiFi override //
  pinMode(PIN_WIFI_AP_MODE, INPUT_PULLUP);

  weaponESC.attach(PIN_WEAPON_ESC);
  weaponESC.writeMicroseconds(ESC_MIN_USEC);
}



// get the debugging LED //
bool getStatusLED() {
  return !digitalRead(PIN_LED2);
}

// set the debugging LED //
void setStatusLED(bool active) {
  digitalWrite(PIN_LED2, !active);
}

// is the WiFi forced to AP mode by jumper //
bool getWiFiForceAPMode() {
  return !digitalRead(PIN_WIFI_AP_MODE);
}
// set power to the wheels //
void setWheelPower(int left, int right) {
  left  = constrain(left,  -1023, 1023);
  left = map(left, -1023, 1023, WHEEL_BACK_USEC, WHEEL_FWD_USEC);
  right = constrain(right, -1023, 1023);
//  right = map(right, -1023, 1023, WHEEL_BACK_USEC, WHEEL_FWD_USEC);
  right = map(right, -1023, 1023, WHEEL_FWD_USEC, WHEEL_BACK_USEC);

  if(PRINT_TO_SERIAL_MONITOR) {
    DBG_OUTPUT_PORT.printf("left: %d, right: %d", left, right);
  }
  
//  digitalWrite(PIN_L_DIR, left >= 0);
//  digitalWrite(PIN_R_DIR, right >= 0);
  
//  analogWrite(PIN_L_PWM, abs(left));
//  analogWrite(PIN_R_PWM, abs(right));
  leftWheel.writeMicroseconds(left + leftTrim);
  rightWheel.writeMicroseconds(right + rightTrim);

}


// set the weapon power //
void setWeaponPower(int power) {  
  if(PRINT_TO_SERIAL_MONITOR) {
    DBG_OUTPUT_PORT.printf(", weapon: %d\n", power);
  }
  int usec = map(power, 0, 1023, ESC_MIN_USEC, ESC_MAX_USEC);
  weaponESC.writeMicroseconds(usec);
}

// interpret a command string //
//  format: "${leftPower}:${rightPower}(:${weaponPower})?"
//
//  where:  leftPower   - int [-1023, 1023]
//          rightPower  - int [-1023, 1023]
//          weaponPower - int [-1023, 1023] (optional)
//
//  positive values are forward 
void updateHardware(String cmd) {
    int index = cmd.indexOf(":"), index2 = cmd.indexOf(":", index + 1);
    int leftPower  = cmd.substring(0, index).toInt();
    int rightPower = (index2 >= 0)? 
      cmd.substring(index + 1, index2).toInt(): 
      cmd.substring(index + 1).toInt();  
    int weaponPower = (index2 >= 0)? 
      cmd.substring(index2 + 1).toInt(): 
      0;
    
    setWheelPower(leftPower, rightPower);
    setWeaponPower(weaponPower);
}

/********************************************************************************
 * Status State Machine                                                         *
 *  State machine to implement global status handling within firmware. Handles  *
 *  blinking of the status LED as well as the command timeout.                  *
 ********************************************************************************/

#define DRIVE_TIMEOUT  2000  // only drive for 2 seconds w/o connection //

RobotState _state = STATE_START, _lastState;
unsigned long _stateDelay, _driveTimeout;

// enter the specified state //
void enterState(RobotState state) {
  switch(state) {
    case STATE_CONNECT:
      setStatusLED(false);
      break;
    case STATE_IDLE:
      setStatusLED(false);
      setWheelPower(0, 0);
      setWeaponPower(0);
      break;
    case STATE_DRIVING_WITH_TIMEOUT:
      _driveTimeout = millis() + DRIVE_TIMEOUT;
      break;
    case STATE_START:
    case STATE_SETUP:
    case STATE_DRIVING:
      break;
  }
  if(_state != state) {
    DBG_OUTPUT_PORT.print("State: "); DBG_OUTPUT_PORT.println(state);
  }
  _state = state;  
}

// run the state machine //
void runStateMachine() {
  switch(_state) {
    case STATE_START:
      setStatusLED(false);
      break;
    case STATE_SETUP:
      setStatusLED(true);
      break;
    case STATE_CONNECT:
      // fast blink //
      if (millis() > _stateDelay) {
        setStatusLED(!getStatusLED()); // toggle LED //
        _stateDelay = millis() + 100;
      }
      break;
    case STATE_IDLE:
      // chirp blink //
      if (millis() > _stateDelay) {
        setStatusLED(!getStatusLED());
        _stateDelay = millis() + (getStatusLED()? 100: 400);
        if (getStatusLED()) { 
          webSocketMessage("idle");
          }
        else if (playback.length()==0) {
          if(SPIFFS.exists("/"+String(playdo)+".json")) {
            playfile = SPIFFS.open("/"+String(playdo)+".json", "r");
            playdo++;
            }
          else { playdo = 0; }
          }
        }
      break;
    case STATE_DRIVING_WITH_TIMEOUT:
      // check for timeout, if expired, stop driving //
      if(millis() > _driveTimeout) {
        enterState(STATE_IDLE);
        break;
      }
      // note: fall-through here //
    case STATE_DRIVING:
      // medium blink //
      if (millis() > _stateDelay) {
        setStatusLED(!getStatusLED());
        _stateDelay = millis() + 500;

        // send heartbeat back to client //
        webSocketMessage("driving");
      }
      break;
  }
}

/********************************************************************************
 * Main Entry Points                                                            *
 *  Configure the hardware, connect to WiFi, start mDNS and setup web server    *
 *  route handling.                                                             *
 ********************************************************************************/

// web server instance //
AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// set a message to the active web socket //
// used for heartbeat to client app       //
void webSocketMessage(String msg) {
  lastping = sonar.ping_cm();
  if(!_activeClient) return;
  //DBG_OUTPUT_PORT.println(String(lastping));
  msg = "{\"msg\":\"" + msg + "\""
    + ",\"cm\":" + String(lastping) 
    + ",\"IO1\":" + String(digitalRead(PIN_IO_1)) //See NodeMCU-Hardware for pin assignments
    + ",\"IO2\":" + String(digitalRead(PIN_IO_2))
    + ",\"IO3\":" + String(digitalRead(PIN_IO_3))
    + ",\"ir\":" + "\"\""
    + "}"
    ;
  //DBG_OUTPUT_PORT.printf("[%u] msg: %s\n", _activeClient->id(), msg.c_str());
  _activeClient->text(msg.c_str());
  }
void setup(void){
  // configure debug serial port //
  DBG_OUTPUT_PORT.begin(DBG_BAUD_RATE);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);

  playdo=0;
  
  enterState(STATE_SETUP);
  runStateMachine();
  
  // configure the hardware //
  setupHardware();

  // start file system //
  if (!SPIFFS.begin()) {
    DBG_OUTPUT_PORT.println("ERROR: File System");
  }
  else {
    DBG_OUTPUT_PORT.println("FS Started");
//     Dir dir = SPIFFS.openDir("/");
//     while (dir.next()) {
//       DBG_OUTPUT_PORT.print(dir.fileName());
//       if(dir.fileSize()) {
//         File f = dir.openFile("r");
//         DBG_OUTPUT_PORT.println(f.size());
//       }
//     }
  }

  // start wifi //
  setupWiFi();

  // attach WebSocket interface //
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);

  // attach REST interface //
  server.on("/control", HTTP_PUT, handleControlPut);

  // serve static file system //
  server
    .serveStatic("/", SPIFFS, "/")
    .setDefaultFile("index.html");
  
  // file system editor //
#ifdef ESP32
  server.addHandler(new SPIFFSEditor(SPIFFS));
#elif defined(ESP8266)
  server.addHandler(new SPIFFSEditor("","",SPIFFS));
  //must specify empty user and pass to set FS... 
#endif

  // 404'd! //
  server.onNotFound([](AsyncWebServerRequest *request){
    DBG_OUTPUT_PORT.print("404:");
    DBG_OUTPUT_PORT.println(request->url());
    request->send(404);
  });
  
  // we're ready //
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");
  enterState(STATE_IDLE);
}

unsigned long playbackDelay;
unsigned long playbackSpeed;
void loop(void){
  int i;
  if (Serial.available() > 0) {
    String body = Serial.readString();
    DBG_OUTPUT_PORT.println(body);
    if(SPIFFS.exists(body+".json")) {
      playfile = SPIFFS.open(body+".json", "r");
      } else { DBG_OUTPUT_PORT.println("not found"); }
    }
  if (playfile) {
      playback = playfile.readString();
      playfile.close();
      if ('[' == playback.charAt(0)) {
        playback.remove(0,1);
        }
      playbackSpeed = playback.toInt();
      i = playback.indexOf('[');
      playback.remove(0,i);
      DBG_OUTPUT_PORT.println("Play");
      playbackDelay = 0;
    };
  if (playback.length() > 0) {
    delay(1);
    if (millis() > playbackDelay) {
      playbackDelay = millis() + playbackSpeed;
      // send heartbeat back to client during playback every 500ms //
      webSocketMessage("playback");
      DBG_OUTPUT_PORT.println(".");
      if ('[' == playback.charAt(0)) {
        playback.remove(0,1);
        int i = playback.indexOf(']');
        int j = playback.indexOf(',');
        if (i>0 && j>0 && j < i) {
          playbackSpeed = playback.toInt();
          playback.remove(0,j+1);
          i -= j+1;
          DBG_OUTPUT_PORT.print("t:");
          DBG_OUTPUT_PORT.println(playbackSpeed);
        }
        if (i > 0) {
          DBG_OUTPUT_PORT.println(playback.substring(0,i));
          updateHardware(playback.substring(0,i));
          playback.remove(0,i+1);
          i = playback.indexOf('['); //find next record
          if (i > 0) {playback.remove(0,i);} //skip crap 
          }
        else { playback = ""; }
        }
      else { playback = ""; }
      }
    }
  else { runStateMachine(); }
}