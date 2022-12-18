/*-------------------------------------------------------------------
**
**  Fichero:
**    lab13.c  12/1/2021
**
**    (c) J.M. Mendias
**    Programaci�n de Sistemas y Dispositivos
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Ejemplo de una aplicaci�n bajo un kernel de planificaci�n
**    no expropiativa de tareas cooperativas multiestado
**
**  Notas de dise�o:
**
**-----------------------------------------------------------------*/

#include "kernelcoop.h"

#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <system.h>
#include <leds.h>
#include <segs.h>
#include <uart.h>
#include <pbs.h>
#include <keypad.h>
#include <timers.h>
#include <rtc.h>
#include <lcd.h>
#include <common_functions.h>

/* Declaraci�n de recursos */

uint8 scancode;
boolean flagTask5;
boolean flagTask6;
boolean flagTask8;

volatile boolean flagPb;

/* Declaraci�n de tareas */

void Task1( void );
void Task2( void );
void Task3( void );
void Task4( void );
void Task5( void );
void Task6( void );
void Task7( void );
void Task8( void );
void Task9( void );


/* Declaraci�n de RTI */

void isr_pb( void ) __attribute__ ((interrupt ("IRQ")));

/*******************************************************************/

void main( void )
{
    sys_init();      /* Inicializa el sistema */
    timers_init();
    uart0_init();
    leds_init();
    segs_init();
    rtc_init();
    pbs_init();
    keypad_init();
    lcd_init();
    lcd_clear();
    lcd_on();

    uart0_puts( "\n\n Ejecutando kernel de planificaci�n no expropiativa\n" );
    uart0_puts( " --------------------------------------------------\n\n" ) ;

    flagTask5  = FALSE;               /* Inicializa flags */
    flagTask6  = FALSE;
    flagTask8  = FALSE;
    flagPb     = FALSE;

    scheduler_init();                 /* Inicializa el kernel */

    create_task( Task2, 5 );          /* Crea las tareas de la aplicaci�n... */
    create_task( Task7, 5 );          /* ... el kernel asigna la prioridad seg�n orden de creaci�n: Task2 > Task5 > Task6 > ... */
    create_task( Task5, 10 );         /* ... las tareas m�s frecuentes tienen mayor prioridad (criterio Rate-Monotonic-Scheduling) */
    create_task( Task6, 10 );
    create_task( Task8, 10 );
    create_task( Task1, 50 );
    create_task( Task3, 100 );
    create_task( Task9, 100 );
    create_task( Task4, 1000 );

    timer0_open_tick( scheduler, TICKS_PER_SEC );  /* Instala scheduler como RTI del timer0  */
    pbs_open( isr_pb );                            /* Instala isr_pb como RTI por presi�n de pulsadores  */

    while( 1 )
    {
        sleep();                /* Entra en estado IDLE, sale por interrupci�n */
        dispacher();            /* Las tareas preparadas se ejecutan en esta hebra (background) en orden de prioridad */
    }

}

/*******************************************************************/

void Task1( void )  /* Cada 0,5 segundos (50 ticks) alterna el led que se enciende */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 1: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
        led_on( LEFT_LED );
        led_off( RIGHT_LED );
    }
    else
    {
        led_toggle( LEFT_LED );
        led_toggle( RIGHT_LED );
    }
}

void Task2( void )  /* Cada 50 ms (5 ticks) muestrea el keypad y env�a el scancode a otras tareas */
{
    static boolean init = TRUE;
    static enum { wait_keydown, scan, wait_keyup } state;

    if( init )
    {
        init  = FALSE;
        uart0_puts( " Task 2: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
        state = wait_keydown;
    }
    else switch( state )
    {
        case wait_keydown:
            if( keypad_pressed() )
                state = scan;
            break;
        case scan:
            scancode = keypad_scan();
            if( scancode != KEYPAD_FAILURE )
            {
                flagTask5 = TRUE;
                flagTask6 = TRUE;
                flagTask8 = TRUE;
            }
            state = wait_keyup;
            break;
        case wait_keyup:
            if( !keypad_pressed() )
                state = wait_keydown;
            break;
    }
}

void Task3( void  )  /* Cada segundo (100 ticks) muestra por la UART0 la hora del RTC */
{
    static boolean init = TRUE;
    rtc_time_t rtc_time;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 3: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else
    {
        rtc_gettime( &rtc_time );
        uart0_puts( "  (Task 3) Hora: " );
        uart0_putint( rtc_time.hour );
        uart0_putchar( ':' );
        uart0_putint( rtc_time.min );
        uart0_putchar( ':' );
        uart0_putint( rtc_time.sec );
        uart0_puts( "\n" );
    }
}

void Task4( void )  /* Cada 10 segundos (1000 ticks) muestra por la UART0 los ticks transcurridos */
{
    static boolean init = TRUE;
    static uint32 ticks;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 4: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
        ticks = 0;
    }
    else
    {
        ticks += TICKS_PER_SEC * 10;
        uart0_puts( "  (Task 4) Ticks: " );
        uart0_putint( ticks );
        uart0_puts( "\n" );
    }
}

void Task5( void )  /* Cada vez que reciba un scancode lo muestra por la UART0 */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 5: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else if( flagTask5 )
    {
        flagTask5 = FALSE;
        uart0_puts( "  (Task 5) Tecla pulsada: " );
        uart0_puthex( scancode );
        uart0_puts( "\n" );
    }
}

void Task6( void )  /* Cada vez que reciba un scancode lo muestra por los 7 segmentos */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 6: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else if( flagTask6 )
    {
        flagTask6 = FALSE;
        segs_putchar( scancode );
    }
}

void Task7( void )  /* Cada vez que se presione un pulsador lo avisa por la UART0 */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 7: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else if( flagPb )
    {   
        flagPb = FALSE;
        uart0_puts( "  (Task 7) Se ha pulsado alg�n pushbutton...\n" );
    }
}

void Task8( void ) /* Muestra en el LCD cada una de las teclas pulsadas*/
{
    static boolean init = TRUE;
    static char* str = "Tecla pulsada:  ";

    if (init)
    {
        init = FALSE;
        uart0_puts( " Task 8: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else if (flagTask8)
    {
        flagTask8 = FALSE;
        str[15] = hexToString(scancode)[0];
        lcd_puts(LCD_WIDTH/2 - 64, LCD_HEIGHT/2, BLACK, str);
    }
}

void Task9(void) /* Muestra cada segundo en el LCD los segundos transcurridos */
{
    static boolean init = TRUE;
    static char* str = "Segundos: ";
    static uint32 secs;

    if (init)
    {
        init = FALSE;
        uart0_puts( " Task 9: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
        secs = 0;
    }
    else
    {
        secs++;
        lcd_puts(8, 8, BLACK, str);
        lcd_putint(88, 8, BLACK, secs);
    }
}

/*******************************************************************/


void isr_pb( void )
{
    flagPb = TRUE;
    EXTINTPND = BIT_RIGHTPB | BIT_LEFTPB;
    I_ISPC = BIT_PB;
}

/*******************************************************************/
