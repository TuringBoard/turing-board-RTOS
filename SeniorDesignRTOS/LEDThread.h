#ifndef LED_H_
#define LED_H_

typedef struct _led {
    uint8_t id;
    uint8_t mode;
} ledData;

#pragma DATA_SECTION(ledThreadData, ".tbglob")
#pragma DATA_SECTION(ledDataReceivedFlag, ".tbglob")

extern ledData ledThreadData;
extern uint8_t ledDataReceivedFlag;

void setPwmDutyCycle(uint8_t mode);
void initializePwm();
void ledThread();

#endif
