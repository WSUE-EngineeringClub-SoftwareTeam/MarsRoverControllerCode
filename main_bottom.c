//#include <stdio.h>
#include "p32xxxx.h"
#include "plib.h"
#include "Rover.h"
#include "Singals.h"

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
    #include "LCD_display.h"
#endif

#ifdef _BLUE
    #include "Bluetooth.h"
#endif

#ifdef _DL
    #include "DragonLink.h"
#endif

#ifdef _MOTORS
    #include "Motors.h"
#endif

#ifdef _BOARD
    #include "Board.h"
#endif


/***************
 ***Globals***** 
 ***************/
uint buffer;
uint channel_start[6];
uint channel_data[7];
const uchar mask[5] = { 0x80, 0xC0, 0xA0, 0x40, 0x60 };
uchar char_buffer;
uchar last_bluetooth_cmd;
uchar is_different = 0;
uint rx_channel_data[6];

/*************************
 *** Component Flags ***** 
 *************************/
int LCD_enabled = FALSE;
int Bluetooth_enabled = FALSE;
int Motors_enabled = FALSE;
int DragonLink_enabled = FALSE;
int Board_enabled = FALSE;
int Time_enabled = FALSE;

/*****************
 *** Movement Speeds  ***** 
 *****************/
int speed_boom_up    = 31;
int speed_boom_down  = 31;
int speed_boom_left  = 31;
int speed_boom_right = 31;

int speed_wrist_up    = 31;
int speed_wrist_down  = 31;
int speed_wrist_left  = 31;
int speed_wrist_right = 31;

int speed_rover_forward    = 31;
int speed_rover_backward   = 31;
int speed_rover_turn_left  = 31;
int speed_rover_turn_right = 31;

int speed_claw_open  = 31;
int speed_claw_close = 31;

int speed_elbow_up   = 31;
int speed_elbow_down = 31;
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
 *** Main ******
 ***************/
void main(void)
{
    int i,j;
    int x = 0, y = 0;
    initialize(channel_data);

    if(LCD_enabled){
        clearLCD();
    }

    if(DragonLink_enabled){
        uchar last_mode = getMode(channel_data[CHANNEL5]), current_mode, the_mode;
    }
    while (1)
    {
        if(DragonLink_enabled)
        {
            current_mode = getMode(channel_data[CHANNEL5]);
            if (last_mode == current_mode)
            {
                the_mode = current_mode;
            } 
            else
            {
                the_mode = last_mode;
            } 
            switch(the_mode)
            {
                case 0: setMoveMode(channel_data, DragonLink_enabled); break;
                case 1: setArmMode(channel_data, DragonLink_enabled); break;
                case 2: setClawMode(channel_data, DragonLink_enabled); break;
            }
            last_mode = current_mode;
        }

        if(LCD_enabled)
        {
            /*for (i = 0; i < 4; i++) {
                //for (j = 0; j < 200; j++);
                showNumber(channel_data[i], i);
            }*/        
        }

        if(Bluetooth_enabled)
        {
            if (is_different) {
                switch(last_bluetooth_cmd) {
                    case BOOM_LEFT: boomMoveLeft(speed_boom_left); break    // moves boom left
                    
                    case BOOM_RIGHT: boomMoveRight(speed_boom_right); break  // Moves boom right        
                    
                    case ROVER_SHUTDOWN: shutdownMotors(); break;                   // Stop driving
                    
                    case ROVER_FORWARD: moveForward(speed_rover_forward); break;   // Drive forward                    
                    
                    case ROVER_RIGHT: turnRight(speed_rover_turn_right); break;     // Turn Right
                    
                    case ROVER_LEFT: turnLeft(speed_rover_turn_left); break;       // Turn Left
                    
                    case ROVER_BACKWARD: moveBackward(speed_rover_backward); break;    // Back up
                                                  
                    case BOOM_UP: boomMoveUp(speed_boom_up); break;       // Move Boom up
                   
                    case BOOM_DOWN: boomMoveDown(speed_boom_down); break;   // Boom Down

                    case WRIST_UP: wristMoveUp(speed_wrist_up); break;     // Wrist Up
                    
                    case WRIST_DOWN: wristMoveDown(speed_wrist_down); break; // Wrist Down
                    
                    case CLAW_OPEN: clawOpen(speed_claw_open); break;   // Claw Open
                    
                    case CLAW_CLOSE: clawClose(speed_claw_close); break; // Claw Close 
                     
                    case ELBOW_UP:  elbowUp(speed_elbow_up); break;     // Elbow up
                    
                    case ELBOW_DOWN: elbowDown(speed_elbow_down); break;        // Elbow down
                    
                    case WRIST_LEFT: wristMoveLeft(speed_wrist_left); break;    // Wrist Left

                    case WRIST_RIGHT: wristMoveRight(speed_wrist_right); break;  // Wrist Right 
                     
                    case SPEED_UP: x++; break; // Speed up
                    
                    case SPEED_DOWN: x--; break; // Speed down
                    
                    default: shutdownMotors();
                }
                if (x > 31) x == 31;
                if (x < -31) x = -31;
                /*y = x;
                if (y < 0) { sendBluetoothChar('-'); y *= -1; }
                sendBluetoothChar((y/10)+48);
                sendBluetoothChar((y%10)+48);*/ 
                //showNumber(0, 0);
                //sendLCDData(last_bluetooth_cmd);
                is_different = 0;
            }
        }

        if(Board_enabled)
        {
            //for (i = 0; i < 4; i++) sendBoardData(i);
            //j = 0;
            driveWithArdu();
        }
    }
}

