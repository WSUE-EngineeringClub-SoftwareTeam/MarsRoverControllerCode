#include "Bluetooth.h"
#include "p32xxxx.h"
#include "plib.h"


void sendBluetoothChar(char data)
{
    while ((U2STA & 0x200)) LATB = 0x100;
    LATB = 0;
    U2TXREG = data;
}

void sendBluetoothString(char* data, uint length)
{
    int i = 0;
    for (;i < length; i++) sendBluetoothChar(data[i]);
}

void setupBluetooth(void){
    U2STASET = 0x1040;
    IEC1SET = 1 << 9;
    IFS1CLR = 1 << 9;
    IPC8SET = 6 << 2;
}

void __ISR(_UART_2_VECTOR, IPL6SOFT) BluetoothInterruptHandler(void)
{
    asm volatile("DI");
    IFS1CLR = 1 << 9;
    last_blue = U2RXREG;
    is_different = 1;
    if (U2STA & 2) U2STACLR = 2;   // clear overrun if it occurred
    asm volatile("EI");
}