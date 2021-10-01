/**
 *  Hardware assignments for ESP32 DevKitC
 *
 **/



// module LEDs (active low) //
#define PIN_LED1  2    //  //
#define PIN_LED2  2    // only one LED on the ESP32 GPIO2 //

// motor driver //
#define PIN_PWM_A 16
#define PIN_DIR_A 5    // will spit garbage on startup
#define PIN_PWM_B 17
#define PIN_DIR_B 18   

// use D6 for weapon ESC //
#define PIN_WEAPON_ESC  19

// Ping //
#define PIN_PING 22 // Ping Trigger, TX1 debug serial out
#define PIN_ECHO 23 // Ping Echo back

// IO Pins //
#define PIN_IO_1 32 // ADC4
#define PIN_IO_2 33 // ADC5
#define PIN_IO_3 34 // ADC6 CAUTION: Input only

// Mode Select //
#define PIN_D5 27 //wifi AP select pin