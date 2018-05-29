#ifndef BUFFERS_H_
#define BUFFERS_H_

#include <stdbool.h>
#include <stdint.h>                 // needed for the uint8_t data type
#include <stdlib.h>                 // needed for the size_t and malloc
#include "custom_data_type.h"

#define RX_BUFFER_SIZE		 			64						// Buffer size in frames
#define TX_BUFFER_SIZE		 			8						// Buffer size in frames


/** functions prototyping */
void rbuf_init(rbuf_t *rbuf, uint8_t rbuf_size);
void _advance_idx(size_t *idx, size_t *size);
bool rbuf_empty(rbuf_t* rbuf);
bool rbuf_full(rbuf_t *rbuf);
void rbuf_put( rbuf_t *rbuf, uint8_t data);
void rbuf_write( rbuf_t *rbuf, uint8_t numBytes, uint8_t *data);
/*------------------------------------------------------------------------------*/ 

#endif //
