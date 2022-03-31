#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "gpio.h"
#include "wait.h"
#include "svcCalls.h"
#include "LEDThread.h"
// led.c

// Hardware Config:
// Light 1:
//  M0PWM2 (PB4) controls the red colored light on the LED strip
// Light 2:
//  M0PWM3 (PB5) controls the blue colored light on the LED strip
// Light 3:
//  M0PWM4 (PE4) controls the green colored light on the LED strip

// Port B Masks
#define LIGHT1_MASK 16
#define LIGHT2_MASK 32

// Port E Masks
#define LIGHT3_MASK 16


ledData ledThreadData;
uint8_t ledDataReceivedFlag = 0;

// Initialize PWM
void initializePwm()
{
    // Enable clocks
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0; // Clock PWM0 Module
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R4; // Clock port B and E
    _delay_cycles(3);

    // Configure Lights
    GPIO_PORTB_DIR_R |= LIGHT1_MASK | LIGHT2_MASK;                 // make bit 4 and 5 an output
    GPIO_PORTB_DR2R_R |= LIGHT1_MASK | LIGHT2_MASK;                // set drive strength to 2mA
    GPIO_PORTB_DEN_R |= LIGHT1_MASK | LIGHT2_MASK;                 // enable digital
    GPIO_PORTB_AFSEL_R |= LIGHT1_MASK | LIGHT2_MASK;               // select auxiliary function
    GPIO_PORTB_PCTL_R &= GPIO_PCTL_PB4_M | GPIO_PCTL_PB5_M;    // enable PWM
    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB4_M0PWM2 | GPIO_PCTL_PB5_M0PWM3;

    GPIO_PORTE_DIR_R |= LIGHT3_MASK;                 // make bits 4
    GPIO_PORTE_DR2R_R |= LIGHT3_MASK;                // set drive strength to 2mA
    GPIO_PORTE_DEN_R |= LIGHT3_MASK;                 // enable digital
    GPIO_PORTE_AFSEL_R |= LIGHT3_MASK;               // select auxiliary function
    GPIO_PORTE_PCTL_R &= GPIO_PCTL_PE4_M;    // enable PWM
    GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_M0PWM4;

    // Configure PWM module 0 to drive 4 LEDS (eventually 2 motors)
    // Light 1   on M0PWM2 (PB4), M0PWM1a
    // Light 2   on M0PWM3 (PB5), M0PWM1b
    // Light 3   on M0PWM4 (PE4), M0PWM2a
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0;                // reset PWM0 module
    SYSCTL_SRPWM_R = 0;                              // leave reset state
    PWM0_1_CTL_R = 0;                                // turn-off PWM0 generator 1 (drives outs Left Motor) id = 0
    PWM0_2_CTL_R = 0;                                // turn-off PWM0 generator 2 (drives outs Right Motor) id = 1
    PWM0_1_GENA_R = PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
                                                     // output 2 on PWM0, gen 1a, cmpa
    PWM0_1_GENB_R = PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE;
                                                     // output 3 on PWM0, gen 1b, cmpb
    PWM0_2_GENA_R = PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
                                                     // output 4 on PWM0, gen 2a, cmpa
    PWM0_1_LOAD_R = 255;                            // set frequency to 40 MHz sys clock / 2 / 1024 = 19.53125 kHz
    PWM0_2_LOAD_R = 255;
    PWM0_INVERT_R = PWM_INVERT_PWM2INV | PWM_INVERT_PWM3INV | PWM_INVERT_PWM4INV;
                                                     // invert outputs so duty cycle increases with increasing compare values
    PWM0_1_CMPA_R = 0;                               // Light 1 off (0=always low, 254=always high)
    PWM0_1_CMPB_R = 0;                               // Light 2 off
    PWM0_2_CMPB_R = 0;                               // Light 3 off

    PWM0_1_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM0 generator 1
    PWM0_2_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM0 generator 2
    PWM0_ENABLE_R = PWM_ENABLE_PWM2EN | PWM_ENABLE_PWM3EN | PWM_ENABLE_PWM4EN;
                                                     // enable outputs
}

void setPwmDutyCycle(uint8_t mode)
{
    switch(mode)
    {
    case 0: // Light 1 on, others off
        PWM0_1_CMPA_R = 254; // Light 1 on
        PWM0_1_CMPB_R = 0; // Light 2 off
        PWM0_2_CMPA_R = 0; // Light 3 off
        break;
    case 1: // Light 2 on, others off
        PWM0_1_CMPA_R = 0; // Light 1 off
        PWM0_1_CMPB_R = 254; // Light 2 on
        PWM0_2_CMPA_R = 0; // Light 3 off
        break;
    case 2: // Light 2 on, others off
        PWM0_1_CMPA_R = 0; // Light 1 off
        PWM0_1_CMPB_R = 0; // Light 2 off
        PWM0_2_CMPA_R = 254; // Light 3 on
        break;
    case 3: // All lights off
        PWM0_1_CMPA_R = 0; // Light 1 off
        PWM0_1_CMPB_R = 0; // Light 2 off
        PWM0_2_CMPA_R = 0; // Light 3 off
        break;
    }
}

void ledThread() {
    while(true) {
        if(ledDataReceivedFlag){
            uint8_t currentMode = ledThreadData.mode;
            setPwmDutyCycle(currentMode);
            ledDataReceivedFlag = 0;
        }
        yield();
    }
}
