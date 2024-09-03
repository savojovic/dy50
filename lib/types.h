#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* ***** Structures ***** */

// Helper class to create UART packets
typedef struct
{
    uint16_t start_code;  // "Wakeup" code for packet detection
    uint8_t address[4];   // 32-bit Fingerprint sensor address
    uint8_t type;         // Type of packet
    uint16_t length;      // Length of packet
    uint8_t data[256];    // The raw buffer for packet payload
    uint16_t checksum;    // The checksum
} Packet;

typedef struct
{
    uint16_t status_reg;
    uint16_t system_id;
    uint16_t capacity;
    uint16_t security_level;
    uint32_t device_addr;
    uint16_t packet_len;
    uint16_t baud_rate;
} SensorParams;

// Return value of the fingerSearch(uint8_t bufferId) function
typedef struct
{
    uint16_t fingerprintPage;
    uint16_t confidence;
    uint8_t statusCode;
} FingerPageAndConfidence;

#endif /* TYPES_H */
