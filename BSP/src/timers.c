/*
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>

extern void isr_TIMER0_dummy( void );

static uint32 loop_ms = 0;
static uint32 loop_s = 0;

static void sw_delay_init( void );

void timers_init( void )
{
    TCFG0 = ...;
    TCFG1 = ...;

    TCNTB0 = ...;
    TCMPB0 = ...;
    TCNTB1 = ...;
    TCMPB1 = ...;
    TCNTB2 = ...;
    TCMPB2 = ...;
    TCNTB3 = ...;
    TCMPB3 = ...;
    TCNTB4 = ...;
    TCMPB4 = ...;    
    TCNTB5 = ...;

    TCON = ...;
    TCON = ...;

    sw_delay_init();
}

static void sw_delay_init( void )
{
    uint32 i;
    
    timer3_start();
    for( i=1000000; i; i--);
    loop_s = ((uint64)1000000*10000)/timer3_stop();
    loop_ms = loop_s / 1000;
};

void timer3_delay_ms( uint16 n )
{
    for( ; n; n-- )
    {
        ...
    }
}

void sw_delay_ms( uint16 n )
{
    uint32 i;
    
    for( i=loop_ms*n; i; i-- );
}

void timer3_delay_s( uint16 n )
{
    ...
}

void sw_delay_s( uint16 n )
{
    uint32 i;
    
    for( i=loop_s*n; i; i-- );
}

void timer3_start( void ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);    
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = 0xffff; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_stop( void )
{
    TCON &= ~(1 << 16);
    return 0xffff - TCNTO3;
}

void timer3_start_timeout( uint16 n ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);          
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = n; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_timeout( )
{
    return !TCNTO3;
}    

void timer0_open_tick( void (*isr)(void), uint16 tps )
{
    pISR_TIMER0 = ...;
    I_ISPC      = ...;
    INTMSK     &= ...;

    if( tps > 0 && tps <= 10 ) {
        TCFG0  = ...;
        TCFG1  = ...;
        TCNTB0 = (40000U / tps);
    } else if( tps > 10 && tps <= 100 ) {
        TCFG0  = ...;
        TCFG1  = ...;
        TCNTB0 = (400000U / (uint32) tps);
    } else if( tps > 100 && tps <= 1000 ) {
        TCFG0  = ...;
        TCFG1  = ...;
        TCNTB0 = (4000000U / (uint32) tps);
    } else if ( tps > 1000 ) {
        TCFG0  = ...;
        TCFG1  = ...;
        TCNTB0 = (32000000U / (uint32) tps);
    }

    TCON = ...;
    TCON = ...;
}

void timer0_open_ms( void (*isr)(void), uint16 ms, uint8 mode )
{
    pISR_TIMER0 = ...;
    I_ISPC      = ...;
    INTMSK     &= ...;

    TCFG0 = ...;
    TCFG1 = ...;
    TCNTB0 = 10*ms;

    TCON = ...;
    TCON = ...;
}

void timer0_close( void )
{
    TCNTB0 = ...;
    TCMPB0 = ...;

    TCON = ...;
    TCON = ...;
    
    INTMSK     |= ...;
    pISR_TIMER0 = ...;
}
*/
