//#include <stdio.h>
#include "p32xxxx.h"
#include "plib.h"
#include "Rover.h"

/***************
 **Components***
 ***************/
//#define _BLUE       // if bluetooth is connected
//#define _LCD        // if lcd screen is connected
#define _MOTORS     // if motor controllers are connected
//#define _DL         // if dragon link is connected
#define _BOARD      // if top board is attached
//#define _TIMER      // if timer2 interrupt is needed

/***************
 ***Prototypes** 
 ***************/
void initialize(uint*);
BOOL isBetween(uint, uint, uint);
void setLeds(uchar);
#ifdef _LCD
void sendLCDData(char);
void clearLCD(void);
void dispLCD(char*, uint length);
void showNumber(uint num, uint location);
#endif
#ifdef _BLUE
void sendBluetoothChar(char data);
void sendBluetoothString(char* data, uint length);
#endif
#ifdef _DL
uchar getMode(uint);
void getDatachan(uint*,uint*);
#endif
#ifdef _MOTORS
void doMoveMode(uint*);
void doArmMode(uint*);
void doClawMode(uint*);
void shutdownMotors(void);
void serialOut(uint, uint, uint);
uchar sendMotorData(uchar);
void driveRover(uint* datachan);
void driveBaseRotation(ChannelData rotation);
void driveWristRotation(ChannelData wrist);
void driveWrist(ChannelData wrist);
void driveClaw(ChannelData claw);
void driveBoom(ChannelData boom);
void driveElbow(ChannelData elbow);
void driveWithArdu();
#endif
#ifdef _BOARD
void sendToTop(uchar c);
void sendBoardData(uchar channel);
#endif


/***************
 ***Globals***** 
 ***************/
uint buffer;
uint channel_start[6];
uint channel_data[7];
const uchar mask[5] = { 0x80, 0xC0, 0xA0, 0x40, 0x60 };
uchar char_buffer;
uchar last_blue;
uchar is_different = 0;
uint rx_channel_data[6];


/***************
 ***Pragmas***** 
 ***************/
#pragma config FNOSC = PRIPLL // Oscillator selection
#pragma config POSCMOD = EC // Primary oscillator mode
#pragma config FPLLIDIV = DIV_2 // PLL input divider
#pragma config FPLLMUL = MUL_20 // PLL multiplier
#pragma config FPLLODIV = DIV_1 // PLL output divider
#pragma config FPBDIV = DIV_2 // Peripheral bus clock divider
#pragma config FSOSCEN = OFF // Secondary oscillator enable


/***************
 ***Main***** 
 ***************/
void main(void)
{
    int i,j;
    int x = 0, y = 0;
    initialize(channel_data);
#ifdef _LCD
    clearLCD();
#endif
    
    
#ifdef _DL
    uchar last_mode = getMode(channel_data[CHANNEL5]), current_mode, the_mode;
#endif
    while (1)
    {
#ifdef _DL
        current_mode = getMode(channel_data[CHANNEL5]);
        if (last_mode == current_mode) the_mode = current_mode;
        else the_mode = last_mode;
        switch(the_mode)
        {
            case 0: doMoveMode(channel_data); break;
            case 1: doArmMode(channel_data); break;
            case 2: doClawMode(channel_data); break;
        }
        last_mode = current_mode;
#endif
#ifdef _LCD
        /*for (i = 0; i < 4; i++) {
            //for (j = 0; j < 200; j++);
            showNumber(channel_data[i], i);
        }*/
#endif
#ifdef _BLUE
        
        if (is_different) {
            switch(last_blue) {
                case 'W': serialOut(131, 6, 96+x); break;
                case 'C': serialOut(131, 6, 32-x); break;
                case 'S': shutdownMotors(); break;
                case 'U': serialOut(128, 6, 96+x); serialOut(128, 7, 32-x); serialOut(129, 6, 32-x); serialOut(129, 7, 96+x); break;
                case 'R': serialOut(128, 6, 32-x); serialOut(128, 7, 96+x); serialOut(129, 6, 32-x); serialOut(129, 7, 96+x); break;
                case 'L': serialOut(128, 6, 96+x); serialOut(128, 7, 32-x); serialOut(129, 6, 96+x); serialOut(129, 7, 32-x); break;
                case 'D': serialOut(128, 6, 32-x); serialOut(128, 7, 96+x); serialOut(129, 6, 96+x); serialOut(129, 7, 32-x); break;
                case 'B': serialOut(131, 7, 32-x); break;
                case 'b': serialOut(131, 7, 96+x); break;
                case 'P': serialOut(132, 6, 32-x); break;
                case 'p': serialOut(132, 6, 96+x); break;
                case 'A': serialOut(130, 7, 32-x); break;
                case 'a': serialOut(130, 7, 96+x); break;
                case 'E': serialOut(130, 6, 32-x); break;
                case 'e': serialOut(130, 6, 96+x); break;
                case 'V': serialOut(132, 7, 32-x); break;
                case 'v': serialOut(132, 7, 96+x); break;
                case 'X': x++; break;
                case 'x': x--; break;
                default: shutdownMotors();
            }
            if (x > 31) x == 31;
            if (x < -31) x = -31;
            /*y = x;
            if (y < 0) { sendBluetoothChar('-'); y *= -1; }
            sendBluetoothChar((y/10)+48);
            sendBluetoothChar((y%10)+48);*/
            //showNumber(0, 0);
            //sendLCDData(last_blue);
            is_different = 0;
        }
#endif
#ifdef _BOARD
        //for (i = 0; i < 4; i++) sendBoardData(i);
        //j = 0;
        driveWithArdu();
#endif
    }
}

