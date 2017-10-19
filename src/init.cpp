/** @file init.c
 * @brief File for initialization code
 *
 * created by asgard MQP
 */

#include "main.h"
#include "controlLoop.h"
#include "readUart.h"

extern "C" {
  void __libc_init_array();
  void __cxa_pure_virtual() { exit(1111); }
}

void initializeIO() {
  __libc_init_array();
  usartInit(uart1, 115200, SERIAL_DATABITS_8 | SERIAL_PARITY_NONE);
}

void initialize() {
  taskCreate((TaskCode)controlLoop, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
  taskCreate((TaskCode)readUart, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}
