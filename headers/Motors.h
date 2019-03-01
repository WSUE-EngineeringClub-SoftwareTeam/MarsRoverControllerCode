#include "Rover.h"

void setupMotors(void);
void setMoveMode(uint*);
void setArmMode(uint*);
void setClawMode(uint*);
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

void moveForward(int speed);
void moveBackward(int speed)
void turnRight(int speed);
void turnLeft(int speed);

void boomMoveUp(int speed);
void boomMoveDown(int speed);
void boomMoveLeft(int speed);
void boomMoveRight(int speed);

void wristMoveUp(int speed);
void wristMoveDown(int speed);

void clawOpen(int speed);
void clawClose(int speed);

void elbowUp(int speed);
void elbowDown(int speed);