#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "solenoid.h"
#include "clock.h"

#define Solenoid PORTA,2

uint8_t solenoidFlag = 0;

void initSolenoidHw() {

    initSystemClockTo40Mhz();
    enablePort(PORTA);
    selectPinPushPullOutput(Solenoid);
}
void engageSolenoid() {
    setPinValue(Solenoid, 0);
    solenoidFlag = 1;
}
void disengageSolenoid() {
    setPinValue(Solenoid, 1);
    solenoidFlag = 0;
}
uint8_t getSolenoidState() {
    return solenoidFlag;
}
