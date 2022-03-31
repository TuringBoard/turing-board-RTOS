
#ifndef TURNINGMECHANISMTHREAD_H_
#define TURNINGMECHANISMTHREAD_H_

typedef struct _turningMechanismData {
    uint8_t id;
    uint8_t direction;
    uint8_t angle;
    uint8_t rate;
} turningMechanismData;

#pragma DATA_SECTION(turnMecData, ".tbglob")
#pragma DATA_SECTION(turnMechDataReceivedFlag, ".tbglob")
#pragma DATA_SECTION(currentAngle, ".tbglob")

extern uint8_t turnMechDataReceivedFlag;
extern turningMechanismData turnMecData;
extern uint8_t currentAngle;

void initTurningMechanismThreadHw();
void microStep();
void fullStep();
void setDirection(uint8_t dir);
void setStepFormat(uint8_t stepMode);
void turningMechanism();

#endif
