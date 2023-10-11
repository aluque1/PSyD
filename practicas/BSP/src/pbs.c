#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( !(PDATG & PB_LEFT) )
        return PB_LEFT;
    else if( !(PDATG & PB_RIGHT) )
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_pressed( void )
{
    if( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT))
        return FALSE;
    else
        return TRUE;
}

uint8 pb_getchar( void )
{
    uint8 pb;
    while( !pb_pressed() );
    sw_delay_ms( PB_KEYDOWN_DELAY );
    pb = pb_scan();
    while( pb_pressed() );
    sw_delay_ms( PB_KEYUP_DELAY );
    return pb;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( !pb_pressed() );
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( pb_pressed() );
    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
    uint8 scancode;

    timer3_start_timeout(ms * 10);
    while( !pb_pressed() )
    {
        if( timer3_timeout() )
            return PB_TIMEOUT;
    }
    sw_delay_ms( PB_KEYDOWN_DELAY );
    scancode = pb_scan();
    while( pb_pressed() );
    sw_delay_ms( PB_KEYUP_DELAY );
    return scancode;
}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32) isr;
    EXTINTPND = 0xF;
    I_ISPC    = BIT_PB;
    INTMSK   &= ~(BIT_GLOBAL | BIT_PB);
}

void pbs_close( void )
{
    INTMSK  |= BIT_PB;
    pISR_PB  = (uint32) isr_PB_dummy;
}
