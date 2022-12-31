#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <adc.h>
#include <lcd.h>
#include <ts.h>

#define PX_ERROR    (7)

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
    PDATE = (1 << 4) | (1 << 5) | (0 << 6) | (1 << 7);
    sw_delay_ms( 1 );
    ts_on();
    ts_calibrate();
    ts_off();
}

void ts_on( void )
{
    adc_on();
    state = ON;
}

void ts_off( void )
{
    adc_off();
    state = OFF;
}

uint8 ts_status( void )
{
    return state;
}

uint8 ts_pressed( void )
{
    return (PDATG & (1 << 2)) ? 0 : 1;
}

static void ts_calibrate( void )
{
    uint16 x, y;
    
    lcd_on();
    do {    
        // BORRAR PANTALLA
        lcd_clear();
        // MOSTRAR MENSAJE DE CALIBRACION
        lcd_puts(100, 112, BLACK, "Calibrando...");
        sw_delay_s(2);
        //PINTA CUADRADO 5X5 PX EN (0,0) Y SOLICITA PRESIONAR TOUCHSCREEN
        lcd_clear();
        lcd_puts(64, 128, BLACK, "Presione en el cuadrado");
        lcd_draw_box(0, 0, 5, 5, BLACK, 1);
    
        while(!ts_pressed()); // ESPERAMOS PRESION DE TOUCHSCREEN
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmin, &Vymax );
        while( ts_pressed() ); // ESPERAMOS DEPRESION DE TOUCHSCREEN
        sw_delay_ms( TS_UP_DELAY );

        //PINTA CUADRADO 5X5 PX EN (319,239) Y SOLICITA PRESIONAR TOUCHSCREEN
        lcd_clear();
        lcd_puts(64, 128, BLACK, "Presione en el cuadrado");
        lcd_draw_box(319-5, 239-5, 319, 239, BLACK, 1);
           
        while(!ts_pressed()); // ESPERAMOS PRESION DE TOUCHSCREEN
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmax, &Vymin );
        while( ts_pressed() ); // ESPERAMOS DEPRESION DE TOUCHSCREEN
        sw_delay_ms( TS_UP_DELAY );
    
        //PINTA CUADRADO 5X5 PX EN (160,120) Y SOLICITA PRESIONAR TOUCHSCREEN
        lcd_clear();
        lcd_puts(80, 128, BLACK, "Presione en el cuadrado");
        lcd_draw_box(158, 118, 163, 123, BLACK, 1);

        ts_getpos( &x, &y );

    } while( (x > LCD_WIDTH/2+PX_ERROR) || (x < LCD_WIDTH/2-PX_ERROR) || (y > LCD_HEIGHT/2+PX_ERROR) || (y < LCD_HEIGHT/2-PX_ERROR) );
    lcd_clear();
}

void ts_getpos( uint16 *x, uint16 *y )
{
    uint16 Vx, Vy;
    while ( !ts_pressed() );
    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );
    while ( ts_pressed() );
    sw_delay_ms( TS_UP_DELAY );
    ts_sample2coord( Vx, Vy, x, y );
}

void ts_getpostime( uint16 *x, uint16 *y, uint16 *ms )
{
    uint16 Vx, Vy;
    while ( !ts_pressed() );
    timer3_start();
    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );

    while ( ts_pressed() );
    *ms = timer3_stop() / 10;
    sw_delay_ms( TS_UP_DELAY );
    ts_sample2coord( Vx, Vy, x, y );
}

uint8 ts_timeout_getpos( uint16 *x, uint16 *y, uint16 ms )
{
    uint16 Vx, Vy;
    timer3_start();
    while ( !ts_pressed() )
    {
        if (timer3_timeout())
            return TS_TIMEOUT;
    }
    sw_delay_ms( TS_DOWN_DELAY );
    ts_scan( &Vx, &Vy );
    while ( ts_pressed() );
    sw_delay_ms( TS_UP_DELAY );
    ts_sample2coord( Vx, Vy, x, y );
    return TS_OK; 
}

static void ts_scan( uint16 *Vx, uint16 *Vy )
{
    PDATE = (0 << 4) | (1 << 5) | (1 << 6) | (0 << 7); // X- tierra, X+ VDD, resto abiertos
    *Vx = adc_getSample(1);
    
    PDATE = (1 << 4) | (0 << 5) | (0 << 6) | (1 << 7); // Y- tierra, Y+ VDD, resto abiertos
    *Vy = adc_getSample(0);
    
    PDATE = (1 << 4) | (1 << 5) | (0 << 6) | (1 << 7); // Y- tierra, resto abiertos
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
    
    if( Vy < Vymin )
        *y = LCD_HEIGHT-1;
    else if( Vy > Vymax )
        *y = 0;
    else 
        *y = LCD_HEIGHT*(Vymax - Vy) / (Vymax-Vymin);
}

void ts_open( void (*isr)(void) )
{
    pISR_TS = (uint32)isr;
    INTPND &= ~(BIT_EINT2);
    INTMSK &= ~(BIT_GLOBAL | BIT_EINT2);
}

void ts_close( void )
{
    INTMSK |= BIT_EINT2;
    pISR_TS = (uint32)isr_TS_dummy;
}
