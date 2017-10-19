#ifndef WRITEUART_H
#define WRITEUART_H

#include <API.h>

void writeUart(uint8_t systemId, int32_t data);
void readUart(uint8_t& systemId, int32_t& data);

#endif
