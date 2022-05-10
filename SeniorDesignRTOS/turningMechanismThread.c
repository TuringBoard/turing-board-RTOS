// Using DRV8825
// Keaton Koehler

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#define DEBUG
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include "gpio.h"
#include "wait.h"
#include "clock.h"
#include "svcCalls.h"
#include "turningMechanismThread.h"
#include "solenoid.h"


// PortF masks
#define DIR   PORTB,6        //Direction blue
#define STEP  PORTB,7        //Step      white
#define SLEEP PORTA,4        //SLEEP
//#define ENABLE  PORTD,0      //enable
#define PUSH_BUTTON PORTF,4
#define M0 PORTA,5
#define M1 PORTA,6
#define M2 PORTA,7
#define RED_LED PORTF,1
//#define GREEN_LED PORTF,3
//#define nSleep PORTE,2

#define FULLSTEP 1.8
#define ONEFOURTH 0.45
#define ONEEIGHTH 0.225
#define ONESIXTIETH 0.1125
#define ONETHIRTYTWOTH 0.05625


turningMechanismData turnMecData;
uint8_t turnMechDataReceivedFlag = 0;
uint8_t currentAngle = 50;
uint8_t motorOnFlag = 0;
uint8_t angleOffset = 6;
bool turningMechOnFlag = false;

#define WAIT 55000

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void setStepFormat(uint8_t stepMode) {
    switch(stepMode) {
        case 0:
            setPinValue(M0,0);
            setPinValue(M1,0);
            setPinValue(M2,0);
//            waitTime = 1000;
            break;
        case 1:
            setPinValue(M0,1);
            setPinValue(M1,0);
            setPinValue(M2,0);
            break;
        case 2:
            setPinValue(M0,0);
            setPinValue(M1,1);
            setPinValue(M2,0);
            break;
        case 3:
            setPinValue(M0,1);
            setPinValue(M1,1);
            setPinValue(M2,0);
//            waitTime = 500;
            break;
        case 4:
            setPinValue(M0,0);
            setPinValue(M1,0);
            setPinValue(M2,1);
//            waitTime = 150;
            break;
        case 5:
            setPinValue(M0,1);
            setPinValue(M1,0);
            setPinValue(M2,1);
            break;
        case 6:
            setPinValue(M0,0);
            setPinValue(M1,1);
            setPinValue(M2,1);
            break;
        case 7:
            setPinValue(M0,1);
            setPinValue(M1,1);
            setPinValue(M2,1);
//            waitTime = 500;
            break;
    }
}

