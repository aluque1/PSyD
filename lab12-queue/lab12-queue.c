/*-------------------------------------------------------------------
**
**  Fichero:
**    lab12-queue.c  14/1/2021
**
**    (c) J.M. Mendias
**    Programaci�n de Sistemas y Dispositivos
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Ejemplo de una aplicaci�n guiada por tiempo con arquitectura
**    cola de funciones tipo FIFO
**
**  Notas de dise�o:
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
#include <lcd.h>
#include <common_functions.h>

#define TICKS_PER_SEC   (100)

/* Declaraci�n de fifo de punteros a funciones */

#define BUFFER_LEN   (512)

typedef void (*pf_t)(void);

typedef struct fifo {
    uint16 head;
    uint16 tail;
    uint16 size;
    pf_t buffer[BUFFER_LEN];
} fifo_t;

void fifo_init( void );
void fifo_enqueue( pf_t pf );
pf_t fifo_dequeue( void );
boolean fifo_is_empty( void );
boolean fifo_is_full( void );

/* Declaraci�n de recursos */

volatile fifo_t fifo;

uint8 scancode;

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
void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));

/*******************************************************************/

void main( void )
{
    pf_t pf;
    
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
      
    uart0_puts( "\n\n Ejecutando una aplicaci�n como cola de funciones\n" );
    uart0_puts( " ------------------------------------------------\n\n" ) ;

    fifo_init();             /* Inicializa cola de funciones */

    Task1();    /* Ejecuta por primera vez a las funciones para inicializarlas */
    Task2();
    Task3();
    Task4();
    Task5();
    Task6();
    Task7();
    Task8();
    Task9();

    pbs_open( isr_pb );                           /* Instala isr_pb como RTI por presi�n de pulsadores  */
    timer0_open_tick( isr_tick, TICKS_PER_SEC );  /* Instala isr_tick como RTI del timer0  */
        
    while( 1 )
    {
        sleep();                        /* Entra en estado IDLE, sale por interrupci�n */
        while( !fifo_is_empty() )
        {
            pf = fifo_dequeue();
            (*pf)();                    /* Las tareas encoladas se ejecutan en esta hebra (background) en orden de encolado */
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
                fifo_enqueue( Task5 );
                fifo_enqueue( Task6 );
                fifo_enqueue( Task8 );
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
        uart0_puts( " Task 6: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
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
        uart0_puts( " Task 7: iniciada.\n" );  /* Muestra un mensaje inicial en la UART0 (no es necesario sem�foro) */
    }
    else
    {   
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
    else
    {
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
    fifo_enqueue( Task7 );
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
        fifo_enqueue( Task2 );        
    }
    if( !(--cont50ticks) )
    {
        cont50ticks = 50;
        fifo_enqueue( Task1 );
    }        
    if( !(--cont100ticks) )
    {
        cont100ticks = 100;
        fifo_enqueue( Task3 );
        fifo_enqueue( Task9 );       
    }    
    if( !(--cont1000ticks) )
    {
        cont1000ticks = 1000;
        fifo_enqueue( Task4 );        
    }   
    
    I_ISPC = BIT_TIMER0;
};

/*******************************************************************/

void fifo_init( void )
{
    fifo.head = 0;
    fifo.tail = 0;
    fifo.size = 0;
}

void fifo_enqueue( pf_t pf )
{
    fifo.buffer[fifo.tail++] = pf;
    if( fifo.tail == BUFFER_LEN )
        fifo.tail = 0;
    INT_DISABLE;
    fifo.size++;
    INT_ENABLE;
}

pf_t fifo_dequeue( void )
{
    pf_t pf;
    
    pf = fifo.buffer[fifo.head++];
    if( fifo.head == BUFFER_LEN )
        fifo.head = 0;
    INT_DISABLE;
    fifo.size--;
    INT_ENABLE;
    return pf;
}

boolean fifo_is_empty( void )
{
    return (fifo.size == 0);
}

boolean fifo_is_full( void )
{
    return (fifo.size == BUFFER_LEN-1);
}

/*******************************************************************/
