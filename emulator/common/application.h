#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>

// called at start
void start();

// called at about 50 Hz
void tick(uint32_t deltaMilliseconds);

#endif