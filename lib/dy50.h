#ifndef DY50_H
#define DY50_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils/config.h"
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"

/* ***** Defines ***** */

#define FINGERPRINT_STARTCODE                   0xEF01 // Fixed falue of EF01H; High byte transferred first
#define FINGERPRINT_SETPASSWORD                 0x12   // Sets passwords
#define DEFAULT_MODULE_ADDRESS                  0xFFFFFFFF
#define FINGERPRINT_PASSVERIFY                  0x21 // Verify the fingerprint passed
#define FINGERPRINT_TEMPLATECOUNT               0x1D // Read finger template numbers
#define FINGERPRINT_COMMANDPACKET               0x1  // Command packet
#define FINGERPRINT_LEDON                       0x50 // Turn on the onboard LED
#define FINGERPRINT_LEDOFF                      0x51 // Turn off the onboard LED
#define FINGERPRINT_SEARCH                      0x04 // Search for fingerprint in slot
#define FINGERPRINT_DELETE                      0x0C // Delete templates
#define FINGERPRINT_EMPTY                       0x0D // Empty library
#define FINGERPRINT_UPLOAD                      0x08 // Upload template
#define FINGERPRINT_LOAD                        0x07 // Read/load template
#define FINGERPRINT_STORE                       0x06 // Store template
#define FINGERPRINT_REGMODEL                    0x05 // Combine character files and generate template
#define FINGERPRINT_IMAGE2TZ                    0x02 // Generate character file from image
#define FINGERPRINT_GETIMAGE                    0x01 // Collect finger image
#define FINGERPRINT_READSYSPARAM                0x0F // Read system parameters
#define FINGERPRINT_VERIFYPASSWORD              0x13 // Verifies the password
#define PACKAGE_SIZE_WITHOUT_DATA               11   // Package size without data is fixed 11 bytes
#define FINGERPRINT_OK                          0x00 // Command execution is complete

#define FINGERPRINT_PACKETRECIEVEERR            0x01 // Error when receiving data package
#define FINGERPRINT_NOFINGER                    0x02 // No finger on the sensor
#define FINGERPRINT_IMAGEFAIL                   0x03 // Failed to enroll the finger
#define FINGERPRINT_IMAGEMESS                   0x06 // Failed to generate character file due to overly disorderly
                                                     // fingerprint image
#define FINGERPRINT_FEATUREFAIL                 0x07 // Failed to generate character file due to the lack of character point
                                                     // or small fingerprint image
#define FINGERPRINT_NOMATCH                     0x08 // Finger doesn't match
#define FINGERPRINT_NOTFOUND                    0x09 // Failed to find matching finger
#define FINGERPRINT_ENROLLMISMATCH              0x0A // Failed to combine the character files
#define FINGERPRINT_BADLOCATION                 0x0B // Addressed PageID is beyond the finger library
#define FINGERPRINT_DBRANGEFAIL                 0x0C // Error when reading template from library or invalid template
#define FINGERPRINT_UPLOADFEATUREFAIL           0x0D // Error when uploading template
#define FINGERPRINT_PACKETRESPONSEFAIL          0x0E // Module failed to receive the following data packages
#define FINGERPRINT_UPLOADFAIL                  0x0F // Error when uploading image
#define FINGERPRINT_DELETEFAIL                  0x10 // Failed to delete the template
#define FINGERPRINT_DBCLEARFAIL                 0x11 // Failed to clear finger library
#define FINGERPRINT_PASSFAIL                    0x13 // Find whether the fingerprint passed or failed
#define FINGERPRINT_INVALIDIMAGE                0x15 // Failed to generate image because of lac of valid primary image
#define FINGERPRINT_FLASHERR                    0x18 // Error when writing flash
#define FINGERPRINT_INVALIDREG                  0x1A // Invalid register number
#define FINGERPRINT_ADDRCODE                    0x20 // Address code
#define FINGERPRINT_DATAPACKET                  0x2  // Data packet, must follow command packet or acknowledge packet
#define FINGERPRINT_ACKPACKET                   0x7  // Acknowledge packet
#define FINGERPRINT_ENDDATAPACKET               0x8  // End of data packet
#define FINGERPRINT_TIMEOUT                     0xFF // Timeout was reached
#define FINGERPRINT_BADPACKET                   0xFE // Bad packet was sent
#define FINGERPRINT_AURALEDCONFIG               0x35 // Aura LED control
#define DEFAULTTIMEOUT                          1000 // UART reading timeout in milliseconds

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

/* ***** Globals ***** */

uint16_t fingerprintPage; // The matching location that is set by fingerSearch() or fingerFastSearch()
uint16_t confidence; // The confidence of the fingerFastSearch() match, higher numbers are more confidents
Packet responsePacket; // Will be populated with received data in interrupt handler


/* ***** Extern functions ***** */

extern Packet awaitReponsePacket();
extern void sendPacket(Packet* packet);

/* ***** Functions ***** */

void sensorInit();
void begin(uint32_t baud);
bool verifyPassword(void);
SensorParams getParameters(void);
uint8_t getImage(void);
uint8_t image2Tz(uint8_t slot); // Slot values 1 & 2 for CharBuffer 1 & CharBuffer2 respectively
uint8_t createModel(void);
uint8_t emptyDatabase(void);
uint8_t storeModel(uint8_t buffer, uint16_t pageID);
uint8_t loadModel(uint8_t buffer, uint16_t location);
uint8_t getModel(void);
uint8_t deleteModel(uint16_t templateNum, uint8_t numberOfTemplates);
uint8_t fingerFastSearch(void);
FingerPageAndConfidence fingerSearch(uint8_t bufferId);
uint16_t getTemplateCount(void);
uint8_t setPassword(uint32_t password);
uint8_t LEDcontrol(bool on);
void writeStructuredPacket(Packet *p);
void getStructuredPacket();
uint8_t checkPassword(uint32_t password);
Packet createPacket(uint32_t sensorAddress, uint8_t type, uint8_t* content, uint8_t contentLength);
uint16_t calculateChecksum(Packet *packet);

#endif // DY50_H
