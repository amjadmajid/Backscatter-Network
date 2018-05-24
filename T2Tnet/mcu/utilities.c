#include <utilities.h>

const unsigned int CRC_Init = 0xFFFF;

/**  
 * @description This function calculate the CRC.
 * @param		checksum a value to calculate the CRC for it. 
 * @return		It returns the CRC value 
-----------------------------------------------------------------------------*/
uint16_t calCRC(uint16_t checksum)
{
    CRCINIRES = CRC_Init;                     // Init CRC16 HW module
    CRCDIRB = checksum;                 // Input data in CRC
    __no_operation();
    uint16_t resultCRC = CRCINIRES;
    return resultCRC;
}
