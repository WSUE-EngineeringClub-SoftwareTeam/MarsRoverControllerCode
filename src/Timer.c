#include "Timer.h"
#include "p32xxxx.h"
#include "plib.h"

void setupTimer(void)
{
    IEC0SET = 1 << 12;
    IPC3SET = 6 << 2;
}

void __ISR(_TIMER_2_VECTOR, IPL6SOFT) Timer23InterruptHandler(void)
{
    asm volatile("DI");
    IFS0CLR = 1 << 12;
    static int i = 0;
    for (i = 0; i < 4; i++)
        if (channel_data[i] == last_blue[i]) channel_data[i] = 0;
    
    asm volatile("EI");
}