/***************
 ***Functions***** 
 ***************/
void setLeds(uchar num)
{
    LATB = (PORTB & 0xFFFFC3FF) | ((num & 0xF) << 10);

}

BOOL isBetween(uint num, uint num1, uint num2)
{
    if (num >= num1)
        if (num <= num2) return TRUE;
    return FALSE;
}


#ifdef _LCD
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
#endif


#ifdef _DL
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
#endif


#ifdef _MOTORS
void shutdownMotors(void)
{
    serialOut(128,0,0);       //command stop motor 1
    serialOut(128,4,0);       //command stop motor 2
    serialOut(129,0,0);
    serialOut(129,4,0);
    serialOut(130,0,0);
    serialOut(130,4,0);
    serialOut(131,0,0);
    serialOut(131,4,0);
    serialOut(132,0,0);
    serialOut(132,4,0);
}

void serialOut(uint addr, uint cmd, uint dat)
{
    if(!sendMotorData(addr)) return;
    if(!sendMotorData(cmd)) return;
    if(!sendMotorData(dat)) return;
    if(!sendMotorData((addr+cmd+dat)&127)) return;
}

uchar sendMotorData(uchar data)
{
    int wait = 0;
    while ((U1STA & 0x200)) { LATB = 0x2000; wait++; }
    LATB = 0;
    if (wait == 20000) return 0;
    U1TXREG = data;
    return 1;
    
}





void driveClaw (ChannelData claw)
{
    if (isBetween(claw.data, claw.lower_dead, claw.upper_dead)) claw.data = 64;
    else
    {
        if (claw.data > claw.upper_dead) claw.data = (claw.data-claw.upper_dead)*claw.multiplier2+64;
        else claw.data = ((claw.data)*claw.multiplier1);

    }
    serialOut(130, 6, claw.data);
}

void driveRover(uint* datachan)
{
    static uint cmd1;
    static uint cmd2;
    
    if (isBetween(datachan[CHANNEL1], 80, 100)) { cmd1 = 9; datachan[0] = 0; }
    else
    {
        if (datachan[0] > 100) { cmd1 = 9; datachan[0] = datachan[0] - 100; }
        else { cmd1 = 8; datachan[0] = 80-datachan[0]; }
        datachan[0] *= 1.5;
    }
    serialOut(129, cmd1, datachan[0]);
    
    // Turn Left Right
    if (isBetween(datachan[CHANNEL2], 40, 48)) { cmd2 = 10; datachan[1] = 0; }
    else
    {
        if (datachan[1] > 48) { cmd2 = 11; datachan[1] = datachan[1] - 48; }
        else { cmd2 = 10; datachan[1] = 40 - datachan[1]; }
        datachan[1] *= 2;
    }
    serialOut(129, cmd2, datachan[1]);

}

void driveBaseRotation(ChannelData rotation)
{
    if (isBetween(rotation.data, rotation.lower_dead, rotation.upper_dead)) rotation.data = 64;
    else
    {
        if (rotation.data > rotation.upper_dead) rotation.data = (rotation.range - rotation.data)*rotation.multiplier2;
        else rotation.data = ((rotation.lower_dead - rotation.data)*rotation.multiplier1)+64;

    }
    serialOut(131, 6, rotation.data);
}

void driveBoom(ChannelData boom)
{
    if (isBetween(boom.data, boom.lower_dead, boom.upper_dead)) boom.data = 64;
    else
    {
        if (boom.data > boom.upper_dead) boom.data = (boom.range-boom.data)*boom.multiplier2;
        else boom.data = ((boom.lower_dead-boom.data)*boom.multiplier1)+64;
    }
    serialOut(131, 7, boom.data);
}

