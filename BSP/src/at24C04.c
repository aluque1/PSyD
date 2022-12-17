#include <s3c44b0x.h>
#include <at24c04.h>
#include <iic.h>

#define DEVICE_ADDR  (( 0xA << 4) | (0 << 1))

#define READ  (1)
#define WRITE (0)

void at24c04_bytewrite( uint16 addr, uint8 data )
{
    static uint8 page;
    
    page = (addr & 0x100) >> 8; 

    iic_start( IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE );
    iic_putByte( addr & 0xFF );
    iic_putByte( data );
    iic_stop( 5 );
}
    
void at24c04_byteread( uint16 addr, uint8 *data )
{
    static uint8 page;
    
    page = (addr & 0x100) >> 8; 

    iic_start(IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE);
    iic_putByte(addr & 0xFF);
    iic_start(IIC_Rx, DEVICE_ADDR | (page << 1) | READ);
    *data = iic_getByte(0);
    iic_stop( 5 );

}

void at24c04_load(uint8 *buffer )
{
    static uint16 addr = 0;
    static uint8 page = 0;
    static uint16 index;

    page = (addr & 0x100) >> 8;
    iic_start(IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE);
    iic_putByte(addr & 0xFF);
    iic_start(IIC_Rx, DEVICE_ADDR | (page << 1) | READ);

    for (index = 0; index < AT24C04_DEPTH; ++index)
    {
        buffer[index] = iic_getByte(index < (AT24C04_DEPTH - 1));
    }

    iic_stop( 5 );
}

void at24c04_store( uint8 *buffer )
{ 
    static uint16 addr = 0;
    static uint8 page = 0;
    static uint16 index = 0;
    static uint8 i;
    static uint8 j;

    for (i = 0; i < 32; ++i)
    {
        page = addr >> 8;
        iic_start(IIC_Tx, DEVICE_ADDR | (page << 1) | WRITE);
        iic_putByte(addr & 0xFF);
        for (j = 0; j < 16; j++)
        {
            iic_putByte(buffer[index]);
            ++index;
        }
        iic_stop( 5 );
        addr += 16;  
    }
}


void at24c04_clear( void )
{
    uint8 buffer[AT24C04_DEPTH] = {0};
    at24c04_store(buffer);
}   
