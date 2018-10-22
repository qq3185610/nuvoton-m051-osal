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
#ifndef __BUFMGR_H__
#define __BUFMGR_H__

#include <stdint.h>

typedef struct ring_buf_t {
	uint16_t size;
	uint16_t head;
	uint16_t tail;
	uint8_t *buf;
} RING_BUF_t;

extern void    ring_buf_flush( RING_BUF_t *rb );
extern void    ring_buf_put  ( RING_BUF_t *rb, uint8_t byte );
extern uint8_t ring_buf_get  ( RING_BUF_t *rb );
extern uint8_t ring_buf_full ( const RING_BUF_t *rb );
extern uint8_t ring_buf_empty( const RING_BUF_t *rb );

#endif

