
#ifndef TURNINGMECHANISMTHREAD_H_
#define TURNINGMECHANISMTHREAD_H_

typedef struct _turningMechanismData {
    uint8_t id;
    uint8_t direction;
    uint8_t angle;
    uint8_t solenoidFlag;
} turningMechanismData;

#pragma DATA_SECTION(turnMecData, ".tbglob")
#pragma DATA_SECTION(turnMechDataReceivedFlag, ".tbglob")
#pragma DATA_SECTION(currentAngle, ".tbglob")
#pragma DATA_SECTION(motorOnFlag, ".tbglob")
#pragma DATA_SECTION(angleOffset, ".tbglob")
#pragma DATA_SECTION(turningMechOnFlag, ".tbglob")

extern uint8_t turnMechDataReceivedFlag;
extern turningMechanismData turnMecData;
extern uint8_t currentAngle;
extern uint8_t motorOnFlag;
extern uint8_t angleOffset;
extern bool turningMechOnFlag;


void initTurningMechanismThreadHw();
void microStep();
void fullStep();
void setDirection(uint8_t dir);
void setStepFormat(uint8_t stepMode);
void turningMechanism();
void turnOnMotor();
void turnOffMotor();
void goHome();
void goHomePlusOne();
bool motorIsOn();
void moveMotor();
void turn(uint8_t dir);
bool turningMechanismIsOn();
void changeTurningMechanismOnFlag(bool state);

#endif
