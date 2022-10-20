
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

extern void isr_TICK_dummy( void );

void rtc_init( void )
{
    TICNT   = 0x0;
    RTCALM  = 0x0;
    RTCRST  = 0x0;
        
    RTCCON  = 0x1;
    
    BCDYEAR = 0x25;
    BCDMON  = 0x5;
    BCDDAY  = 0x15;
    BCDDATE = 0x04;
    BCDHOUR = 0x19;
    BCDMIN  = 0x33;
    BCDSEC  = 0x12;

    ALMYEAR = 0x0;
    ALMMON  = 0x0;
    ALMDAY  = 0x0;
    ALMHOUR = 0x0;
    ALMMIN  = 0x0;
    ALMSEC  = 0x0;

    RTCCON &= 0x0;
}

void rtc_puttime( rtc_time_t *rtc_time )
{
    RTCCON |= ...;
    
    BCDYEAR = uint8toBCD(rtc_time->year);
    BCDMON  = uint8toBCD(rtc_time->mon);
    BCDDAY  = uint8toBCD(rtc_time->wday);
    BCDDATE = uint8toBCD(rtc_time->mday);
    BCDHOUR = uint8toBCD(rtc_time->hour);
    BCDMIN  = uint8toBCD(rtc_time->min);
    BCDSEC  = uint8toBCD(rtc_time->sec);
        
    RTCCON &= ...;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
    RTCCON |= ...;
    
    rtc_time->year = ...;
    rtc_time->mon  = ...;
    rtc_time->mday = ...;
    rtc_time->wday = ...;
    rtc_time->hour = ...;
    rtc_time->min  = ...;
    rtc_time->sec  = ...;
    if( ! rtc_time->sec ){
        rtc_time->year = ...;
        rtc_time->mon  = ...;
        rtc_time->mday = ...;
        rtc_time->wday = ...;
        rtc_time->hour = ...;
        rtc_time->min  = ...;
        rtc_time->sec  = ...;
    };

    RTCCON &= ...;
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = ...;
    I_ISPC    = ...;
    INTMSK   &= ...;
    TICNT     = ...;
}

void rtc_close( void )
{
    TICNT     = ...;  
    INTMSK   |= ...;    
    pISR_TICK = (uint32) isr_TICK_dummy;
}
