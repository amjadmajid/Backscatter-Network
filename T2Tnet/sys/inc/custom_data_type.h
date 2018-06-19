/*
 * custom_data_type.h
 *
 *  Created on: 28 May 2018
 *      Author: amjad
 */


#ifndef SYS_CUSTOM_DATA_TYPE_H_
#define SYS_CUSTOM_DATA_TYPE_H_

#include <stdlib.h>

/* Declare a data type that holds the characteristics of a ring buffer */
typedef struct {
    uint8_t *buffer;                // a pointer to hold the address to the ring buffer
    size_t size;                    // `size_t` is used here to represent buffer
                                    // object size in byte. `malloc` expects this data type
    size_t data_len;                // capture the length of the valid
    size_t head;                    // the start index of the valid data
    size_t tail;                    // the end index of the valid data
}rbuf_t;
/*-----------------------------------------------------------------------------*/

/* Define MessageType for incoming and outgoing frames */
typedef enum {
    M_DATA,
    M_ACKNOWLEDGMENT
} MessageType;

typedef void* (*func_ptr)();

typedef enum {
    E_SUCCESS,
    E_FAILURE,
    E_CHANNEL_FREE,
    E_CHANNEL_BUSY,
    E_NO_PACKETS,
    E_WAITING_PACKETS,
    E_BUFFER_EMPTY,
    E_BUFFER_FULL
} error_t;

#endif /* SYS_CUSTOM_DATA_TYPE_H_ */
