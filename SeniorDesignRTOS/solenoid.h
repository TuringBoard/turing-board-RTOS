#ifndef SOLENOID_H_
#define SOLENOID_H_

#pragma DATA_SECTION(solenoidFlag, ".tbglob")
extern uint8_t solenoidFlag;

void initSolenoidHw();
void engageSolenoid();
void disengageSolenoid();
uint8_t getSolenoidState();

#endif
