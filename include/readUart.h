#ifndef READUART_H
#define READUART_H

#include <API.h>

void readUart();

extern float leftVelTarget;
extern float rightVelTarget;
extern Semaphore velSem;

#endif
