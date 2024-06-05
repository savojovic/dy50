#include "dy50.h"

/**
 * @brief  Construct a packet of raw data
 * @param  sensorAddress                     - Every sensor has an 4 byte address
 * @param  type                              - Packet type
 * @param  content                           - Array of bytes which represents the content/data/instructions of a packet
 * @param  contentLength                     - length of content array
 * @return struct Packet
 */
Packet createPacket(uint32_t sensorAddress, uint8_t type,
                    uint8_t* content, uint8_t contentLength)
{
    Packet packet;
    uint8_t* bytePointer = (uint8_t*)&sensorAddress;
    const uint8_t sizeOfChecksum = 0x02;
    packet.start_code = FINGERPRINT_STARTCODE;
    packet.address[0] = *bytePointer++;
    packet.address[1] = *bytePointer++;
    packet.address[2] = *bytePointer++;
    packet.address[3] = *bytePointer++;
    packet.type = type;
    packet.length = contentLength + sizeOfChecksum;
    for(int i = 0; i < contentLength; i++)
    {
        packet.data[i] = content[i];
    }
    packet.checksum = calculateChecksum(&packet);
    return packet;
}

/**
 * @brief  Sets a password used during the device handshake. By default the password is the length of 4 bytes and it's
 *         set to 0
 * @param  password                          - Represents a 4 byte password
 * @return                                   - The status of the password setting operation:
 *                                             0 if the password was set successfully.
 *                                             1 if an error occurred during the setting process.
 */
