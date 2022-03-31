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
            setPinValue(SLEEP, 1);
            //1 - right
            //0 - left
            uint8_t dir = turnMecData.direction;
            if(dir == 1 || dir == 0) {
                setDirection(dir);
                uint8_t angle = turnMecData.angle;
                uint32_t degrees; //= (angle/ONEEIGHTH);// * 4;
                if(currentAngle > angle)
                    degrees = currentAngle - angle;
                else
                    degrees = angle - currentAngle;
                degrees = (degrees/FULLSTEP) * 4;
                uint16_t i;
                for(i = 0; i < degrees; i++) {
                    microStep();
                    yield();
                }
                currentAngle = angle;
            }
            else if(dir == 2) {
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
                degrees = (degrees/FULLSTEP) * 4;
                uint16_t i;
                for(i = 0; i < degrees; i++) {
                    microStep();
                    yield();
                }
                currentAngle = 50;
            }
            turnMechDataReceivedFlag = 0;
            setPinValue(SLEEP, 0);
        }
        yield();
    }
}
/*
void turningMechanism() {
    uint8_t value = 1;

    while(1) {
        //1/8
        uint8_t i;
        for(i = 0; i < 800; i++) {
            microStep();
            yield();
        }
        waitMicrosecond(5000); //15000
        value ^= 1;
        setPinValue(DIR, value);
        for(i = 0; i < 800; i++){
            microStep();
            yield();
        }
    }

    setStepFormat(4);
    uint32_t i = 0;
    while(1){
        value ^= 1;
        setPinValue(DIR, value);
        for(i = 0; i < 42000; i++) {
            microStep();
        }
        waitMicrosecond(1000000); //15000
        value ^= 1;
        setPinValue(DIR, value);
        for(i = 0; i < 4200; i++){
            microStep();
        }
    }

}
*/
//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

//int main(void) {
//    // Initialize hardware
//    initHw();
//    initUart0();
//    setUart0BaudRate(115200, 40e6);
//
////    setStepFormat(0);
//
//    /*
//    setPinValue(DIR, 1); //right
//
//    while(1) {
//        uint8_t j = 0;
//        for(j = 0; j < 200; j++){
//            microStep();
//        }
//        waitMicrosecond(2000000); //15000
//        setPinValue(DIR, 0); //left
//        j = 0;
//        for(j = 0; j < 200; j++){
//            microStep();
//        }
//        waitMicrosecond(2000000); //15000
//        setPinValue(DIR, 1);
//    }
//    */
//
////    while(true) {
////        putcUart0(70);
////        putcUart0(71);
////        waitMicrosecond(500000);
////    }
//    /*
//    while(true) {
//        if(kbhitUart0()) {
//            uint8_t distance = (uint8_t)getcUart0();
//            uint8_t direction = (uint8_t)getcUart0();
//            putcUart0(70);
//            putcUart0(71);
//            if(distance == 70 || direction == 71) {
//                setPinValue(RED_LED, 1);
//                waitMicrosecond(1000000);
//                setPinValue(RED_LED, 0);
//            }
//            setPinValue(GREEN_LED, 1);
//            waitMicrosecond(2000000);
//            setPinValue(GREEN_LED, 0);
////            setPinValue(DIR, direction);
////            uint8_t i = 0;
////            double dist = distance;
////            for(i = 0; i < (int)dist; i++) {
////                microStep();
////            }
//        }
//    }
//    /*
//    // set up step format
//    setStepFormat(4);
//    waitMicrosecond(10000);
//    setPinValue(DIR, 1);
//    waitMicrosecond(1);
////    shellFunction();
//
//    uint32_t count = 0;
//    while(true) {
//        microStep();
//        if(getPinValue(PUSH_BUTTON) == 0) {
//            waitMicrosecond(180000); //15000
//            changeDirection();
//        }
//        count++;
//    }
// */
//    /*
//    //1/16
//    setStepFormat(4);
//    uint32_t i = 0;
//    while(1){
//        for(i = 0; i < 3200; i++) {
//            microStep();
//        }
//        waitMicrosecond(500000); //15000
//        changeDirection();
//        for(i = 0; i < 3200; i++){
//            microStep();
//        }
//    }
//
//*/
//   //1/8
//
//    setStepFormat(3);
//    uint8_t value = 1;
//    setPinValue(DIR, value);
//    uint32_t i = 0;
//    while(1){
//        for(i = 0; i < 800; i++) {
//            microStep();
//        }
//        waitMicrosecond(500000); //15000
//        value ^= 1;
//        setPinValue(DIR, value);
//        for(i = 0; i < 800; i++){
//            microStep();
//        }
//    }
//
////    uint16_t i = 0;
////    setPinValue(DIR, 1);
////    for(i = 0; i < 6400; i++) {
////        microStep();
////    }
////    i = 0;
////    waitMicrosecond(160000);
////    setPinValue(DIR, 0);
////    for(i = 0; i < 12800; i++) {
////        microStep();
////    }
////    i = 0;
////    waitMicrosecond(160000);
////    setPinValue(DIR, 1);
////    for(i = 0; i < 6400; i++) {
////        microStep();
////    }
//
//    while(1);
//
//}
