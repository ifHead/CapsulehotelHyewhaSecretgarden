#ifndef Gvar_h         
#define Gvar_h

#include<Arduino.h>
//전역변수


struct read_struct {
    boolean backup[15];
    boolean pin[15];
};


struct timer_s{
    boolean en;
    char scale;
    char cnt;

};

struct timer_s timer;
struct read_struct ext_pin;


#endif
