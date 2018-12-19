// rover top
#include <stdio.h>
#include "p32xxxx.h"
#include "plib.h"
#include "Rover.h"

void initialize(uint*);
void getDatachan(uint*,uint*);
void sendLCDData(char);
void clearLCD(void);
void dispLCD(char*, uint length);
void showNumber(uint num, uint location);
BOOL isBetween(uint, uint, uint);
void sendToBottom(uchar data);
uchar receiveFromBottom(void);
void sendChannelData(uchar channel);


uint channel_start[6];
uint channel_data[7];

#pragma config FNOSC = PRIPLL // Oscillator selection
#pragma config POSCMOD = EC // Primary oscillator mode
#pragma config FPLLIDIV = DIV_2 // PLL input divider
#pragma config FPLLMUL = MUL_20 // PLL multiplier
#pragma config FPLLODIV = DIV_1 // PLL output divider
#pragma config FPBDIV = DIV_2 // Peripheral bus clock divider
#pragma config FSOSCEN = OFF // Secondary oscillator enable

uint buffer;
uint data[5] = {0, 0, 0, 0, 0};
uchar mask[5] = { 0x80, 0xC0, 0xE0, 0xD0, 0xF0 };
uint buffer2;

void main(void)
{
    initialize(channel_data);
    clearLCD();
    int i,j;
    
    while (1)
    {

        for (i = 0; i < 4; i++) {
            //for (j = 0; j < 200; j++);
            sendChannelData(i);
            showNumber(data[i], i);
            
        }
    }
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

BOOL isBetween(uint num, uint num1, uint num2)
{
    if (num >= num1)
        if (num <= num2) return TRUE;
    return FALSE;
}

uchar getMode(uint data)
{
    if (isBetween(data, MODE0 - 1, MODE0+1)) return 0;
    if (isBetween(data, MODE1 - 1, MODE1+1)) return 1;
    if (isBetween(data, MODE2 - 1, MODE2+1)) return 2;
    return 4;
}

void initialize(uint* channel)
{
    asm volatile("DI");     // disable interrupts
    
    INTCON = 0x1000;        // turn on multi vector mode, change external internal interrupts leading edge
    
    uint checksum = 0;
    // Create and initialize 6 data channels
    int i = 0;
    for (;i < 6; i++) channel[i] = 0;
    channel[FLAGS] = SAFETY;    // Turn on safety

    // Set Up Timer - SMCLK, continuous up mode, clock divisions 1
    T2CON = 0x78;       // 256 divider, 32 bit
    T2CON = 0;
    PR2 = 0xFFFF;
    TMR2 = 0;
    
    // Channel 5 - RD12 JK
    IC5CON = 0x8361;
    IEC0 = 0x222220;
    IPC5 = 0x1800;
    
    // Channel 4 - RD11 JE
    IC4CON = 0x8361;
    IPC4 = 0x1800;
    
    // Channel 3 - RD10 JD
    IC3CON = 0x8361;
    IPC3 = 0x1800;
    
    // Channel 2 - RD09 JD
    IC2CON = 0x8361;
    IPC2 = 0x1800;
    
    // Channel 1 - RD08 JH
    IC1CON = 0x8361;
    IPC1 = 0x1800;
    
    // Configure Pins
    TRISBCLR = 0x3C00;      // LEDs
    TRISFCLR = 0x100;

    // Default all pins low
    
    //Setup SPI interface (debug)
    U2MODE = 0x8000;
    U2STA = 0x400;
    U2BRG = 259;

    //CONFIGURING UART BAUD RATE
    // bit 15 - on
    U1MODE = 0x8000;
    // bit 12 - rx enable, 10 - tx enable, 7,6 - interrupt on full rx
    U1STA = 0x14C0;
    // ~9600 baud
    U1BRG = 259;
    // interrupt on
    IEC0SET = 1 << 27;
    IPC6SET = 6 << 2;
    
    //set up LCD
    for (i = 0; i < 20000; i++) { asm volatile("NOP"); }
    char temp[] = { 0x1B, '[', 'j', 0x1B, '[', '0', 'h', 0x1B, '[', '0', ';', '0', 'H' };
    dispLCD(temp, 13);
    
    // Timer interrupt
    IEC0SET = 1 << 12;
    IPC3SET = 6 << 2;

    asm volatile("EI");     // enable interrupts
    T2CON = 0x8078;      // start timer
}

void sendLCDData(char data)
{
    while ((U2STA & 0x200)) LATB = 0x100;
    LATB = 0;
    U2TXREG = data;
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

/*void __ISR(_TIMER_3_VECTOR, IPL6SOFT) Timer23InterruptHandler(void)
{
    asm volatile("DI");
    IFS0CLR = 1 << 12;
    static int i = 0;
    for (i = 0; i < 4; i++) {
        channel_data[i] = data[i];
        data[i] = 0;
    }
        
    
    asm volatile("EI");
}
*/
void __ISR(_UART1_VECTOR, IPL6SOFT) UART1InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 1 << 27;
    static uchar d1, d2, d3, d4, channelMask;
    buffer = U1RXREG;
    channelMask = buffer & 0xF0;
    switch (channelMask)
    {
        case 0x80: channelMask = CHANNEL1; break;
        case 0xC0: channelMask = CHANNEL2; break;
        case 0xA0: channelMask = CHANNEL3; break;
        case 0x40: channelMask = CHANNEL4; break;
        case 0x60: channelMask = CHANNEL5; break;
    }
    d1 = buffer & 0xF;
    d2 = U1RXREG & 0xF;
    d3 = U1RXREG & 0xF;
    d4 = U1RXREG & 0xF;
    data[channelMask] = d1 + (d2 << 4) + (d3 << 8) + (d4 << 12);
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_1_VECTOR, IPL6SOFT) Channel1InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x20;
    buffer = IC1BUF;
    buffer2 = buffer-channel_start[CHANNEL1];
    if (buffer2 > 1000 && buffer2 < 4000)
    {
        channel_data[CHANNEL1] = buffer-channel_start[CHANNEL1];
        channel_start[CHANNEL1] = buffer;
    }
    //data[0]++;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL6SOFT) Channel2InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x200;
    buffer = IC2BUF;
    buffer2 = buffer-channel_start[CHANNEL2];
    if (buffer2 > 1000 && buffer2 < 4000)
    {
        channel_data[CHANNEL2] = buffer-channel_start[CHANNEL2];
        channel_start[CHANNEL2] = buffer;
    }
    //data[1]++;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL6SOFT) Channel3InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x2000;
    buffer = IC3BUF;
    buffer2 = buffer-channel_start[CHANNEL3];
    if (buffer2 > 1000 && buffer2 < 4000)
    {
        channel_data[CHANNEL3] = buffer-channel_start[CHANNEL3];
        channel_start[CHANNEL3] = buffer;
    }
    //data[2]++;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL6SOFT) Channel4InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x20000;
    buffer = IC4BUF;
    buffer2 = buffer-channel_start[CHANNEL4];
    if (buffer2 > 1000 && buffer2 < 4000)
    {
        channel_data[CHANNEL4] = buffer-channel_start[CHANNEL4];
        channel_start[CHANNEL4] = buffer;
    }
    //data[3]++;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_5_VECTOR, IPL6SOFT) Channel5InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x200000;
    
    buffer = IC5BUF;
    buffer2 = buffer-channel_start[CHANNEL5];
    if (buffer2 > 1000 && buffer2 < 4000)
    {
        channel_data[CHANNEL5] = buffer-channel_start[CHANNEL5];
        channel_start[CHANNEL5] = buffer;
    }
    //data[4]++;
    
    asm volatile("EI");
}

//Top board functions
void sendToBottom(uchar data)
{
	while ((U1STA & 0x200)) LATB = 0x1000;
	LATB = 0;
	data = 
	U1TXREG = data;

}

uchar receiveFromBottom(void)
{
	uchar data;

	while ((U1STA & 0b1)) LATB = 0x1000;
	LATB = 0;
    data = U1RXREG;
	return data;
}

void sendChannelData(uchar channel)
{
    uchar d1,d2,d3,d4;
    d1 = mask[channel] + (channel_data[channel] & 0xF);
    d2 = mask[channel] + ((channel_data[channel] & 0xF0) >> 4);
    d3 = mask[channel] + ((channel_data[channel] & 0xF00) >> 8);
    d4 = mask[channel] + ((channel_data[channel] & 0xF000) >> 12);
    sendToBottom(d1); sendToBottom(d2); sendToBottom(d3); sendToBottom(d4);
}
