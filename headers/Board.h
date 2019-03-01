#include "Rover.h"
#include "p32xxxx.h"
#include "plib.h"

void setupBoard_UART2(void);
void setupBoard_UART1(void);
void sendToTop(uchar c);
void sendBoardData(uchar channel);
void __ISR(_UART2_VECTOR, IPL6SOFT) UART2InterruptHandler(void);