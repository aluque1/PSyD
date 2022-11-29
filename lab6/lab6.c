/*-------------------------------------------------------------------
**
**  Fichero:
**    lab6.c  28/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Test del laboratorio 6
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <uart.h>
#include <system.h>
#include <timers.h>
#include <keypad.h>
#include <pbs.h>

void isr_keypad( void ) __attribute__ ((interrupt ("IRQ")));
void isr_pbs( void ) __attribute__ ((interrupt ("IRQ")));
void isr_timer0( void ) __attribute__ ((interrupt ("IRQ")));

void one_second_loop( void );

volatile boolean flagTimer0;
volatile boolean flagKeypad;
volatile boolean flagPbs;

void main( void )
{
    uint16 ms, ticks, time;
    uint8 scancode;

    sys_init();
    uart0_init();
    timers_init();
    keypad_init();
    pbs_init();
    
      /***********************************/

    uart0_puts( "\n\n" );
    uart0_puts( "TEST DE TEMPORIZADORES\n" );
    uart0_puts( "----------------------\n\n" );
    
    if( ((PLLCON & ~(0x3 << 10 ) & ~(0x3 << 2)) != 0x38021) || ((CLKSLOW & ~(0xf)) != 0) )
    {
        uart0_puts( "  - Error en la configuración del gestor de reloj y energia\n" );
        while( 1 );
    } 
    
    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de un bucle con 1 s de tiempo de ejecución aproximada...\n" );
    timer3_start() ;
    one_second_loop();
    ms = timer3_stop() / 10;
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" );
   
    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por sw_delay_ms()...\n" );
    timer3_start() ;
    sw_delay_ms( 1000 );
    ms = timer3_stop() / 10;
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" );
   
    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por sw_delay_s()...\n" );
    timer3_start() ;
    sw_delay_s( 1 );
    ms = timer3_stop() / 10;
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" );
    
    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por timer0_open_ms() en modo TIMER_ONE_SHOT...\n" );
    timer3_start() ;
    timer0_open_ms( isr_timer0, 1000, TIMER_ONE_SHOT );
    flagTimer0 = FALSE;
    while( !flagTimer0 );
    ms = timer3_stop() / 10;
    timer0_close();
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" );  
    
    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por timer0_open_ms() en modo TIMER_INTERVAL...\n" );
    timer3_start() ;
    timer0_open_ms( isr_timer0, 10, TIMER_INTERVAL );
    flagTimer0 = FALSE;
    ticks = 0;
    while( ticks != 100 )
    {
        while( !flagTimer0 );
        flagTimer0 = FALSE;
        ticks++;
    }
    ms = timer3_stop() / 10;
    timer0_close();
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" ); 

    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por timer0_open_tick() con tps=1...\n" );
    timer3_start() ;
    timer0_open_tick( isr_timer0, 1 );
    flagTimer0 = FALSE;
    while( !flagTimer0 );
    ms = timer3_stop() / 10;
    timer0_close();
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" ); 

    uart0_puts( "  - Medida con timer3_start() / timer3_stop() del retardo de 1 s generado por timer0_open_tick() con tps=100...\n" );
    timer3_start() ;
    timer0_open_tick( isr_timer0, 100 );
    flagTimer0 = FALSE;
    ticks = 0;
    while( ticks != 100 )
    {
        while( !flagTimer0 );
        flagTimer0 = FALSE;
        ticks++;
    }
    ms = timer3_stop() / 10;
    timer0_close();
    uart0_puts( "    ... retardo medido: " );
    uart0_putint( ms );
    uart0_puts( " ms\n" ); 

    uart0_puts( "  - Retardo hardware de 5 s usando timer3_delay_ms(): mídalo...\n" );
    timer3_delay_ms( 5000 );
    uart0_puts( "    ... fin\n" );

    uart0_puts( "  - Retardo hardware de 5 s usando timer3_delay_s(): mídalo...\n" );
    timer3_delay_s( 5 );    
    uart0_puts( "    ... fin\n" );

    uart0_puts( "  - Retardo hardware de 5 s usando timer3_start_timeout(): mídalo...\n" );
    timer3_start_timeout( 50000 );
    while( !timer3_timeout() );
    uart0_puts( "    ... fin\n" );

    /************************************/
    
    uart0_puts( "\n\n" );
    uart0_puts( "TEST DE KEYPAD Y PBS\n" );
    uart0_puts( "--------------------\n\n" );
    
    if( pb_pressed() )
    {
        uart0_puts( "  - Error en la función pb_pressed()\n" );
        while( 1 );
    }      

    if( keypad_pressed() )
    {
        uart0_puts( "  - Error en la función keypad_pressed()\n" );
        while( 1 );
    }      

    uart0_puts( "Lectura del keypad sin espera máxima usando keypad_getchar():\n" );
    ticks = 0;
    while( ticks != 5 )
    {
        switch( scancode = keypad_getchar() )
        {
            case KEYPAD_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            default:
                uart0_puts( "  - La tecla 0x" );
                uart0_puthex( scancode );
                uart0_puts( " ha sido presionada\n" );
                break;
        }        
        ticks++;
    }    
          
    uart0_puts( "Lectura del keypad con espera máxima de 5 s usando keypad_timeout_getchar():\n" );
    ticks = 0;
    while( ticks != 5 )
    {
        switch( scancode = keypad_timeout_getchar( 5000 ) )
        {
            case KEYPAD_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            case KEYPAD_TIMEOUT:
                uart0_puts( "  - Timeout\n" );
                break;
            default:
                uart0_puts( "  - La tecla 0x" );
                uart0_puthex( scancode );
                uart0_puts( " ha sido presionada\n" );
                break;
        }          
        ticks++;
    }     
    
    uart0_puts( "Lectura del keypad por interrupción usando keypad_getchartime():\n" );
    keypad_open( isr_keypad );
    flagKeypad = FALSE;
    ticks = 0;
    while( ticks != 5 )
    {
        while( !flagKeypad );
        flagKeypad = FALSE;
        switch( scancode = keypad_getchartime( &time ) )
        {
            case KEYPAD_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            default:
                uart0_puts( "  - La tecla 0x" );
                uart0_puthex( scancode );
                uart0_puts( " ha sido presionada durante " );
                uart0_putint( time );
                uart0_puts( " ms \n" );                
        }        
        ticks++;
    } 
    keypad_close();
    
    uart0_puts( "Lectura de pulsadores sin espera máxima usando pb_getchar():\n" );
    ticks = 0;
    while( ticks != 5 )
    {
        switch( pb_getchar() )
        {
            case PB_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            case PB_LEFT:
                uart0_puts( "  - El pulsador izquierdo ha sido presionado\n" );
                break;
            case PB_RIGHT:
                uart0_puts( "  - El pulsador derecho ha sido presionado\n" );
                break;
        }        
        ticks++;
    }    
    
    uart0_puts( "Lectura de pulsadores con espera máxima de 5 s usando pb_timeout_getchar():\n" );
    ticks = 0;
    while( ticks != 5 )
    {
        switch( pb_timeout_getchar( 5000 ) )
        {
            case PB_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            case PB_TIMEOUT:
                uart0_puts( "  - Timeout\n" );
                break;
            case PB_LEFT:
                uart0_puts( "  - El pulsador izquierdo ha sido presionado\n" );
                break;
            case PB_RIGHT:
                uart0_puts( "  - El pulsador derecho ha sido presionado\n" );
                break;
        }          
        ticks++;
    }     
       
    uart0_puts( "Lectura de pulsadores por interrupción usando pb_getchartime():\n" );
    pbs_open( isr_pbs );
    flagPbs = FALSE;
    ticks = 0;
    while( ticks != 5 )
    {
        while( !flagKeypad );
        flagPbs = FALSE;
        switch( pb_getchartime( &time ) )
        {
            case PB_FAILURE:
                uart0_puts( "  - Error de lectura\n" );
                break;
            case PB_LEFT:
                uart0_puts( "  - El pulsador izquierdo ha sido presionado durante " );
                uart0_putint( time );
                uart0_puts( " ms \n" );
                break;
            case PB_RIGHT:
                uart0_puts( "  - El pulsador derecho ha sido presionado durante " );
                uart0_putint( time );
                uart0_puts( " ms \n" );
                break;

        }       
        ticks++;
    } 
    pbs_close();
    
    while( 1 );

}

void one_second_loop( void )
{
    uint32 i;

    for( i=591436; i; i-- );
} 

void isr_timer0( void )
{
    flagTimer0 = TRUE;
    I_ISPC = BIT_TIMER0;
}

void isr_keypad( void )
{
    flagKeypad = TRUE;
    I_ISPC = BIT_KEYPAD;
}

void isr_pbs( void )
{
    switch( pb_scan() )
    {
        case PB_FAILURE:
            EXTINTPND = BIT_LEFTPB | BIT_RIGHTPB;   
            break;
        case PB_LEFT:
            EXTINTPND = BIT_LEFTPB;   
            break;
        case PB_RIGHT:
            EXTINTPND = BIT_RIGHTPB;   
            break;
    }
    flagPbs = TRUE;
    I_ISPC = BIT_PB;
}
