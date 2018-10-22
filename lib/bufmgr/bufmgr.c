/******************************************************************************

 @file  

 @brief 

 Group: 
 Target Device: 

 ******************************************************************************
 

 ******************************************************************************
 Release Name: 
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bufmgr.h"

extern void    ring_buf_flush( RING_BUF_t *rb )
{
	rb->head = 0;
	rb->tail = 0;
}

extern void    ring_buf_put  ( RING_BUF_t *rb, uint8_t byte )
{
	uint16_t head;
	
	head = rb->head;
	rb->buf[head++] = byte;
	if( head >= rb->size )
		head = 0;
	rb->head = head;
}

extern uint8_t ring_buf_get  ( RING_BUF_t *rb )
{
	uint16_t tail;
	uint8_t byte;
	
	tail = rb->tail;
	byte = rb->buf[tail++];
	if( tail >= rb->size )
		tail = 0;
	rb->tail = tail;
	return byte;
}

extern uint8_t ring_buf_full ( const RING_BUF_t *rb )
{
	uint16_t head;
	head = rb->head;
	head++;
	if( head >= rb->size )
		head = 0;
	return ( head == rb->tail );
}

extern uint8_t ring_buf_empty( const RING_BUF_t *rb )
{
	return ( rb->tail == rb->head );
}
