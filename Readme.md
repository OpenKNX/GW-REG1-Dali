# KNX IR-Modul
Can be used to receive and send ir-codes.

## things to do
- create hardware.h in lib/knx/include
 - #define PROG_BUTTON_PIN 0
 - #define PROG_LED_PIN 0
 - #define PROG_LED_PIN_ACTIVE_ON 1
- create knxprod.h in lib/knx/include
 - #define MAIN_ variables
 - #define HARDWARE_NAME
- pio flag
 - SERIAL_DEBUG=KNX_DEBUG_SERIAL
 - KNX_UART_RX_PIN=
 - KNX_UART_TX_PIN=