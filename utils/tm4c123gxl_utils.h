#pragma once

#include "lib/types.h"
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

/* ***** Defines ***** */

#define PACKAGE_SIZE_WITHOUT_DATA               11   // Package size without data is fixed 11 bytes

/* ***** Functions ***** */

void init();
void UART_Init(uint32_t uartBase, uint32_t baudRate);
void UART_Send(uint32_t uartBase, uint8_t data);
void UARTInterruptHandler();
void sendPacket(Packet *packet);
Packet awaitReponsePacket();
