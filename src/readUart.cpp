#include "readUart.h"

union Converter {
  int32_t full_data;     // occupies 4 bytes
  uint8_t bytes[4];     // occupies 4 byte
};  

float leftVelTarget;
float rightVelTarget;
Semaphore velSem = semaphoreCreate();

void readUart() {
  int received[7];
  int id;
  int checksum;
  Converter inData;

  while (1) {
    for (int i = 0; i < 7; i++)
      received[i] = fgetc(uart1);

    id = received[1];
    inData.bytes[0] = received[2];
    inData.bytes[1] = received[3];
    inData.bytes[2] = received[4];
    inData.bytes[3] = received[5];
    checksum = received[6];

    semaphoreTake(velSem, -1);

    // convert from deg/min to rpm
    if (id == 0x01)
      leftVelTarget = inData.full_data/360.0;
    else if(id == 0x02)
      rightVelTarget = inData.full_data/360.0;

    semaphoreGive(velSem);

    taskDelay(20);
  }
}