void driveElbow(ChannelData elbow)
{
    if (isBetween(elbow.data, elbow.lower_dead, elbow.upper_dead)) elbow.data = 64;
    else
    {
        if (elbow.data > elbow.upper_dead) elbow.data = (elbow.data-elbow.upper_dead)*elbow.multiplier2+64;
        else elbow.data = ((elbow.data)*elbow.multiplier1);

    }
    serialOut(130, 7, elbow.data);
}

void driveWrist(ChannelData wrist)
{
    if (isBetween(wrist.data, wrist.lower_dead, wrist.upper_dead)) wrist.data = 64;
    else
    {
        if (wrist.data > wrist.upper_dead) wrist.data = (wrist.range-wrist.data)*wrist.multiplier2;
        else wrist.data = ((wrist.lower_dead-wrist.data)*wrist.multiplier1)+64;
    }
    serialOut(132, 6, wrist.data);

}

void driveWristRotation(ChannelData wrist)
{
    if (isBetween(wrist.data, wrist.lower_dead, wrist.upper_dead)) wrist.data = 64;
    else
    {
        if (wrist.data > wrist.upper_dead) wrist.data = (wrist.range-wrist.data)*wrist.multiplier2;
        else wrist.data = ((wrist.lower_dead-wrist.data)*wrist.multiplier1)+64;
    }
    serialOut(132, 7, wrist.data);

}
void doMoveMode(uint* channel)
{
    static uint datachan[5];
#ifdef _DL
    getDatachan(channel, datachan);
#endif

    if (channel[FLAGS] != MODE_MOVE)
    {
        serialOut(129,17,8);
        shutdownMotors();
        if (isBetween(datachan[0], 80, 100)) channel[FLAGS] &= ~SAFETY;
        else channel[FLAGS] |= SAFETY;
        channel[FLAGS] = (channel[FLAGS] & ~MODE_MASK) | MODE_MOVE;
    }
    else
    {
        // Motor Movement Forwards Backwards
        driveRover(datachan);

        //Arm Movement (Rotation, Boom)
        ChannelData rotation, boom;
        rotation.data = datachan[CHANNEL4];
        rotation.max = CHANNEL4_MAX;
        rotation.range = CHANNEL4_RANGE;
        rotation.lower_dead = CHANNEL4_LOWERDEAD;
        rotation.upper_dead = CHANNEL4_UPPERDEAD;
        rotation.multiplier1 = 2;
        rotation.multiplier2 = 1.4;
        boom.data = datachan[CHANNEL3];
        boom.max = CHANNEL3_MAX;
        boom.range = CHANNEL3_RANGE;
        boom.lower_dead = CHANNEL3_LOWERDEAD;
        boom.upper_dead = CHANNEL3_UPPERDEAD;
        boom.multiplier1 = 1.07;
        boom.multiplier2 = 2;
        driveBaseRotation(rotation);
        driveBoom(boom);
    }
}

void doArmMode(uint* channel)
{
    static uint datachan[5];
#ifdef _DL
    getDatachan(channel, datachan);
#endif
    if ((channel[FLAGS] & SAFETY) || !(channel[FLAGS] & MODE_ARM))
    {
        serialOut(129,17,8);
        shutdownMotors();
        if (isBetween(datachan[0], 80, 100)) channel[FLAGS] &= ~SAFETY;
        else channel[FLAGS] |= SAFETY;
        channel[FLAGS] = (channel[FLAGS] & ~MODE_MASK) | MODE_ARM;
    }
    else
    {
        ChannelData rotation, boom, elbow, wrist;
        rotation.data = datachan[CHANNEL4];
        rotation.max = CHANNEL4_MAX;
        rotation.range = CHANNEL4_RANGE;
        rotation.lower_dead = CHANNEL4_LOWERDEAD;
        rotation.upper_dead = CHANNEL4_UPPERDEAD;
        rotation.multiplier1 = 2;
        rotation.multiplier2 = 1.4;
        boom.data = datachan[CHANNEL3];
        boom.max = CHANNEL3_MAX;
        boom.range = CHANNEL3_RANGE;
        boom.lower_dead = CHANNEL3_LOWERDEAD;
        boom.upper_dead = CHANNEL3_UPPERDEAD;
        boom.multiplier1 = 1.07;
        boom.multiplier2 = 2;
        elbow.data = datachan[CHANNEL2];
        elbow.max = CHANNEL2_MAX;
        elbow.range = CHANNEL2_RANGE;
        elbow.lower_dead = CHANNEL2_LOWERDEAD;
        elbow.upper_dead = CHANNEL2_UPPERDEAD;
        elbow.multiplier1 = 2;
        elbow.multiplier2 = 1.4;
        wrist.data = datachan[CHANNEL1];
        wrist.max = CHANNEL1_MAX;
        wrist.range = CHANNEL1_RANGE;
        wrist.lower_dead = CHANNEL1_LOWERDEAD;
        wrist.upper_dead = CHANNEL1_UPPERDEAD;
        wrist.multiplier1 = .78;
        wrist.multiplier2 = .72;
        
        driveElbow(elbow);
        driveWrist(wrist);
        driveBaseRotation(rotation);
        driveBoom(boom);
    }
}

