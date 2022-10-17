/*-------------------------------------------------------------------
**
**  Fichero:
**    lab12-foreback.c  14/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Ejemplo de una aplicación guiada por tiempo con arquitectura
**    foreground/background
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

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

#define TICKS_PER_SEC   (100)

/* Declaración de recursos */

uint8 scancode;
boolean flagTask5;
boolean flagTask6;

volatile boolean flagPb;
volatile boolean flagToggleLeds;
volatile boolean flagReadKeypad;
volatile boolean flagWriteRTC;
volatile boolean flagWriteTicks;

/* Declaración de tareas */

void Task1( void );
void Task2( void );
void Task3( void );
void Task4( void );
void Task5( void );
void Task6( void );
void Task7( void );

/* Declaración de RTI */

void isr_pb( void ) __attribute__ ((interrupt ("IRQ")));
void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));

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

    uart0_puts( "\n\n Ejecutando una aplicación foreground/background\n" );
    uart0_puts( " -----------------------------------------------\n\n" ) ;
    
    flagTask5      = FALSE;    /* Inicializa flags */
    flagTask6      = FALSE;
    flagPb         = FALSE;
    flagToggleLeds = FALSE;
    flagReadKeypad = FALSE;
    flagWriteRTC   = FALSE;
    flagWriteTicks = FALSE;

    Task1();    /* Ejecuta por primera vez a las funciones para inicializarlas */
    Task2();
    Task3();
    Task4();
    Task5();
    Task6();
    Task7();
   
    pbs_open( isr_pb );                           /* Instala isr_pbs como RTI por presión de pulsadores  */
    timer0_open_tick( isr_tick, TICKS_PER_SEC );  /* Instala isr_tick como RTI del timer0  */
        
    while( 1 )
    {
        sleep();                        /* Entra en estado IDLE, sale por interrupción */
        if( flagToggleLeds )            /* Las tareas se ejecutan en esta hebra (background) en orden de aparición en el código */
        {
            flagToggleLeds = FALSE;
            Task1();
        }
        if( flagReadKeypad )        
        {
            flagReadKeypad = FALSE;
            Task2();
        }
        if( flagWriteRTC )
        {
            flagWriteRTC = FALSE;
            Task3();
        }
        if( flagWriteTicks )
        {
            flagWriteTicks = FALSE;
            Task4();
        }
        if( flagTask5 )
        {
            flagTask5 = FALSE;
            Task5();
        }
        if( flagTask6 )
        {
            flagTask6 = FALSE;
            Task6();
        }
        if( flagPb )
        {
            flagPb = FALSE;
            Task7();
        }
    }
}

/*******************************************************************/

void Task1( void )  /* Cada 0,5 segundos (50 ticks) alterna el led que se enciende */
{
    static boolean init = TRUE;
  
    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 1: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
        led_on( LEFT_LED );
        led_off( RIGHT_LED );
    }
    else
    {
        led_toggle( LEFT_LED );
        led_toggle( RIGHT_LED );
    }
}

void Task2( void )  /* Cada 50 ms (5 ticks) muestrea el keypad y envía el scancode a otras tareas */
{
    static boolean init = TRUE;
    static enum { wait_keydown, scan, wait_keyup } state;

    if( init )
    {
        init  = FALSE;
        uart0_puts( " Task 2: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
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
        uart0_puts( " Task 3: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
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
        uart0_puts( " Task 4: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
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
        uart0_puts( " Task 5: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else
    {
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
        uart0_puts( " Task 6: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else
    {
        segs_putchar( scancode );
    }
}

void Task7( void )  /* Cada vez que se presione un pulsador lo avisa por la UART0 */
{
    static boolean init = TRUE;

    if( init )
    {
        init = FALSE;
        uart0_puts( " Task 7: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario semáforo) */
    }
    else
    {   
        uart0_puts( "  (Task 7) Se ha pulsado algún pushbutton...\n" );
    }
}

/*******************************************************************/

void isr_pb( void )
{
    flagPb = TRUE;
    EXTINTPND = BIT_RIGHTPB | BIT_LEFTPB;
    I_ISPC = BIT_PB;
}

void isr_tick( void )
{  
    static uint16 cont5ticks    = 5;
    static uint16 cont50ticks   = 50;
    static uint16 cont100ticks  = 100;
    static uint16 cont1000ticks = 1000;
    
    if( !(--cont5ticks) )
    {
        cont5ticks = 5;
        flagReadKeypad = TRUE;  
    }
    if( !(--cont50ticks) )
    {
        cont50ticks = 50;
        flagToggleLeds = TRUE;
    }        
    if( !(--cont100ticks) )
    {
        cont100ticks = 100;
        flagWriteRTC  = TRUE;
    }    
    if( !(--cont1000ticks) )
    {
        cont1000ticks = 1000;
        flagWriteTicks = TRUE;
    }   
    
    I_ISPC = BIT_TIMER0;
};

/*******************************************************************/
