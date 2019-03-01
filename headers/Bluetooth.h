#include "Rover.h"

void sendBluetoothChar(char data);
void sendBluetoothString(char* data, uint length);
void setUpBluetooth(void);
void __ISR(_UART_2_VECTOR, IPL6SOFT) BluetoothInterruptHandler(void);