void doClawMode(uint* channel)
{
    static uint datachan[5];
//    static uint checksum;
#ifdef _DL
    getDatachan(channel, datachan);
#endif

    if ((channel[FLAGS] & SAFETY) || !(channel[FLAGS] & MODE_CLAW))
    {
        serialOut(129,17,8);
        shutdownMotors();
        if (isBetween(datachan[0], 80, 100)) channel[FLAGS] &= ~SAFETY;
        else channel[FLAGS] |= SAFETY;
        channel[FLAGS] = (channel[FLAGS] & ~MODE_MASK) | MODE_CLAW;
    }
    else
    {
        ChannelData wristrotation, wristpitch, elbow, claw;
        wristrotation.data = datachan[CHANNEL4];
        wristrotation.max = CHANNEL4_MAX;
        wristrotation.range = CHANNEL4_RANGE;
        wristrotation.lower_dead = CHANNEL4_LOWERDEAD;
        wristrotation.upper_dead = CHANNEL4_UPPERDEAD;
        wristrotation.multiplier1 = 2;
        wristrotation.multiplier2 = 1.4;
        wristpitch.data = datachan[CHANNEL3];
        wristpitch.max = CHANNEL3_MAX;
        wristpitch.range = CHANNEL3_RANGE;
        wristpitch.lower_dead = CHANNEL3_LOWERDEAD;
        wristpitch.upper_dead = CHANNEL3_UPPERDEAD;
        wristpitch.multiplier1 = 1.07;
        wristpitch.multiplier2 = 2;
        elbow.data = datachan[CHANNEL2];
        elbow.max = CHANNEL2_MAX;
        elbow.range = CHANNEL2_RANGE;
        elbow.lower_dead = CHANNEL2_LOWERDEAD;
        elbow.upper_dead = CHANNEL2_UPPERDEAD;
        elbow.multiplier1 = 2;
        elbow.multiplier2 = 1.4;
        claw.data = datachan[CHANNEL1];
        claw.max = CHANNEL1_MAX;
        claw.range = CHANNEL1_RANGE;
        claw.lower_dead = CHANNEL1_LOWERDEAD;
        claw.upper_dead = CHANNEL1_UPPERDEAD;
        claw.multiplier1 = .78;
        claw.multiplier2 = .72;
        
        driveWristRotation(wristrotation);
        driveWrist(wristpitch);
        driveElbow(elbow);
        driveClaw(claw);


//        serialOut(131, 7, datachan[3]);
    }
}

void driveWithArdu()
{
    if (rx_channel_data[CHANNEL1] < ACHANNEL1_MIN || rx_channel_data[CHANNEL3] < ACHANNEL3_MIN) return;
    uint ch1 = rx_channel_data[CHANNEL1] - ACHANNEL1_MIN;
    uint ch3 = rx_channel_data[CHANNEL3] - ACHANNEL3_MIN;
    if (!isBetween(ch3,ACHANNEL3_LOWERDEAD, ACHANNEL3_UPPERDEAD))
    {
        if (isBetween(ch1, ACHANNEL1_LOWERDEAD, ACHANNEL1_UPPERDEAD))
        {   // no turn
            serialOut(129, 6, ch3);
            serialOut(129, 7, ch3);
            serialOut(128, 6, ch3);
            serialOut(128, 7, ch3);
        } else { // turn
            if (ch1 < ACHANNEL3_LOWERDEAD)
            { // turn right
                ch1 = (63 - ch1);
                if (ch3 < ACHANNEL3_LOWERDEAD) 
                {
                    ch3 = 63 - ch3;
                    serialOut(129,MIXED_REVERSE, ch1+ch3);
                    serialOut(129,MIXED_FORWARD, ch1+ch3);
                }
                else
                {
                    ch3 = ch3 - 63;
                    serialOut(129,MIXED_FORWARD, ch1+ch3);
                    serialOut(129,MIXED_REVERSE, ch1+ch3);
                }
            }
            else
            { // turn left
                ch1 = ch1 - 63;
                if (ch3 < ACHANNEL3_LOWERDEAD) 
                {
                    ch3 = 63 - ch3;
                    serialOut(129,MIXED_FORWARD, ch1+ch3);
                    serialOut(129,MIXED_REVERSE, ch1+ch3);
                }
                else
                {
                    ch3 = ch3 - 63;
                    serialOut(129,MIXED_REVERSE, ch1+ch3);
                    serialOut(129,MIXED_FORWARD, ch1+ch3);
                }
            }
        }
    } else shutdownMotors();
}

