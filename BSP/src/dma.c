/*
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
    pISR_BDMA0 = ...;
    I_ISPC     = ...;
    INTMSK    &= ...;
}

void bdma0_close( void )
{
    INTMSK    |= ...;
    pISR_BDMA0 = ...;
}
*/