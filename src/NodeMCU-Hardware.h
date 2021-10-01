/**
 *  Hardware assignments for NodeMCU + Motor Shield module
 *
 *  This info was surprisingly annoying to locate from the various
 *  online sources, mainly because you have ESP8266 pins mapped to 
 *  NodeMCU pins mapped to motor shield / dev board pins.
 **/

// raw GPIO pins //
#define PIN_D0    16    // RED LED / Wake. Supports digital read/write only, no PWM, pullup, etc. //
#define PIN_D1    5     // GPIO / I2C SCL
#define PIN_D2    4     // GPIO / I2C SDA
#define PIN_D3    0     // Boot mode
#define PIN_D4    2     // BLU LED / TX1 / Boot mode (high), sends debug info
#define PIN_D5    14    // GPIO / SPI CLK
#define PIN_D6    12    // GPIO / SPI MISO
#define PIN_D7    13    // GPIO / SPI MOSI
#define PIN_D8    15    // GPIO (No pullup) / SPI SS. 
#define PIN_D9    3     // RX0 Program / Monitor
#define PIN_D10   1     // TX0 Monitor / Program
#define PIN_D11   9     // FLASH (don't use)
#define PIN_D12   10    // FLASH (don't use)

// alternative GPIO pin labels //
#define PIN_RX    PIN_D9
#define PIN_TX    PIN_D10
#define PIN_SD2   PIN_D11
#define PIN_SD3   PIN_D12

// module LEDs (active low) //
#define PIN_LED1  PIN_D4    // near antenna on ESP8266, shared with DIR_B //
#define PIN_LED2  PIN_D0    // on NodeMCU, closer to USB connector //

// motor driver //
#define PIN_PWM_A PIN_D1
//#define PIN_DIR_A PIN_D3    // high for forward //
#define PIN_PWM_B PIN_D2
//#define PIN_DIR_B PIN_D4    // high for forward //

// use D6 for weapon ESC //
#define PIN_WEAPON_ESC  PIN_D3 //PIN_D6

// Ping //
#define PIN_PING PIN_D7 // Ping Trigger
#define PIN_ECHO PIN_D5 // Ping Echo back
//can't use D8 for echo: must be low on start or ESP will try to boot from external SD Card.

// IO Pins //
#define PIN_IO_1 PIN_D6 //PIN_D3 // If not needed for Motor A direction
#define PIN_IO_2 PIN_D4 // If not needed for Motor B direction
#define PIN_IO_3 PIN_D8 // CAUTION: D8 must start low! Use only for sensor that pulls the pin up

/**
 * For reference, the motor sheild board also has the following connections.
 **/
//  Shield pin    NodeMCU pin
//   SPI_CLK       CLK
//   SPI_MISO      SDO
//   SPI_MOSI      CMD
//   SPI_INT       SD1
