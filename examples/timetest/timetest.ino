// select drift mode
// 1: simply drift if miss
// 2: no drift, but multible execution possible if multible misses
// 3: drift only for multible misses
// 4: stay in meassure

//tells every 1,5 seconds and every minute how many millisconds have been
// counted since reset; uses Serial at 57600 baud.

#define ET_DRIFT 3

#include "everytime.h"
#include <avr/sleep.h>

void setup(){
    Serial.begin(57600);
}

void loop(){

    every(1500){
        Serial.print("its ");
        Serial.println(millis());
    }
    everynow(60000){
        Serial.print("a minute has gone by ");
        Serial.println(millis());
    }

}
