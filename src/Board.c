#include "Board.h"
#include "plib.h"



void setupBoard_UART2(void)
{
    U2STA = 0x14C0;
    IEC1SET = 1 << 9;
    IPC8SET = 6 << 2;
}


void setupBoard_UART1(void)
{
    U2STA = 0x400;
}

void sendToTop(uchar c)
{
    if (!c) return;
    while ((U2STA & 0x200)) LATB = 0x100;
    LATB = 0;
    U2TXREG = c;
}

void sendBoardData(uchar channel)
{
    uchar d1,d2,d3,d4;
    d1 = channel + 1;
    d2 = 128 + (rx_channel_data[channel] & 0x7F);
    d3 = 128 + (((rx_channel_data[channel]) & 0x3F80) >> 7);
    d4 = 128;
    sendToTop(d1); sendToTop(d2); sendToTop(d3); sendToTop(d4);
}

void __ISR(_UART2_VECTOR, IPL6SOFT) UART2InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS1CLR = 1 << 9;
    static uchar d0, d1, d2;
    static uchar count[5];
    static uchar channel;
    static uint i;
    d0 = U2RXREG;
    d1 = d0 & 128;
    d0 &= 127;
    if (!d1 && d0 > 0 && d0 < 6)
    {
        d1 = U2RXREG & 0x7F;
        d2 = U2RXREG & 0x7F;
        rx_channel_data[d0 - 1] = d1 + (((uint)(d2)) << 7);
    } else {
        d0 = U2RXREG;
        d1 = d0 & 128;
    d0 &= 127;
        if (!d1 && d0 > 0 && d0 < 6)
        {
            d1 = U2RXREG & 0x7F;
            d2 = U2RXREG & 0x7F;
            rx_channel_data[d0 - 1] = d1 + (((uint)(d2)) << 7);
        }
    }
    U2STACLR = 2;
    
    asm volatile("EI");
}