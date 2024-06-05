#pragma once

#include "lib/dy50.h"
#include <stdint.h>
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

bool isReceived;

void init();
void UART_Init(uint32_t uartBase, uint32_t baudRate);
void UART_Send(uint32_t uartBase, uint8_t data);
void initSystemClock();
void configureUARTPrint();
void UARTInterruptHandler();
void sendPacket(Packet *packet);
Packet awaitReponsePacket();
void delay(uint8_t seconds);
