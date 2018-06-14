#ifndef BUFFERS_H_
#define BUFFERS_H_

#include <stdlib.h>
#include <custom_data_type.h>

#define RX_BUFFER_SIZE		FRAME_LENGTH * 6					// Buffer size in bytes
#define TX_BUFFER_SIZE		FRAME_LENGTH * 2					// Buffer size in bytes


/** functions prototyping */
void rbuf_init(rbuf_t *rbuf, uint8_t rbuf_size);
void _advance_idx(size_t *idx, size_t *size);
bool rbuf_empty(rbuf_t* rbuf);
bool rbuf_full(rbuf_t *rbuf);
void rbuf_put( rbuf_t *rbuf, uint8_t data);
void rbuf_write( rbuf_t *rbuf, uint8_t *data, uint8_t numBytes);
error_t rbuf_read( uint8_t *data, rbuf_t *rbuf, uint8_t numBytes);
/*------------------------------------------------------------------------------*/ 

#endif //
