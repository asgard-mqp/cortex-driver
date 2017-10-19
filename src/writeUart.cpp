#include "writeUart.h"

union Converter {
  int32_t full_data;     // occupies 4 bytes
  uint8_t bytes[4];     // occupies 4 byte
};  

void writeUart(uint8_t systemId, int data) {
  Converter outData;

  outData.full_data = data;

  fputc(0xFA, uart1); //start byte
  fputc(systemId, uart1);
  for(int i = 0; i < 4; i++)
    fputc(outData.bytes[i], uart1);
  fputc(0xFF, uart1); //fake checksum
}
