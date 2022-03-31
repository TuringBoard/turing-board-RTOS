#ifndef UARTCOM_H_H
#define UARTCOM_H_H
#define TURNINGTHREAD 1
#define LIGHTTHREAD   2

typedef enum {
    TT = 0x01,
    LT = 0x02,
};

typedef struct _uartData {
    uint8_t id;
    uint8_t data[5];
} uartData;

//typedef struct _led {
//    uint8_t id;
//    uint8_t color;
//} ledData;

#pragma DATA_SECTION(ledThreadData, ".tbglob")

#pragma DATA_SECTION(sendUartFlag, ".tbglob")
#pragma DATA_SECTION(sendLEDDataFlag, ".tbglob")
#pragma DATA_SECTION(sendTurnMechDataFlag, ".tbglob")

#pragma DATA_SECTION(ledDataReceivedFlag, ".tbglob")

extern uint8_t sendUartFlag;
extern uint8_t sendLEDDataFlag;
extern uint8_t sendTurnMechDataFlag;

void uartReceiveInput();
void uartSendOutput();
void uartCommunication();

#endif
