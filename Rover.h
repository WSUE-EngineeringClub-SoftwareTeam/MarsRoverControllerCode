/*
 * RoverHeader.h
 *
 *  Created on: Jan 25, 2017
 *      Author: Mark Walsh
 */

#ifndef ROVERHEADER_H_
#define ROVERHEADER_H_

/*
 Description                Usage               Wire Color      Board Location
 ------------------------------------------------------------------------------
 Channel 1                  Throttle            Orange          JH - RD 08
 Channel 2                  Aileron             Pink            JD - RD 09
 Channel 3                  Elevator            Green           JD - RD 10
 Channel 4                  Rudder              Blue            JE - RD 11
 Channel 5                  Mode                Yellow          JK - RD 12
 Data Out                   Motor Data          White           JE - RF 08
 Ground                     Case Ground         Dark Green      Main Ground
 Ground                     Motor Ground        Green (bare)    JE - GND
 UART2                      LCD Display         N/A             JH Top
 Power                      Controller Power    Red             Main Power
 * 
 * 
 * 
 * Motor Controllers
 * -----------------
 * Channel      Command     Control
 * --------------------------------
 * 128          9,10,11,12  Rover
 * 129          6           Boom
 * 129          7           Base Rotation
 * 130          6           Claw Open/Close
 * 130          7           Elbow
 * 131          6           Wrist Pitch
 * 131          7           Wrist Rotation
 * 
 * Remote Movement
 * Movement     Channel
 * --------------------
 * L Up/Down    1
 * L Left/Right 4
 * R Up/Down    3
 * R Left/Right 2        
 * 
 */

// short hand for native data types
#define uint unsigned int
#define uchar unsigned char

#define MAX_INT 0xFFFFFFFF

// channel numbers for channel array
#define CHANNEL1    0
#define CHANNEL2    1
#define CHANNEL3    2
#define CHANNEL4    3
#define CHANNEL5    4
#define CHANNEL6    5
#define FLAGS       6

// channel flags
#define SAFETY      1
#define MODE_MOVE   2
#define MODE_ARM    4
#define MODE_CLAW   8
#define MODE_MASK   0xE

//LCD Information
#define ESC         0x1B

//Channel Adjustment values
#define CHANNEL1_MIN        2797
#define CHANNEL1_MAX        2983
#define CHANNEL1_RANGE      187
#define CHANNEL1_LOWERDEAD  80
#define CHANNEL1_UPPERDEAD  100

#define CHANNEL2_MIN        2846
#define CHANNEL2_MAX        2933
#define CHANNEL2_RANGE      87
#define CHANNEL2_LOWERDEAD  40
#define CHANNEL2_UPPERDEAD  48

#define CHANNEL3_MIN        2828
#define CHANNEL3_MAX        2953
#define CHANNEL3_RANGE      94
#define CHANNEL3_LOWERDEAD  58
#define CHANNEL3_UPPERDEAD  66

#define CHANNEL4_MIN        2840
#define CHANNEL4_MAX        2939
#define CHANNEL4_RANGE      97
#define CHANNEL4_LOWERDEAD  44
#define CHANNEL4_UPPERDEAD  52

#define ACHANNEL1_MIN       2828
#define ACHANNEL1_MAX       2953
#define ACHANNEL1_RANGE     125
#define ACHANNEL1_LOWERDEAD 61
#define ACHANNEL1_UPPERDEAD 65

#define ACHANNEL3_MIN       2828
#define ACHANNEL3_MAX       2953
#define ACHANNEL3_RANGE     125
#define ACHANNEL3_LOWERDEAD 61
#define ACHANNEL3_UPPERDEAD 65

#define MODE0               2828
#define MODE1               2890
#define MODE2               2952
#define MODE4               3014
#define MODE5               9999

#define MIXED_REVERSE       9
#define MIXED_FORWARD       8
#define CH1_REVERSE         

#define ARDU_DATA_SIZE 4

typedef struct _ChannelData
{
    uint data;
    uint max;
    uint range;
    uint lower_dead;
    uint upper_dead;
    float multiplier1;
    float multiplier2;
} ChannelData;

typedef struct _ArduInputData
{
    uchar mode;
    uint data[4];
} ArduInputData;

typedef struct _MotorControlStatus
{
    uchar channel;
    uchar status;
} MotorControlStatus;

// Communications Protocol
#define CHANNEL6_HEADER     0xFF
#define CHANNEL7_HEADER     0xEE
#define CHANNEL8_HEADER     0xCC



#endif /* ROVERHEADER_H_ */
