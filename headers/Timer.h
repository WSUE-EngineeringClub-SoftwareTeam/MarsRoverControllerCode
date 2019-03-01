#include "Rover.h"

void setupTimer(void);
void __ISR(_TIMER_2_VECTOR, IPL6SOFT) Timer23InterruptHandler(void);