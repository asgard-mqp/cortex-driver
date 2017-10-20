#include "uart.h"

union Converter {
  int32_t fullData;   // occupies 4 bytes
  uint8_t bytes[4];    // occupies 4 byte
};

void writeUart(uint8_t packet_id, int32_t value) {
  Converter out_data;
  out_data.fullData = value;

  fputc(0xFA, uart1);
  fputc(packet_id, uart1);
  uint8_t checksum = 255;
  for (int i = 0; i < 4; i++) {
    checksum -= out_data.bytes[i];
    fputc(out_data.bytes[i], uart1);
  }


  printf("FA:%02x:%02x:%02x:%02x:%02x:%02x\n", packet_id,
    out_data.bytes[0], out_data.bytes[1], out_data.bytes[2], out_data.bytes[3],
    checksum);

  fputc(checksum, uart1);
}

void readUart(uint8_t& packet_id, int32_t& value) {
  Converter in_data;

  // printf("checking uart.... %d bytes available\n", fcount(uart1));
  if (fcount(uart1) < 7) {
    return;
  }

  uint8_t startByte = fgetc(uart1); // TODO -- check for partial data
  packet_id = fgetc(uart1);

  uint8_t checksum_calc = 255;
  for (int i = 0; i < 4; i++) {
    in_data.bytes[i] = fgetc(uart1);
    checksum_calc -= in_data.bytes[i];
  }
  uint8_t checksum_given = fgetc(uart1);

  printf("%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", startByte, packet_id,
    in_data.bytes[0], in_data.bytes[1], in_data.bytes[2], in_data.bytes[3],
    checksum_given);

  if (false && checksum_calc == checksum_given) {
    packet_id = 0;
  } else {
    value = in_data.fullData;
  }
}
