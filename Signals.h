#ifndef SIGNALS_H_
#define SIGNALS_H_


/*
 * Defines the bluetooth singals that the rover accepts
 * General format: PART_DIRECTION
 * With the exepction of speed and shutdown
 */


#define BOOM_LEFT 'W'
#define BOOM_RIGHT 'C'
#define BOOM_UP 'B'
#define BOOM_DOWN 'b'


#define ROVER_FORWARD 'U'
#define ROVER_BACKWARD 'D'
#define ROVER_LEFT 'L'
#define ROVER_RIGHT 'R'
#define ROVER_SHUTDOWN 'S'

#define WRIST_UP 'P'
#define WRIST_DOWN 'p'
#define WRIST_LEFT 'V'
#define WRIST_RIGHT 'v'

#define CLAW_OPEN 'A'
#define CLAW_CLOSE 'a'

#define ELBOW_UP 'E'
#define ELBOW_DOWN 'e'

#define SPEED_UP 'X'
#define SPEED_DOWN 'x'

#endif