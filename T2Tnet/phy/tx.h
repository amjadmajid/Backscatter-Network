#ifndef TX_H_
#define TX_H_

#include "radio.h"
#include "custom_data_type.h"

void backscatter(bool phaseShift);
void __backscatter_1();
void __backscatter_0();
void __backscatter_byte(uint8_t byte);
void backscatter_frame();
void create_frame(uint8_t receiverId, MessageType messageType, uint8_t *payloadPtr);

#endif	// end the TX_H_