#endif


#ifdef _BLUE
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
#endif

#ifdef _BOARD
/***********************************
* Top board uses UART1
* Bottom board uses UART2
***********************************/

//Top board functions

#endif



void initialize(uint* channel)
{
    asm volatile("DI");     // disable interrupts
    
    INTCON = 0x1000;        // turn on multi vector mode, change external internal interrupts leading edge
    
    // Create and initialize 6 data channels
    int i = 0;
    for (;i < 6; i++) channel[i] = 0;
    channel[FLAGS] = SAFETY;    // Turn on safety

    // Set Up Timer - SMCLK, continuous up mode, clock divisions 1
    T2CON = 0x78;       // 256 divider, 32 bit
    T2CON = 0;
    PR2 = 0xFFFF;
    TMR2 = 0;
    
#ifdef _TIMER
    IEC0SET = 1 << 12;
    IPC3SET = 6 << 2;
#endif
    
    //set up dragonlink input
#ifdef _DL
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
#endif
    
    // Configure Pins
    TRISBCLR = 0x3C00;      // LEDs
    TRISFCLR = 0x100;

    // Default all pins low
    
    //Setup UART2
#ifdef _BOARD
    U2STA = 0x14C0;
    IEC1SET = 1 << 9;
    IPC8SET = 6 << 2;
#else
    U2STA = 0x400;
#endif
    U2BRG = 259;
    

    //CONFIGURING UART1    
    U1STA = 0x400;
    U1BRG = 259;
    
    //setup bluetooth
#ifdef _BLUE
    U2STASET = 0x1040;
    IEC1SET = 1 << 9;
    IFS1CLR = 1 << 9;
    IPC8SET = 6 << 2;
#endif
    U1MODE = 0x8000;    // start UART1
    U2MODE = 0x8000;    // start UART2
    
    //set up LCD
#ifdef _LCD
    for (i = 0; i < 20000; i++) { asm volatile("NOP"); }
    char temp[] = { 0x1B, '[', 'j', 0x1B, '[', '0', 'h', 0x1B, '[', '0', ';', '0', 'H' };
    dispLCD(temp, 13);
#endif

    //set up motors
#ifdef _MOTORS
    //configuring serial timeout (command 14) and dead bands (command 17) (dead bands will persist, and must be reset to default through this same procedure)
    serialOut(128,14,1);  //serial timout command (1=100ms)  (I thought this would disable the motor controller when modes were switched, but it didn't; maybe commands to other drivers count as commands)
    serialOut(128,17,10);  //configure dead band to 10 (default is 3) (this motor controller has channel 1 on it)

    serialOut(129,14,1);
    //dead bands for 129 (motor controller 1) are now configured in the safety "else" statements

    serialOut(130,14,1);
    serialOut(130,17,10);   //configure dead band to 10 (this motor controller has channel 1 on it)

    serialOut(131,14,1);
    serialOut(131,17,8);

    shutdownMotors();
#endif
    
    asm volatile("EI"); // enable interrupts
    T2CON = 0x8078;     // start timer
    
}

#ifdef _BOARD
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
#endif

/***************
 ***Interrupts***** 
 ***************/
#ifdef _TIMER
void __ISR(_TIMER_2_VECTOR, IPL6SOFT) Timer23InterruptHandler(void)
{
    asm volatile("DI");
    IFS0CLR = 1 << 12;
    static int i = 0;
    for (i = 0; i < 4; i++)
        if (channel_data[i] == last_blue[i]) channel_data[i] = 0;
    
    asm volatile("EI");
}
#endif

#ifdef _DL
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
#endif

#ifdef _BLUE
void __ISR(_UART_2_VECTOR, IPL6SOFT) BluetoothInterruptHandler(void)
{
    asm volatile("DI");
    IFS1CLR = 1 << 9;
    last_blue = U2RXREG;
    is_different = 1;
    if (U2STA & 2) U2STACLR = 2;   // clear overrun if it occurred
    asm volatile("EI");
}
#endif

#ifdef _BOARD
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
#endif
