/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"

// Morsecode States
enum BUTTON_STATES {SOS, OK} BUTTON_STATE, BUTTON_INIT;

unsigned char i = 0; // Initialize counter to 0
unsigned char state; // State to read LEDs arrays

// Array to tell state machine which LEDs to turn on/off
int SOS_LEDs[] = {
                  1,0,1,0,1, // S (dot, dot, dot) - Flashes Red 3 times - 500 ms each
                  0,0,0, // Break between characters - 1500 ms
                  2,2,2,0,2,2,2,0,2,2,2, // O (dash, dash, dash) - Flashes Green 3 times - 1500 ms each
                  0,0,0, // Break between characters - 1500 ms
                  1,0,1,0,1, // S (dot, dot, dot) - Flashes Red 3 times - 500 ms each
                  0,0,0,0,0,0,0 // Break between words - 3500 ms
};

int OK_LEDs[] = {
                 2,2,2,0,2,2,2,0,2,2,2, // O (dash, dash, dash) - Flashes Green 3 times - 1500 ms each
                 0,0,0, // Break between characters - 1500 ms
                 2,2,2,0,1,0,2,2,2, // K (dash, dot, dash) - Flashes Green, Red, Green - 1500 ms, 500 ms, 1500 ms
                 0,0,0,0,0,0,0 // Break between words - 3500 ms
};

void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{

    if (BUTTON_INIT == SOS) {
        state = SOS_LEDs[i]; // Set state to SOS_LEDs array

        switch(state) {
            case 0:// Both lights off - Breaks for characters and end of words
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                break;
            case 1: // Red light on - Green light off - dots
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                break;
            case 2:// Green light on - Red light off - dashes
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
                break;
        }

        i++;
        if (i == 34){ // Sets initial button state to current state (SOS) when array is done
            BUTTON_INIT = BUTTON_STATE;
            i = 0; // Clears i for next callback
        }
    }

    if (BUTTON_INIT == OK) {
        state = OK_LEDs[i]; // Set state to OK_LEDs array

        switch(state) {
            case 0:// Both lights off - Breaks for characters and end of words
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                break;
            case 1:// Red light on - Green light off - dots
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
                break;
            case 2:// Green light on - Red light off - dashes
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
                break;
        }

        i++;
        if (i == 30){ // Sets initial button state to current state (OK) when array is done
            BUTTON_INIT = BUTTON_STATE;
            i = 0; // Clears i for next callback
        }
    }


}

void initTimer(void){
    Timer_Handle timer0;
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);
    params.period = 500000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL) {
        /* Failed to initialized timer */
        while (1) {}
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }
}

/*
 *      *****UNUSED BUTTON*****
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{
    // Switch between words depending on initial button state
    if(BUTTON_INIT == OK){
        BUTTON_STATE = SOS;
    }
    if(BUTTON_INIT == SOS){
        BUTTON_STATE = OK;
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();
    initTimer();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn off user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

    // Only uses button 1 to switch between SOS and OK
    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        /* Configure BUTTON1 pin */
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

        /* Install Button callback */
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }


    return (NULL);
}
