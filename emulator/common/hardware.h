#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include "mos6502.h"

extern mos6502 theCPU;
extern uint64_t total_cycles;

// change LEDs
void set_red_led(bool state);

// called at start
void start();

// called at 50 ms interval
void tick();

// called for key presses
void onKeyDown(char key);
void onKeyUp(char key);

#endif
