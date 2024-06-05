#include "tm4c123gxl_utils.h"
#include "config.h"

uint32_t transmissionBytesCounter = 0;
uint8_t recvPacket[300];
uint16_t receivePacketLength;

void getStructuredPacket()
{
    uint8_t* bytePointer = recvPacket;
    // Create a packet of raw bytes
    responsePacket.start_code = *(bytePointer++) << 8 | *(bytePointer++);
    responsePacket.address[0] = *(bytePointer++);
    responsePacket.address[1] = *(bytePointer++);
    responsePacket.address[2] = *(bytePointer++);
    responsePacket.address[3] = *(bytePointer++);
    responsePacket.type = *(bytePointer++);
    responsePacket.length = *(bytePointer++) << 8 | *(bytePointer++);
    for(int i = 0; i< responsePacket.length - 2; i++)
    {
        responsePacket.data[i] = *(bytePointer++);
    }
    responsePacket.checksum = *(bytePointer++) << 8 | *(bytePointer++);
}

void UARTInterruptHandler()
{
    // Get the interrupt status
    uint32_t ui32Status = MAP_UARTIntStatus(UART_SENSOR_INTERFACE, true);

    // Clear the asserted interrupts
    MAP_UARTIntClear(UART_SENSOR_INTERFACE, ui32Status);
    // Handle received interrupt

    // Read raw bytes into an array
    while (MAP_UARTCharsAvail(UART_SENSOR_INTERFACE))
    {
       // Read a character from the UART
       recvPacket[transmissionBytesCounter++] = UARTCharGetNonBlocking(UART_SENSOR_INTERFACE);
    }
    if (transmissionBytesCounter >= 9)        //length of the packet has been received
    {
        receivePacketLength = (uint16_t)recvPacket[7] << 8 |
                              (uint16_t)recvPacket[8] + PACKAGE_SIZE_WITHOUT_DATA - 2;
    }
    if (transmissionBytesCounter == receivePacketLength)
    {
        getStructuredPacket();
        transmissionBytesCounter = 0;
        isReceived = true;
    }
}

// Function to initialize UART communication for a given UART number and baud rate
void UART_Init(uint32_t uartBase, uint32_t baudRate)
{
    // Enable UART module and corresponding GPIO
    switch(uartBase) {
        case UART0_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            GPIOPinConfigure(GPIO_PA0_U0RX);
            GPIOPinConfigure(GPIO_PA1_U0TX);
            GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            break;
        case UART1_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            GPIOPinConfigure(GPIO_PB0_U1RX);
            GPIOPinConfigure(GPIO_PB1_U1TX);
            GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            break;
        case UART2_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinConfigure(GPIO_PD6_U2RX);
            GPIOPinConfigure(GPIO_PD7_U2TX);
            GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
            break;
        case UART3_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
            GPIOPinConfigure(GPIO_PC6_U3RX);
            GPIOPinConfigure(GPIO_PC7_U3TX);
            GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
            break;
        case UART4_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
            GPIOPinConfigure(GPIO_PC4_U4RX);
            GPIOPinConfigure(GPIO_PC5_U4TX);
            GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case UART5_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinConfigure(GPIO_PE4_U5RX);
            GPIOPinConfigure(GPIO_PE5_U5TX);
            GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case UART6_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinConfigure(GPIO_PD4_U6RX);
            GPIOPinConfigure(GPIO_PD5_U6TX);
            GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case UART7_BASE:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinConfigure(GPIO_PE0_U7RX);
            GPIOPinConfigure(GPIO_PE1_U7TX);
            GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            break;
        default:
            return; // Unsupported UART module
    }

    // Configure UART
    UARTConfigSetExpClk(uartBase, SysCtlClockGet(), baudRate,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);

    // Enable UART module
    UARTEnable(uartBase);
}

// Function to send data over UART
void UART_Send(uint32_t uartBase, uint8_t data)
{
    // Send data over UART
    UARTCharPut(uartBase, data);
}

void init()
{
    isReceived = false;
    initSystemClock();
    configureUARTPrint();
    UART_Init(UART_SENSOR_INTERFACE, UART_SENSOR_BAUD);

    //Enable UART interrupt
    MAP_IntEnable(INT_UART_ASSIGNMENT);
    MAP_UARTIntEnable(UART_SENSOR_INTERFACE, UART_INT_RX | UART_INT_RT);
}

// Initialize system clock
void initSystemClock()
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL |
                   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}


void configureUARTPrint(void)
{
    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(UART_PRINT_INTERFACE, UART_PRINT_BAUD, 16000000);
}





void sendPacket(Packet *packet)
{
    unsigned char *address = (unsigned char*)&(packet->address);
    unsigned char *data = packet->data;

    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, packet->start_code >> 8);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, (packet->start_code << 8) >> 8);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, *address++);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, *address++);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, *address++);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, *address++);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, packet->type);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, packet->length >> 8);
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, (packet->length << 8) >> 8);
    int i;
    for (i = 0; i < packet->length - 0x2; i++)
    {
        MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, *data++);
    }
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, (packet->checksum >> 8));
    MAP_UARTCharPutNonBlocking(UART_SENSOR_INTERFACE, (packet->checksum << 8) >> 8);
}

/**
 * @brief Halts program execution until a specified receive flag is not set.
 */
Packet awaitReponsePacket()
{
    while (!isReceived)
    {
    }
    isReceived = false;
    return responsePacket;
}


void delay(uint8_t seconds)
{
    SysCtlDelay(seconds * (SysCtlClockGet() / 3));
}
