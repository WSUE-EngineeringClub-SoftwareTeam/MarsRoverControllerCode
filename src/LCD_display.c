#include "LCD_display.h"
#include "p32xxxx.h"
#include "plib.h"





void sendLCDData(char data)
{
    if (!data) return;
    while ((U2STA & 0x200)) LATB = 0x100;
    LATB = 0;
    U1TXREG = data;
}

void dispLCD(char* data, uint length)
{
    int i = 0;
    for (;i < length; i++) sendLCDData(data[i]);
}

void clearLCD(void)
{
    char data[4] = { ESC, '[', 'j', '\0'};
    dispLCD(data,3);
}

void showNumber(uint num, uint location)
{
    char buffer[34] = { 0x1B, '[', '0', ';', '0', 'H' };
    switch (location)
    {
        case 1: buffer[4] = '5'; break;
        case 2: buffer[2] = '1'; break;
        case 3: buffer[2] = '1'; buffer[4] = '5'; break;
        //case 4: buffer[2] = '1'; buffer[4] = '9'; break;
    }    
    
    //buffer[6] = (num/10000) + 48;
    buffer[6] = (num/1000)%10 + 48;
    buffer[7] = (num/100)%10 + 48;
    buffer[8] = (num/10)%10 + 48;
    buffer[9] = num%10 + 48;
    dispLCD(buffer,11);
}

void setupLCD(void)
{
    for (i = 0; i < 20000; i++) { asm volatile("NOP"); }
    char temp[] = { 0x1B, '[', 'j', 0x1B, '[', '0', 'h', 0x1B, '[', '0', ';', '0', 'H' };
    dispLCD(temp, 13);
}