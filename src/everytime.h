#ifndef EVERYTIMEH24b0433
#define EVERYTIMEH24b0433
/* Copyright (c) 2017, Karl Fessel, lib everytime, All rights reserved.*/

// these macros run a command or block in regular intervals

/**
 * these macros depend on unspecified but very common behavior of C, C++ and
 * the target architectur:
 * - integer overflow for subtraction and addition
 * - if the return type of the time function is bigger than the storage
 *   the lower bits schould be stored
 *   (this behavior is specified for values that fit into the storrage type
 *    but is unspecified for values that are bigger)
 */

// simple versions for easy reading:
//     #define every(X) static uint16_t before = 0; uint16_t now = millis();\
            if((now - before >= X) && (before = now,1) )
//
//     static uint32_t before = 0;
//     unsigned long now = millis();
//     if (now - before > 1000){
//         before = now;


//this make the variable names more unique by adding the current line number
#define IDCAT3( A, B) A ## B
#define IDCAT2( A, B) IDCAT3( A, B)
#define IDCAT(X) IDCAT2(X, __LINE__)

/**
 * these are gereric variants to create the mor specifc ones
 * X      is the time betwen two runs
 * START  is the time of the first start or -1 to wait for the first perion after time 0 (-1 saves memory and flash)
 * STARTO is the time of the first start with offset from first test (now) (0 to start now)
 * F      is the function that returns some kind of time
 *  e.g.: Arduinos millis() and micros()
 *  !! millis() do not include all milliseconds since its incremented in
 *  !!   256 / 250 kHz steps and are compensated every ~42 milliseconds by adding another millisecond !!
 *  !! micros() function itself takes more than a microsecond to execute !!
 * T      is the type of the storage variable the type should behave as said above
 *
 * everySTF(X, START, F, T)
 *  start with offset from time 0, -1 -> first intevall
 *
 * everyNTF(X, STARTO, F, T)
 *  start with offset from now
 */
#ifndef ET_DRIFT
// Dirft Mode
// 1 : drift if miss
//      e.g.: trigger every 5, miss at 10 execution, starts at 11 next trigger will be 16
// 2 : never drift but may loose its function if execution takes to long
//      e.g.: trigger every 5, miss at 10 and 15 -> double trigger
// 3 : no drift until big miss then drift to now
//      e.g.: trigger every 5, miss at 10 and 15 -> trigger once at 16 next trigger will be 21
// 4 : try to stay in messure, if big miss advance in steps
//      e.g.: trigger every 5, miss at 10 and 15 trigger once at 16 next trigger will be 20
#define ET_DRIFT 3
#endif

#if ET_DRIFT == 1
// 34 Byte for everySTF(X, -1, millis(), uint16_t )
// drift if miss
// non dynamic initialisation of static variable saves space in memory and flash 0 saves even more
#define everySTF(X, START, F, T) \
            T IDCAT(now) = (F);\
            static T IDCAT(before) = ((START) < 0) ? 0 : (0 - (X) + (START));\
            if( (IDCAT(now) - IDCAT(before) >= (X)) \
             && ((IDCAT(before) = IDCAT(now)),1) )

// start with offset from now
#define everyNTF(X, STARTO, F, T) \
            T IDCAT(now) = (F);\
            static (T) IDCAT(before) = IDCAT(now) - (X) + (STARTO);\
            if( (IDCAT(now) - IDCAT(before) >= (X)) \
             && ((IDCAT(before) = IDCAT(now)),1) )

#endif
#if ET_DRIFT == 2
// 38 Byte for everySTF(X, -1, millis(), uint16_t )
// no drift but may lose its function if miss is to big
// non dynamic initialisation of static variable saves space in memory and flash 0 saves even more
#define everySTF(X, START, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = (START < 0) ? 0 : (0 - X + START);\
            if( (IDCAT(now) - IDCAT(before) >= X) \
             && ((IDCAT(before) += X),1) )

// start with offset from now
#define everyNTF(X, STARTO, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = IDCAT(now) - X + STARTO;\
            if( (IDCAT(now) - IDCAT(before) >= X) \
             && ((IDCAT(before) += X),1) )
#endif
#if ET_DRIFT == 3
// 46 Byte for everySTF(X, -1, millis(), uint16_t )
// drift when big miss (two intervals) else no drift
// non dynamic initialisation of static variable saves space in memory and flash 0 saves even more
#define everySTF(X, START, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = (START < 0) ? 0 : (0 - X + START);\
            if( (IDCAT(now) - IDCAT(before) >= X) \
              && ((IDCAT(before) = (IDCAT(now) - IDCAT(before) > 2 * X)?IDCAT(now):IDCAT(before) + X),1) )

// start with offset from now
#define everyNTF(X, STARTO, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = IDCAT(now) - X + STARTO;\
            if( (IDCAT(now) - IDCAT(before) >= X) \
             && ((IDCAT(before) = (IDCAT(now) - IDCAT(before) > 2 * X)?IDCAT(now):IDCAT(before) + X),1) )
#endif
#if ET_DRIFT == 4
// 68 Bytes for everySTF(X, -1, millis(), uint16_t )
// this uses lambda functions for the caluculation of the increment
// lambda functions are part of c++11
#define everySTF(X, START, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = (START < 0) ? 0 : (0 - X + START);\
            T IDCAT(dist) = IDCAT(now) - IDCAT(before);\
            if( (IDCAT(dist) >= X) \
             && (IDCAT(before) += [&](){ T step = 0; do{ step += X; }while( (IDCAT(dist) > step + X) && (step + X > step) ); return step; }() ,1  ))

// start with offset from now
#define everyNTF(X, STARTO, F, T) \
            T IDCAT(now) = F;\
            static T IDCAT(before) = IDCAT(now) - X + STARTO;\
            T IDCAT(dist) = IDCAT(now) - IDCAT(before);\
            T IDCAT(step) = 0;\
            if( (IDCAT(dist) >= X) \
             && (IDCAT(before) += [&](){ T step = 0; do{ step += X; }while( (IDCAT(dist) > step + X) && (step + X > step) ); return step; }() ,1  ))
#endif


// execute every X milliseconds (8 bit)

#define everyb(X)            everySTF(X, -1, millis(), uint8_t )

#define everynowb(X)         everyNTF(X,  0, millis(), uint8_t )

// execute every X milliseconds (16 bit)

#define every(X)            everySTF(X, -1, millis(), uint16_t )

#define everynow(X)         everyNTF(X,  0, millis(), uint16_t )

// execute every X milliseconds (32 bit)

#define everyl(X)            everySTF(X, -1, millis(), uint32_t )

#define everynowl(X)         everyNTF(X,  0, millis(), uint32_t )


// execute every X microseconds (16 bit)
// !! loop cycle may be longer on most arduinos than the resolution of this !!
// (30 microseconds with this just toggeling a LED using arduino method digitalWrite)

#define everyu(X)            everySTF(X, -1, micros(), uint16_t )

#define everynowu(X)         everyNTF(X,  0, micros(), uint16_t )

// execute every X microseconds (32 bit)

#define everyul(X)            everySTF(X, -1, micros(), uint32_t )

#define everynowul(X)         everyNTF(X,  0, micros(), uint32_t )


#endif
