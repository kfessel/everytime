#include "everytime.h"
#include <avr/sleep.h>

static const int led = 13;

void setup(){
    pinMode(led, OUTPUT);
    Serial.begin(9600);
}

void loop(){
    //execute every 500 ms
    every(500){
        // blink (change state and write to led
        static uint8_t state = 0;
        state = ! state;
        digitalWrite(led ,state);
    }
    //execute every 1000 ms
    every(1000){
        //tell how may milliseconds have gone by since last boot
        Serial.print("its ");
        Serial.println(millis());
    }
    {   //sleepblock put if(0) in front if u do not want the AVR to sleep
        /*     set sleep mode
        *     SLEEP_MODE_IDLE
        *        -the least power savings keeps io-clk running -> timer 0 ->
        *         millis and micros keep working (wakeup every about every 1ms)
        *     SLEEP_MODE_ADC
        *     SLEEP_MODE_PWR_SAVE
        *     SLEEP_MODE_STANDBY
        *     SLEEP_MODE_PWR_DOWN     -the most power savings
        */
        set_sleep_mode(SLEEP_MODE_IDLE); //sleeps for the rest of this millisecond or less if other trigger
        sleep_enable();
        sleep_mode();            // put the device to sleep
        sleep_disable();
    }
}
