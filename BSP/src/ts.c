/*
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <adc.h>
#include <lcd.h>
#include <ts.h>

#define PX_ERROR    (5)

static uint16 Vxmin = 0;
static uint16 Vxmax = 0;
static uint16 Vymin = 0;
static uint16 Vymax = 0;

static uint8 state;

extern void isr_TS_dummy( void );

static void ts_scan( uint16 *Vx, uint16 *Vy );
static void ts_calibrate( void );
static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y );

void ts_init( void )
{
    timers_init();  
    lcd_init();
    adc_init();
    PDATE = ...;
    sw_delay_ms( 1 );
    ts_on();
    ts_calibrate();
    ts_off();
}

void ts_on( void )
{
    ...
}

void ts_off( void )
{
    ...
}

uint8 ts_status( void )
{
    ...
}

uint8 ts_pressed( void )
{
    ...
}

static void ts_calibrate( void )
{
    uint16 x, y;
    
    lcd_on();
    do {    
        ...
    
        while( ... );
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmin, &Vymax );
        while( ... );
        sw_delay_ms( TS_UP_DELAY );

        ...    
           
        while( ... );
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmax, &Vymin );
        while( ... ); 
        sw_delay_ms( TS_UP_DELAY );
    
        ...
    
        ts_getpos( &x, &y );      

        ...
    
    } while( (x > LCD_WIDTH/2+PX_ERROR) || (x < LCD_WIDTH/2-PX_ERROR) || (y > LCD_HEIGHT/2+PX_ERROR) || (y < LCD_HEIGHT/2-PX_ERROR) );
    lcd_clear();
}

void ts_getpos( uint16 *x, uint16 *y )
{
    ...
}

void ts_getpostime( uint16 *x, uint16 *y, uint16 *ms )
{
    ...    
}

uint8 ts_timeout_getpos( uint16 *x, uint16 *y, uint16 ms )
{
    ...    
}

static void ts_scan( uint16 *Vx, uint16 *Vy )
{
    PDATE = ...;
    *Vx = adc_getSample( ... );
    
    PDATE = ...;
    *Vy = adc_getSample( ... );
    
    PDATE = ...;
    sw_delay_ms( 1 );
}

static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y )
{
    if( Vx < Vxmin )
        *x = 0;
    else if( Vx > Vxmax )
        *x = LCD_WIDTH-1;
    else 
        *x = LCD_WIDTH*(Vx-Vxmin) / (Vxmax-Vxmin);    
    ...
}

void ts_open( void (*isr)(void) )
{
    ...
}

void ts_close( void )
{
    ...
}
*/