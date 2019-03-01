#include "Rover.h"
#include "p32xxxx.h"
#include "plib.h"

void __ISR(_INPUT_CAPTURE_1_VECTOR, IPL6SOFT) Channel1InterruptHandler(void);
void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL6SOFT) Channel2InterruptHandler(void);
void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL6SOFT) Channel3InterruptHandler(void);
void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL6SOFT) Channel4InterruptHandler(void);
void __ISR(_INPUT_CAPTURE_5_VECTOR, IPL6SOFT) Channel5InterruptHandler(void);
void setupDragonLink(void);
uchar getMode(uint data);
void getDatachan(uint* channel, uint* datachan);


