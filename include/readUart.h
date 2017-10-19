#ifndef READUART_H
#define READUART_H

#include <API.h>

void readUart();

extern float leftVelTarget, rightVelTarget;
extern Semaphore velSem;

#endif
