#include <stdint.h>
#include <stdbool.h>
#include "uartCommunicationThread.h"
#include "clock.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"
#include "svcCalls.h"
#include "turningMechanismThread.h"
#include "gpio.h"
#include "wait.h"
#include "LEDThread.h"

//#define RED_LED PORTF,1

//ledData ledThreadData;

uint8_t sendUartFlag;
uint8_t sendLEDDataFlag;
uint8_t sendTurnMechDataFlag;

//uint8_t ledDataReceivedFlag = 0;

void uartReceiveInput() {
    uint8_t data[4];
    uint8_t i;
    uint8_t ID = (uint8_t)getcUart0();
    switch(ID) {
        case TT:
            data[0] = ID;
            for(i = 1; i < 4; i++) {
                data[i] = (uint8_t)getcUart0();
            }
            turnMecData.id = ID;
            turnMecData.angle = data[1];
            turnMecData.direction = data[2];
            turnMecData.rate = data[3];
            turnMechDataReceivedFlag = 1;
            break;
        case LT:
            data[0] = ID;
            data[1] = (uint8_t)getcUart0();
            ledThreadData.id = ID;
            ledThreadData.mode = data[1];
            ledDataReceivedFlag = 1;
            break;
        default:
            //don't set flags
            break;
    }
    //testing (remove later)
//    sendUartFlag = 1;
//    sendLEDDataFlag = 1;
}
void uartSendOutput(){
    if(sendLEDDataFlag) {
        putcUart0(ledThreadData.id);
        putcUart0(ledThreadData.mode);
        sendLEDDataFlag = 0;
    }
    if(sendTurnMechDataFlag) {
        putcUart0(turnMecData.id + 1);
        putcUart0(turnMecData.direction + 1);
        putcUart0(turnMecData.angle + 1);
        putcUart0(turnMecData.rate + 1);
        sendTurnMechDataFlag = 0;
    }
    sendUartFlag = 0;
}
void uartCommunication() {
    while(1) {
        if(kbhitUart0()) {
            setPinValue(PORTF,1,1);
            waitMicrosecond(200000);
            setPinValue(PORTF,1,0);
            uartReceiveInput();
        }
        if(sendUartFlag) {
            uartSendOutput();
        }
        yield();
    }
}