uint8_t setPassword(uint32_t password)
{
    Packet packet = {};
    Packet response = {};
    uint8_t content[5] = {};

    content[0] = FINGERPRINT_SETPASSWORD;
    content[1] = (uint8_t) (password >> 24);
    content[2] = (uint8_t) (password >> 16);
    content[3] = (uint8_t) (password >> 8);
    content[4] = (uint8_t) (password & 0xFF);

    packet = createPacket(DEFAULT_MODULE_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 5);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  Read the number of fingerprint templates stored in the module.
 *
 * @return Number of templates
 */
uint16_t getTemplateCount(void)
{
    uint8_t content[1] = {};
    Packet response = {};
    Packet packet = {};
    uint16_t templateCount;

    content[0] = FINGERPRINT_TEMPLATECOUNT;
    packet = createPacket(DEFAULT_MODULE_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();

    templateCount = response.data[1];
    templateCount <<= 8;
    templateCount |= response.data[2];

    return templateCount;
}

/**
 * @brief  Search for the fingerprint in CharBuffer1 or CharBuffer2
 * @param  bufferId  - Number of the buffer 0x1 for CharBuffer1 or 0x2 for CharBuffer2
 * @return           - FingerPageAndConfidence If fingerprint is found struct is set with fingerPage and confidence
 *                     values else both of them are set to response code of the packet
 * @note               The status codes of the respond packet:
 *                         0x00 Found
 *                         0x01 Error in receiving the package
 *                         0x09 Not found
 *                     After this function the content in the selected buffer does not change.
 */
FingerPageAndConfidence fingerSearch(uint8_t bufferId)
{
    FingerPageAndConfidence pageAndConfidence;
    uint8_t content[6] = {};
    Packet packet = {};
    Packet response = {};
    SensorParams params = getParameters();

    content[0] = FINGERPRINT_SEARCH;
    content[1] = bufferId; //CharBuffer
    content[2] = 0x00;
    content[3] = 0x00;
    content[4] = (uint8_t) (params.capacity >> 8);
    content[5] = (uint8_t) (params.capacity & 0xFF);

    packet = createPacket(DEFAULT_MODULE_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 6);
    sendPacket(&packet);
    response = awaitReponsePacket();

    if(response.data[0] == 0x00)
    {
        pageAndConfidence.fingerprintPage = 0xFFFF;
        pageAndConfidence.confidence = 0xFFFF;

        pageAndConfidence.fingerprintPage = response.data[1];
        pageAndConfidence.fingerprintPage <<= 8;
        pageAndConfidence.fingerprintPage |= response.data[2];

        pageAndConfidence.confidence = response.data[3];
        pageAndConfidence.confidence <<= 8;
        pageAndConfidence.confidence |= response.data[4];
    }
    else
    {
        pageAndConfidence.fingerprintPage = response.data[0];
        pageAndConfidence.confidence = response.data[0];
    }
    pageAndConfidence.statusCode = response.data[0];
    return pageAndConfidence;
}

/**
 * @brief  Control built in LED on the sensor
 * @param  isOn                              - If set, the led is activated
 * @return                                   - result of the operation
 */
uint8_t LEDcontrol(bool isOn)
{
    uint8_t content[1] = {};
    Packet packet = {};
    Packet response = {};
    if (isOn)
    {
        content[0] = FINGERPRINT_LEDON;
    }
    else
    {
        content[0] = FINGERPRINT_LEDOFF;
    }
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();
    return response.data[0];
}

/**
 * @brief  delete all fingerprint templates in the fingerprint library in the module.
 * @return 0x00 - Clearing successful
 *         0x01 - Error in receiving the package
 *         0x11 - Clearing failed
 */
uint8_t emptyDatabase(void)
{
    uint8_t content[1] = { FINGERPRINT_EMPTY };
    Packet packet = {};
    Packet response = {};
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  Delete the specified segment (N fingerprint templates starting with the specified template number) template
 *         in the module fingerprint library
 * @param  templateNum                       - Number of template to be deleted
 * @param  numberOfTemplates                 - Number of templates to be deleted
 * @return                                     0x00 - Deletion successful
 *                                             0x01 - Error in receiving the package
 *                                             0x10 - Deletion failed
 */
uint8_t deleteModel(uint16_t templateNum, uint8_t numberOfTemplates)
{
    uint8_t content[5] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_DELETE;
    content[1] = (uint8_t) (templateNum >> 8); // location of a template
    content[2] = (uint8_t) (templateNum & 0xFF);
    content[3] = 0x00; // number of templates to be deleted
    content[4] = 0x01; // number of templates to be deleted
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 5);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * GET MULTIPLE DATA PACKETS??
 */
uint8_t getModel(void)
{
    uint8_t content[2] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_UPLOAD;
    content[1] = 0x01; //transfer from CharBuffer 1
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 2);
    sendPacket(&packet);
    response = awaitReponsePacket();
    return response.data[0];
}

/**
 * @brief  Read the fingerprint template with the specified ID number in the flash database into the template buffer
 *         CharBuffer1 or CharBuffer2
 * @param  buffer                            - Buffer ID
 * @param  templateID                        - Id of a template
 * @return confirmation code                   0x00 Operation successful
 *                                             0x01 Error in receiving the package
 *                                             0x0c Invalid template or error reading
 *                                             0x0b PageID beyond the scope of the fingerprint database
 */
uint8_t loadModel(uint8_t buffer, uint16_t templateID)
{
    uint8_t content[4] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_LOAD;
    content[1] = buffer; //CharBuffer number
    content[2] = (uint8_t) (templateID >> 8);
    content[3] = (uint8_t) (templateID & 0xFF);
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 4);
    sendPacket(&packet);
    response = awaitReponsePacket();
    return response.data[0];
}

/**
 * @brief  Store the template data in the specified buffer (CharBuffer1 or CharBuffer2) to
           Flash in to the specified location in the fingerprint library
 * @param  buffer                            - Buffer ID
 * @param  pageID                            - fingerprint library location number, two bytes, high byte first
 * @return confirmation word                 - 0x00 Operation successful
 *                                             0x01 Error in receiving the package
 *                                             0x0b PageID beyond the scope of the fingerprint database
 *                                             0x18 Flash write error
 */
uint8_t storeModel(uint8_t buffer, uint16_t pageID)
{
    uint8_t content[4] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_STORE;
    content[1] = buffer; //CharBuffer number
    content[2] = (uint8_t) (pageID >> 8);
    content[3] = (uint8_t) (pageID & 0xFF);
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 4);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  Merge the feature files in CharBuffer1 and CharBuffer2 to generate a template, and the result is stored in
           CharBuffer1 and CharBuffer2 (the same content).
 * @return Confirmation word                - 0x00 Merge is successful
 *                                            0x01 Error in receiving the package
 *                                            0x0a Merge failed, the two fingerprints do not belong to the same finger
 */
uint8_t createModel(void)
{
    uint8_t content[1] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_REGMODEL;
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  Generate fingerprint features from the original image in ImageBuffer, and store the file in CharBuffer1 or
 *         CharBuffer2.
 * @param  buffer                            - CharBuffer ID
 * @return Confirmation word                 - 0x00 Feature successfully generated
 *                                             0x06 Fingerprint image is too messy
 *                                             0x07 Fingerprint image is normal, but there are too few feature points
 *                                             0x15 No valid original image in the image buffer
 *
 */
uint8_t image2Tz(uint8_t buffer)
{
    uint8_t content[2] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_IMAGE2TZ;
    content[1] = buffer;
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 2);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  detect the finger, record the fingerprint image and store it in ImageBuffer after detection, and return to confirm the success of the registration
 *         code.
 * @return Confirmation word                - 0x00 Entry is successful
 *                                            0x02 No finger on the sensor
 *                                            0x03 Entry is unsuccessful
 */
uint8_t getImage(void)
{
    uint8_t content[1] = { FINGERPRINT_GETIMAGE };
    Packet packet = {};
    Packet response = {};
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();

    return response.data[0];
}

/**
 * @brief  Read the module's status register and system basic configuration parameters
 * @return Confirmation word and basic parameters
 * @note   Confirmation word can have values - 0x00 Operation successful
 *                                             0x01 Error receiving package
 *         Parameter        -      index - size(word)
 *         Status register           0          1
 *         System ID                 1          1
 *         Fingerpr. lib. size       2          1
 *         Security lvl              3          1
 *         Device address            4          2
 *         Packet size               6          1
 *         Baud rate                 7          1
 */
SensorParams getParameters(void)
{
    SensorParams params;
    uint8_t content[1] = {};
    Packet packet = {};
    Packet response = {};

    content[0] = FINGERPRINT_READSYSPARAM;
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 1);
    sendPacket(&packet);
    response = awaitReponsePacket();

    params.status_reg = ((uint16_t) response.data[1] << 8) | response.data[2];
    params.system_id = ((uint16_t) response.data[3] << 8) | response.data[4];
    params.capacity = ((uint16_t) response.data[5] << 8) | response.data[6];
    params.security_level = ((uint16_t) response.data[7] << 8) | response.data[8];
    params.device_addr = ((uint32_t) response.data[9] << 24) | ((uint32_t) response.data[10] << 16)|
                  ((uint32_t) response.data[11] << 8) | (uint32_t) response.data[12];
    params.packet_len = ((uint16_t) response.data[13] << 8) | response.data[14];
    if (params.packet_len == 0)
    {
        params.packet_len = 32;
    }
    else if (params.packet_len == 1)
    {
        params.packet_len = 64;
    }
    else if (params.packet_len == 2)
    {
        params.packet_len = 128;
    }
    else if (params.packet_len == 3)
    {
        params.packet_len = 256;
    }
    params.baud_rate = (((uint16_t) response.data[15] << 8) | response.data[16]) * 9600;

    return params;
}

/**
 * @brief  Verify the module handshake password
 * @param  password                         - Password to verify
 * @return Confirmation word                - 0x00 Password is correct
 *                                            0x13 Password is incorrect
 */
uint8_t checkPassword(uint32_t password)
{
    Packet response = {};
    Packet packet = {};
    uint8_t content[5] = {};

    content[0] = FINGERPRINT_VERIFYPASSWORD;
    content[1] = (uint8_t) (password >> 24);
    content[2] = (uint8_t) (password >> 16);
    content[3] = (uint8_t) (password >> 8);
    content[3] = (uint8_t) (password);
    packet = createPacket(SENSOR_ADDRESS, FINGERPRINT_COMMANDPACKET, content, 5);
    sendPacket(&packet);
    response = awaitReponsePacket();
    return response.data[0];
}

/**
 * @brief  Calculate and return checksum of the provided packet
 * @param  packet - Pointer to the packet
 * @return Checksum of the provided packet
 */
uint16_t calculateChecksum(Packet *packet)
{
    uint16_t calculatedSum = 0;
    uint16_t contentSum = 0;
    int i = 0;
    for (i = 0; i < packet->length - 0x2; i++)
    {
        contentSum += packet->data[i];
    }
    calculatedSum = packet->type + packet->length + contentSum;
    return calculatedSum;
}
