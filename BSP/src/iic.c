/*
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <iic.h>

void iic_init( void )
{
    ...
}

void iic_start( uint8 mode, uint8 byte )
{
    IICDS   = ...;
    IICSTAT = ...;
    IICCON &= ...;
    while( ... );
}


void iic_putByte( uint8 byte )
{
    IICDS   = ...;
    IICCON &= ...;
    while( ... );    
}

uint8 iic_getByte( uint8 ack )
{
    IICCON  = ...;
    IICCON &= ...;
    while( ... );
    return ...;
}

void iic_stop( uint16 ms )
{
    IICSTAT &= ...;
    IICCON  &= ...;
    sw_delay_ms( ms );
}
*/