// Initialize Hardware
uint32_t waitTime = 300;
void initTurningMechanismThreadHw() {
    initSystemClockTo40Mhz();
    enablePort(PORTA);
    enablePort(PORTC);
    enablePort(PORTF);
    enablePort(PORTB);
    selectPinDigitalInput(PUSH_BUTTON);
    enablePinPullup(PUSH_BUTTON);
    selectPinPushPullOutput(DIR);
    selectPinPushPullOutput(STEP);
    selectPinPushPullOutput(SLEEP);
    selectPinPushPullOutput(M0);
    selectPinPushPullOutput(M1);
    selectPinPushPullOutput(M2);
    selectPinPushPullOutput(RED_LED);

    setStepFormat(0);
}
void microStep() {
    uint8_t value = (uint8_t)getPinValue(STEP);
    value ^= 1;
    setPinValue(STEP, value);
    waitMicrosecond(500);
}
/*
void fullStep() {
    uint8_t value = (uint8_t)getPinValue(STEP);
    value ^= 1;
    setPinValue(STEP, value);
    waitMicrosecond(30000);
}
*/
bool turningMechanismIsOn() {
    return turningMechOnFlag;
}
void changeTurningMechanismOnFlag(bool state) {
    turningMechOnFlag = state;
}
bool motorIsOn(){
    return motorOnFlag == 1;
}
void turnOnMotor() {
    setPinValue(SLEEP, 1);
    motorOnFlag = 1;
}
void turnOffMotor() {
    setPinValue(SLEEP, 0);
    motorOnFlag = 0;
}
void checkCurrentAngle(uint8_t angle) {
    if(turningMechanismIsOn()) {
        if(!getSolenoidState() == 0 && angle < 60 && angle > 40) {
            disengageSolenoid(); //Pull it out
        }
        else if(!getSolenoidState() == 1){
            engageSolenoid(); //Let it go
        }
    }
}
void setDirection(uint8_t dir) {
//    uint8_t direction = (uint8_t)getPinValue(DIR);
////    direction ^= 1;
//    if(direction == 1)
//        direction = 0;
//    else
//        direction = 1;
    setPinValue(DIR, dir);
    waitMicrosecond(1);
}
void goHome() {
    int16_t newDirection = currentAngle - 50;
    uint8_t direction;
    if(newDirection > 0) {
        direction = 0;
    }
    else {
        direction = 1;
    }
    setDirection(direction);
    uint8_t angle = 50;
    uint32_t degrees; //= (angle/ONEEIGHTH);// * 4;
    if(currentAngle > angle)
        degrees = currentAngle - angle;
    else
        degrees = angle - currentAngle;
    uint16_t temp = currentAngle;
    degrees = (degrees/FULLSTEP) * 4;
    if(getSolenoidState() == 1) {
        disengageSolenoid();
        waitMicrosecond(2000);
    }
    uint8_t counter = 0;
    uint16_t i;
    for(i = 0; i < degrees; i++) {
        microStep();
        counter++;
        if(counter == 2) {
            if(direction == 1)
                temp++;
            else
                temp--;
            counter = 0;
        }
        checkCurrentAngle(temp);
        yield();
    }
    if(getSolenoidState() == 0)
        engageSolenoid();
    currentAngle = 50;
}
void goHomePlusOne() {
    int16_t directionAngle = currentAngle - 50;
    uint8_t direction;
    if(directionAngle > 0) {
        direction = 0;
    }
    else {
        direction = 1;
    }
    setDirection(direction);
    uint8_t angle = 50;
    uint32_t degrees; //= (angle/ONEEIGHTH);// * 4;
    if(currentAngle > angle)
        degrees = currentAngle - angle;
    else
        degrees = angle - currentAngle;
    degrees = (degrees/FULLSTEP) * 4;
    degrees += angleOffset;
    uint16_t i;
    for(i = 0; i < degrees; i++) {
        microStep();
        yield();
    }
    currentAngle = 50;
}
void moveMotor() {
    setDirection(1);
    uint8_t angle = 50;
    uint32_t degrees; //= (angle/ONEEIGHTH);// * 4;
    if(currentAngle > angle)
        degrees = currentAngle - angle;
    else
    degrees = angleOffset;
    degrees = (degrees/FULLSTEP) * 4;
    uint16_t i;
    for(i = 0; i < degrees; i++) {
        microStep();
        yield();
    }
    currentAngle = 50 + angleOffset;
//    currentAngle = 50 - degrees;
}

void turn(uint8_t dir) {
    setDirection(dir);
    uint8_t angle = turnMecData.angle;
    uint32_t degrees; //= (angle/ONEEIGHTH);// * 4;
    if(currentAngle > angle) //going right
        degrees = currentAngle - angle;
    else //going left
        degrees = angle - currentAngle;
    uint16_t temp = currentAngle;
//    if(dir == 0)
//        degrees += angleOffset;
    degrees = (degrees/FULLSTEP) * 4;
    if(getSolenoidState() == 1) {
        disengageSolenoid();
        waitMicrosecond(2000);
    }
    uint8_t counter = 0;
    uint16_t i;
    for(i = 0; i < degrees; i++) {
        microStep();
        counter++;
        if(counter == 2) {
            if(dir == 1)
                temp++;
            else
                temp--;
            counter = 0;
        }
        checkCurrentAngle(temp);
        yield();
    }
    if(getSolenoidState() == 0)
        engageSolenoid();
//    if(dir == 0)
//        angle += angleOffset;
    currentAngle = angle;
}
void turningMechanism() {
    while(1) {
        if(turnMechDataReceivedFlag) {
            //calculate steps
            //set direction
            //loop for calculated steps
                //step
                //update currentAngle
                //yield

//            setPinValue(PORTF,1,1);
//            waitMicrosecond(200000);
//            setPinValue(PORTF,1,0);
//            setPinValue(SLEEP, 1);
            //1 - right
            //0 - left
            uint8_t dir = turnMecData.direction;
            uint8_t solenoidF = turnMecData.solenoidFlag;
            if((dir == 1 || dir == 0) && solenoidF == 0 && motorIsOn()) {
                turn(dir);
            }
            else if(dir == 2 && solenoidF == 0 && motorIsOn()) {
                goHome();
            }
            else if(dir == 3 && solenoidF == 1) {
                engageSolenoid(); //Let it go
                waitMicrosecond(20000);
                goHomePlusOne();
                turnOffMotor();
            }
            else if(dir == 3 && solenoidF == 0) {
                disengageSolenoid();
                changeTurningMechanismOnFlag(true);
                waitMicrosecond(20000);
                turnOnMotor();
//                moveMotor();
//                engageSolenoid();
            }
            turnMechDataReceivedFlag = 0;
//            setPinValue(SLEEP, 0);
        }
        yield();
    }
}
