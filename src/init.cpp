/** @file init.c
 * @brief File for initialization code
 *
 * created by asgard MQP
 */

#include "main.h"
#include "controlLoop.h"

extern "C" {
  void __libc_init_array();
  void __cxa_pure_virtual() { exit(1111); }
}

void initializeIO() {
  usartInit(uart1, 115200, SERIAL_DATABITS_8 | SERIAL_PARITY_NONE);
  usartInit(uart2, 115200, SERIAL_DATABITS_8 | SERIAL_PARITY_NONE);
}

void initialize() {
  __libc_init_array();
  imeInitializeAll();
  taskCreate((TaskCode) controlLoop,
    TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}
