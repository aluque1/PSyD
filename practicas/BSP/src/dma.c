#include <s3c44b0x.h>
#include <s3cev40.h>
#include <dma.h>

extern void isr_BDMA0_dummy( void ); 

void bdma0_init( void )
{
    BDCON0  = 0;
    BDISRC0 = 0;
    BDIDES0 = 0;
    BDICNT0 = 0;
}

void bdma0_open( void (*isr)(void) )
{
    pISR_BDMA0 = (uint32)isr;
    I_ISPC     = BIT_BDMA0;
    INTMSK    &= ~(BIT_GLOBAL | BIT_BDMA0 );
}

void bdma0_close( void )
{
    INTMSK    |= BIT_BDMA0;
    pISR_BDMA0 = (uint32)isr_BDMA0_dummy;
}