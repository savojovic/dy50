#include "dy50.h"
#include "tm4c123gxl_utils.h"
#include "config.h"
#include <stdbool.h>

//Enroll
int main(void)
{
    init();
    LEDcontrol(true);
    UARTprintf("Enter id of a fingerprint: ");
    unsigned char id = UARTgetc();
    UARTprintf("You entered %c\n", id);
    int p = -1;
    UARTprintf("Place your finger on the sensor.\n");
    while(p != FINGERPRINT_OK)
    {
        p = getImage(); // Capture the fingerprint image
        UARTprintf("p = %d\n", p);
    }
    UARTprintf("Image taken.\n");
    UARTprintf("Storing the image in CharBuffer 1.\n");
    p = image2Tz(1);          // Store it in the CharBuffer 1
    if( p == FINGERPRINT_OK)
    {
        UARTprintf("Image converted.\n");
        p = -1;
    }
    else
    {
        UARTprintf("p = %d\nExiting!", p);
        return -1;
    }
    UARTprintf("Remove your finger from the sensor.\n");
    while (p != FINGERPRINT_NOFINGER)
    {
      p = getImage();
    }
    UARTprintf("Place the same finger again\n");
    while(p != FINGERPRINT_OK)
    {
        p = getImage(); // Capture the fingerprint image
        UARTprintf("p = %d\n", p);
    }
    UARTprintf("Image taken.\n");
    UARTprintf("Storing the image in CharBuffer 2.\n");
    p = image2Tz(2);          // Store it in the CharBuffer 2
    if( p == FINGERPRINT_OK)
    {
        UARTprintf("Image converted.\n");
        p = -1;
    }
    else
    {
        UARTprintf("p = %d\n Exiting!", p);
        return -1;
    }
    UARTprintf("Creating model for ID %d\n", id);
    p = createModel();
    if(p == FINGERPRINT_OK)
    {
        UARTprintf("The two finger prints matched, model created.\n");
        p = -1;
    }
    else
    {
        UARTprintf("Prints did not match.\nExiting\n");
        return -1;
    }
    UARTprintf("Storing model.\n");
    p = storeModel(1, id);
    if(p == FINGERPRINT_OK)
    {
        UARTprintf("Model stored!\n");
    }
    else
    {
        UARTprintf("p = %d\nExiting!\n", p);
        return -1;
    }
    return 0;
}
//
////Search
//int main(void)
//{
//    init();
//    UARTprintf("Place your finger on the sensor.\n");
//    int p = -1;
//    while(p!= FINGERPRINT_OK)
//    {
//        p = getImage();
//        UARTprintf("p= %d\n", p);
//    }
//    p=-1;
//    UARTprintf("Image taken.\n");
//    p = image2Tz(1);
//    if(p==FINGERPRINT_OK)
//    {
//        UARTprintf("Image converted.");
//        p = -1;
//    }
//    else
//    {
//        UARTprintf("Could not match.\nExiting.\n");
//        return -1;
//    }
//    FingerPageAndConfidence fingerprint = fingerSearch(1);
//    if(fingerprint.statusCode == FINGERPRINT_OK)
//    {
//        UARTprintf("Fingerprint found!\n");
//        UARTprintf("Found by ID %d\n", fingerprint.fingerprintPage);
//        UARTprintf("Confidence %d\n", fingerprint.confidence);
//        if(fingerprint.fingerprintPage == 2)
//        {
//            UARTprintf("Monika<3\n");
//        }
//        if(fingerprint.fingerprintPage == 1)
//        {
//            UARTprintf("Jovo\n");
//        }
//    }
//
//}