/*******************
 *** Functions ***** 
 *******************/
void setLeds(uchar num)
{
    LATB = (PORTB & 0xFFFFC3FF) | ((num & 0xF) << 10);
}

BOOL isBetween(uint num, uint num1, uint num2)
{
    if (num >= num1 && num <= num2)
    {
        return TRUE;
    }
    return FALSE;
}

void initialize(uint* channel)
{
    asm volatile("DI");     // disable interrupts
    
    INTCON = 0x1000;        // turn on multi vector mode, change external internal interrupts leading edge
    
    // Create and initialize 6 data channels
    int numOfDataChannels = 6;
    int i;
    for (i = 0;i < numOfDataChannels; i++)
    {
        channel[i] = 0;
    } 
    channel[FLAGS] = SAFETY;    // Turn on safety

    // Set Up Timer - SMCLK, continuous up mode, clock divisions 1
    T2CON = 0x78;       // 256 divider, 32 bit
    T2CON = 0;
    PR2 = 0xFFFF;
    TMR2 = 0;

// Set up Timer
#ifdef _TIMER
    Time_enabled = TRUE;
    setupTimer();
#endif
    
// Set up dragonlink input
#ifdef _DL
    DragonLink_enabled = TRUE;
    setupDragonLink();
#endif
    
    // Configure Pins
    TRISBCLR = 0x3C00;      // LEDs
    TRISFCLR = 0x100;

    // Default all pins low
    
// Setup UART2
#ifdef _BOARD 
    Board_enabled = TRUE;
    setupBoard_UART2();
#else
    // Not sure if this is actually setting up UART 1
    // if determined that it's not please change 
    Board_enabled = TRUE;
    setupBoard_UART1(); 
#endif

    U2BRG = 259;

    //CONFIGURING UART1    
    U1STA = 0x400;
    U1BRG = 259;
    
// setup bluetooth
#ifdef _BLUE
    Bluetooth_enabled = TRUE;
    setupBluetooth();
#endif

    U1MODE = 0x8000;    // start UART1
    U2MODE = 0x8000;    // start UART2
    
// setup LCD
#ifdef _LCD
    LCD_enabled = TRUE;
    setupLCD;
#endif

// setup motors
#ifdef _MOTORS
    Motors_enabled = TRUE;
    setupMotors();
#endif
   
    asm volatile("EI"); // enable interrupts
    T2CON = 0x8078;     // start timer
}