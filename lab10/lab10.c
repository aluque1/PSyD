/*-------------------------------------------------------------------
**
**  Fichero:
**    lab10.c  11/3/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Test del laboratorio 10
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <common_types.h>
#include <system.h>
#include <uart.h>
#include <timers.h>
#include <lcd.h>
#include <ts.h>

void lcd_draw_grid( void );

void main( void )
{
    uint16 x, y, ms;

    sys_init();
    uart0_init();
    timers_init();
    lcd_init();
    ts_init();
    
    lcd_clear();
    lcd_on();
    ts_on();
    lcd_draw_grid();
    
    uart0_puts( "\n\nPulse la pantalla LCD:\n" );

    while( 1 )
    {
        ts_getpostime( &x, &y, &ms );
        uart0_puts( "  - Ha pulsado en la posicion (" );
        uart0_putint( x );
        uart0_puts( ", " );
        uart0_putint( y );
        uart0_puts( ") durante " );
        uart0_putint( ms );
        uart0_puts( " ms\n" );
        if( x>0 && y>0 && x<79 && y<59 )
        {
            lcd_putchar_x2(  32,  16, WHITE, '0' );    
            sw_delay_ms( 200 );
            lcd_putchar_x2(  32,  16, BLACK, '0' );    
        } 
        else if( x>79 && y>0 && x<159 && y<59 )
        {
            lcd_putchar_x2( 112,  16, WHITE, '1' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 112,  16, BLACK, '1' );
        } 
        else if( x>159 && y>0 && x<239 && y<59 )
        {
            lcd_putchar_x2( 192,  16, WHITE, '2' );        
            sw_delay_ms( 200 );
            lcd_putchar_x2( 192,  16, BLACK, '2' );
        }
        else if( x>239 && y>0 && x<319 && y<59 )
        {        
            lcd_putchar_x2( 272,  16, WHITE, '3' );        
            sw_delay_ms( 200 );
            lcd_putchar_x2( 272,  16, BLACK, '3' );
        }
        else if( x>0 && y>59 && x<79 && y<119 )
        {                    
            lcd_putchar_x2(  32,  76, WHITE, '4' );
            sw_delay_ms( 200 );
            lcd_putchar_x2(  32,  76, BLACK, '4' );                            
        }
        else if( x>79 && y>59 && x<159 && y<119 )
        {    
            lcd_putchar_x2( 112,  76, WHITE, '5' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 112,  76, BLACK, '5' );
        }
        else if( x>159 && y>59 && x<239 && y<119 )
        {
            lcd_putchar_x2( 192,  76, WHITE, '6' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 192,  76, BLACK, '6' );
        }
        else if( x>239 && y>59 && x<319 && y<119 )
        {        
            lcd_putchar_x2( 272,  76, WHITE, '7' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 272,  76, BLACK, '7' );
        }
        else if( x>0 && y>119 && x<79 && y<179 )
        {        
            lcd_putchar_x2(  32, 136, WHITE, '8' );
            sw_delay_ms( 200 );
            lcd_putchar_x2(  32, 136, BLACK, '8' );
        }
        else if( x>79 && y>119 && x<159 && y<179 )
        {
            lcd_putchar_x2( 112, 136, WHITE, '9' );        
            sw_delay_ms( 200 );
            lcd_putchar_x2( 112, 136, BLACK, '9' );        
        }
        else if( x>159 && y>119 && x<239 && y<179 )
        {
            lcd_putchar_x2( 192, 136, WHITE, 'A' );        
            sw_delay_ms( 200 );
            lcd_putchar_x2( 192, 136, BLACK, 'A' );        
        }
        else if( x>239 && y>119 && x<319 && y<179 )
        {
            lcd_putchar_x2( 272, 136, WHITE, 'B' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 272, 136, BLACK, 'B' );
        }
        else if( x>0 && y>179 && x<79 && y<239 )
        {
            lcd_putchar_x2(  32, 196, WHITE, 'C' );
            sw_delay_ms( 200 );
            lcd_putchar_x2(  32, 196, BLACK, 'C' );
        }
        else if( x>79 && y>179 && x<159 && y<239 )
        {
            lcd_putchar_x2( 112, 196, WHITE, 'D' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 112, 196, BLACK, 'D' );
        }
        else if( x>159 && y>179 && x<239 && y<239 )
        {
            lcd_putchar_x2( 192, 196, WHITE, 'E' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 192, 196, BLACK, 'E' );
        }
        else if( x>239 && y>179 && x<319 && y<239 )
        {
            lcd_putchar_x2( 272, 196, WHITE, 'F' );
            sw_delay_ms( 200 );
            lcd_putchar_x2( 272, 196, BLACK, 'F' );
        }
    }
}

void lcd_draw_grid(void)
{

    lcd_draw_box(   0,   0,  79,  59, BLACK, 1 ); /* Pinta cuadricula */
    lcd_draw_box(  79,   0, 159,  59, BLACK, 1 );
    lcd_draw_box( 159,   0, 239,  59, BLACK, 1 );
    lcd_draw_box( 239,   0, 319,  59, BLACK, 1 );
    lcd_draw_box(   0,  59,  79, 119, BLACK, 1 );
    lcd_draw_box(  79,  59, 159, 119, BLACK, 1 );
    lcd_draw_box( 159,  59, 239, 119, BLACK, 1 );
    lcd_draw_box( 239,  59, 319, 119, BLACK, 1 );
    lcd_draw_box(   0, 119,  79, 179, BLACK, 1 );
    lcd_draw_box(  79, 119, 159, 179, BLACK, 1 );
    lcd_draw_box( 159, 119, 239, 179, BLACK, 1 );
    lcd_draw_box( 239, 119, 319, 179, BLACK, 1 );
    lcd_draw_box(   0, 179,  79, 239, BLACK, 1 );
    lcd_draw_box(  79, 179, 159, 239, BLACK, 1 );
    lcd_draw_box( 159, 179, 239, 239, BLACK, 1 );
    lcd_draw_box( 239, 179, 319, 239, BLACK, 1 );

    lcd_putchar_x2(  32,  16, BLACK, '0' );    /* Rotula cuadricula */
    lcd_putchar_x2( 112,  16, BLACK, '1' );
    lcd_putchar_x2( 192,  16, BLACK, '2' );
    lcd_putchar_x2( 272,  16, BLACK, '3' );
    lcd_putchar_x2(  32,  76, BLACK, '4' );
    lcd_putchar_x2( 112,  76, BLACK, '5' );
    lcd_putchar_x2( 192,  76, BLACK, '6' );
    lcd_putchar_x2( 272,  76, BLACK, '7' );
    lcd_putchar_x2(  32, 136, BLACK, '8' );
    lcd_putchar_x2( 112, 136, BLACK, '9' );
    lcd_putchar_x2( 192, 136, BLACK, 'A' );
    lcd_putchar_x2( 272, 136, BLACK, 'B' );
    lcd_putchar_x2(  32, 196, BLACK, 'C' );
    lcd_putchar_x2( 112, 196, BLACK, 'D' );
    lcd_putchar_x2( 192, 196, BLACK, 'E' );
    lcd_putchar_x2( 272, 196, BLACK, 'F' );

}
