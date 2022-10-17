/*
#include <s3c44b0x.h>
#include <at24c04.h>
#include <iic.h>

#define DEVICE_ADDR  (( 0xA << 4) | (0 << 1))

#define READ  (1)
#define WRITE (0)

void at24c04_clear( void )
{
    ...
}

void at24c04_bytewrite( uint16 addr, uint8 data )
{
    uint8 page;
    
    page = (addr & 0x100) >> 8; 

    iic_start( IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE );
    iic_putByte( addr & 0xFF );
    iic_putByte( data );
    iic_stop( 5 );
}
    
void at24c04_byteread( uint16 addr, uint8 *data )
{
    ...
}

void at24c04_load( uint8 *buffer )
{
    ...
}

void at24c04_store( uint8 *buffer )
{ 
    ...
}
*/