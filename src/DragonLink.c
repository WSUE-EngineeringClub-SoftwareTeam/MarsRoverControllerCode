#include "DragonLink.h"

void __ISR(_INPUT_CAPTURE_1_VECTOR, IPL6SOFT) Channel1InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x20;
    buffer = IC1BUF;
    channel_data[CHANNEL1] = buffer-channel_start[CHANNEL1];
    channel_start[CHANNEL1] = buffer;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL6SOFT) Channel2InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x200;
    buffer = IC2BUF;
    channel_data[CHANNEL2] = buffer-channel_start[CHANNEL2];
    channel_start[CHANNEL2] = buffer;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL6SOFT) Channel3InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x2000;
    buffer = IC3BUF;
    channel_data[CHANNEL3] = buffer-channel_start[CHANNEL3];
    channel_start[CHANNEL3] = buffer;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL6SOFT) Channel4InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x20000;
    buffer = IC4BUF;
    channel_data[CHANNEL4] = buffer-channel_start[CHANNEL4];
    channel_start[CHANNEL4] = buffer;
    
    asm volatile("EI");
}

void __ISR(_INPUT_CAPTURE_5_VECTOR, IPL6SOFT) Channel5InterruptHandler(void)
{
    asm volatile("DI");
    
    IFS0CLR = 0x200000;
    
    buffer = IC5BUF;
    channel_data[CHANNEL5] = buffer-channel_start[CHANNEL5];
    channel_start[CHANNEL5] = buffer;
    
    asm volatile("EI");
}

void setupDragonLink(void)
{
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
}

uchar getMode(uint data)
{
    if (isBetween(data, MODE0 - 1, MODE0+1)) return 0;
    if (isBetween(data, MODE1 - 1, MODE1+1)) return 1;
    if (isBetween(data, MODE2 - 1, MODE2+1)) return 2;
    return 4;
}

void getDatachan(uint* channel, uint* datachan)
{
    static uint last_0 = 0;
    static uint last_1 = 0;
    static uint last_2 = 0;
    static uint last_3 = 0;
    datachan[0] = (channel[CHANNEL1]-CHANNEL1_MIN);
    if (datachan[0] > CHANNEL1_RANGE) datachan[0] = last_0;
    last_0 = datachan[0];
    datachan[1] = (channel[CHANNEL2]-CHANNEL2_MIN);
    if (datachan[1] > CHANNEL2_RANGE) datachan[1] = last_1;
    last_1 = datachan[1];
    datachan[2] = (channel[CHANNEL3]-CHANNEL3_MIN);
    if (datachan[2] > CHANNEL3_RANGE) datachan[2] = last_2;
    last_2 = datachan[2];
    datachan[3] = (channel[CHANNEL4]-CHANNEL4_MIN);
    if (datachan[3] > CHANNEL4_RANGE) datachan[3] = last_3;
    last_3 = datachan[3];
}