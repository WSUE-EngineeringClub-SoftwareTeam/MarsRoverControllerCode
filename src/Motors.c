#include "Motors.h"
#include "p32xxxx.h"
#include "plib.h"


void shutdownMotors(void)
{
    serialOut(ROVER_CONTROL,0,0);       //command stop motor 1
    serialOut(ROVER_CONTROL,4,0);       //command stop motor 2
    
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
    while ((U1STA & 0x200)) 
    { 
        LATB = 0x2000; 
        wait++; 
    }
    LATB = 0;

    // Times out
    if (wait == 20000){
        return 0;
    } 
    // Transmit the passed in data
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
    if (isBetween(wrist.data, wrist.lower_dead, wrist.upper_dead)) 
    {
        wrist.data = 64;
    }
    else
    {
        if (wrist.data > wrist.upper_dead) 
        {
            wrist.data = (wrist.range-wrist.data)*wrist.multiplier2;
        }
        else
        {
            wrist.data = ((wrist.lower_dead-wrist.data)*wrist.multiplier1)+64;
        } 
    }
    serialOut(132, 7, wrist.data);

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
            serialOut(ROVER_CONTROL, 6, ch3);
            serialOut(ROVER_CONTROL, 7, ch3);
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

void setMoveMode(uint* channel, int DragonLink_enabled)
{
    static uint datachan[5];
    if(DragonLink_enabled){
        getDatachan(channel, datachan);

    }else
    {
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
}

void setArmMode(uint* channel, int DragonLink_enabled)
{
    static uint datachan[5];
    if(DragonLink_enabled)
    {
        getDatachan(channel, datachan);
    }

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


void setClawMode(uint* channel, int DragonLink_enabled)
{
    static uint datachan[5];
    // static uint checksum;
    if(DragonLink_enabled)
    {
        getDatachan(channel, datachan);
    }
    // serialOut(131, 7, datachan[3]);
}

void setupMotors(void)
{
    // configuring serial timeout (command 14) and dead bands (command 17) 
    // (dead bands will persist, and must be reset to default through this same procedure)
    serialOut(ROVER_CONTROL,14,1);  //serial timout command (1=100ms)  (I thought this would disable the motor controller when modes were switched, but it didn't; maybe commands to other drivers count as commands)
    serialOut(ROVER_CONTROL,17,10);  //configure dead band to 10 (default is 3) (this motor controller has channel 1 on it)

    serialOut(129,14,1);
    //dead bands for 129 (motor controller 1) are now configured in the safety "else" statements

    serialOut(130,14,1);
    serialOut(130,17,10);   //configure dead band to 10 (this motor controller has channel 1 on it)

    serialOut(131,14,1);
    serialOut(131,17,8);

    shutdownMotors();
}

/**********************************
 *** Rover moving functions *******
 **********************************/

/*
 *  This function is based on the orginal code
 *  that was written. 
 */
void moveForward(int speed)
{
    serialOut(ROVER_CONTROL, 6, 96 + speed); 
    serialOut(ROVER_CONTROL, 7, 32 - speed); 
    // Not sure which control is which but 
    // they have the same value so it doesn't
    // change the function
    serialOut(BOOM_CONTROL, 6, 32 - speed); 
    serialOut(BASE_CONTROL, 7, 96 + speed);
}

/*
 *  This function is based on the orginal code
 *  that was written. 
 */
void moveBackward(int speed)
{
    serialOut(ROVER_CONTROL, 6, 32 - speed); 
    serialOut(ROVER_CONTROL, 7, 96 + speed); 
    // Not sure which control is which but 
    // they have the same value so it doesn't
    // change the function
    serialOut(129, 6, 96 + speed); 
    serialOut(129, 7, 32 - speed);
}

void turnRight(int speed)
{
    serialOut(ROVER_CONTROL, 6, 32 - speed); 
    serialOut(ROVER_CONTROL, 7, 96 + speed); 
    // Not sure which control is which but 
    // they have the same value so it doesn't
    // change the function
    serialOut(BOOM_CONTROL, 6, 32 - speed); 
    serialOut(BASE_CONTROL, 7, 96 + speed);
}

void turnLeft(int speed)
{
    serialOut(ROVER_CONTROL, 6, 96 + speed); 
    serialOut(ROVER_CONTROL, 7, 32 - speed); 
    // Not sure which control is which but 
    // they have the same value so it doesn't
    // change the function
    serialOut(BOOM_CONTROL, 6, 96 + speed); 
    serialOut(BASE_CONTROL, 7, 32 - speed);
}

/*************************
 *** Boom functions ******
 *************************/
void boomMoveRight(int speed)
{
    serialOut(WRIST_PITCH_CONTROL, 6, 32 - speed);
}

void boomMoveLeft(int speed)
{
    serialOut(WRIST_PITCH_CONTROL, 6, 96 + speed);
}

void boomMoveUp(int speed)
{
    serialOut(131, 7, 32 - speed);
}

void boomMoveDown(int speed)
{
    serialOut(131, 7, 96 + speed);
}

/**************************
 *** wrist functions ******
 **************************/
void wristMoveUp(int speed)
{
    serialOut(132, 6, 32 - speed);
}

void wristMoveDown(int speed)
{
    serialOut(132, 6, 96 + speed);
}

void wristMoveLeft(int speed)
{
    serialOut(132, 7, 32 - speed);
}

void wristMoveRight(int speed)
{
    serialOut(132, 7, 96 + speed);
}

/**************************
 *** claw functions ******
 **************************/
void clawOpen(int speed)
{
    serialOut(130, 7, 32 - speed); 
}

void clawClose(int speed)
{
    serialOut(130, 7, 96 + speed); 
}

/**************************
 *** elbow functions ******
 **************************/
void elbowUp(int speed)
{
    serialOut(130, 6, 32 - speed);
}

void elbowDown(int speed)
{
    serialOut(130, 6, 96 + speed);
}
