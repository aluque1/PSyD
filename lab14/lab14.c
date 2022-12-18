/*-------------------------------------------------------------------
**
**  Fichero:
**    lab12.c  12/1/2021
**
**    (c) J.M. Mendias
**    Programaci�n de Sistemas y Dispositivos
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Ejemplo de una aplicaci�n bajo uC/OS-II
**
**  Notas de dise�o:
**
**-----------------------------------------------------------------*/

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#include <s3c44b0x.h>
#include <s3cev40.h>
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

/* Declaraci�n de pilas */

#define    TASK_STK_SIZE    10*1024

OS_STK Task1Stk[TASK_STK_SIZE];
OS_STK Task2Stk[TASK_STK_SIZE];
OS_STK Task3Stk[TASK_STK_SIZE];
OS_STK Task4Stk[TASK_STK_SIZE];
OS_STK Task5Stk[TASK_STK_SIZE];
OS_STK Task6Stk[TASK_STK_SIZE];
OS_STK Task7Stk[TASK_STK_SIZE];
OS_STK Task8Stk[TASK_STK_SIZE];
OS_STK Task9Stk[TASK_STK_SIZE];
OS_STK TaskStartStk[TASK_STK_SIZE];

/* Declaraci�n de recursos */

OS_EVENT *uart0Sem;                /* Sem�foro para el acceso mutex a la UART0 */
OS_EVENT *lcdSem;                  /* Sem�foro para el acceso mutex al LCD */

OS_EVENT *keypadMbox;              /* Buz�n para el scancode de la tecla pulsada */
OS_EVENT *flagPb;                  /* Flag para se�alizar la presi�n de un pulsador */

/* Declaraci�n de tareas */

void Task1( void *id );
void Task2( void *id );
void Task3( void *id );
void Task4( void *id );
void Task5( void *id );
void Task6( void *id );
void Task7( void *id );
void Task8( void *id );
void Task9( void *id );
void TaskStart( void *pdata );

/* Declaraci�n de RTI */

extern void OSTickISR( void );       /* RTI del tick del sistema */

extern void OS_CPU_isr_pb( void );   /* RTI (wrapper) por pulsaci�n teclado */
void isr_pb( void );                 /* Funci�n invocada por el anterior wrapper que atiende al dispositivo, no debe tener el atributo interrupt */

/*******************************************************************/

void main( void )
{
    sys_init();                                                          /* Inicializa los dispositivos        */
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

    uart0_puts( "\n\n Ejecutando uCOS-II (version " );
    uart0_putint( OSVersion() );
    uart0_puts( ")\n" ) ;
    uart0_puts( "----------------------------------\n\n" ) ;

    OSInit();                                                              /* Inicializa el kernel              */
    uart0Sem   = OSSemCreate( 1 );                                         /* Crea recursos                     */
    lcdSem     = OSSemCreate( 1 );
    keypadMbox = OSMboxCreate( NULL );
    flagPb     = OSSemCreate( 0 ); 
    
    OSTaskCreate( TaskStart, NULL, &TaskStartStk[TASK_STK_SIZE - 1], 0 );  /* Crea la tarea inicial de arranque */
    OSStart();                                                             /* Inicia multitarea                 */
}                              

/*******************************************************************/

void TaskStart( void *pdata )
{
    const char id1 = '1'; /* Identificadores de tareas */
    const char id2 = '2';
    const char id3 = '3';
    const char id4 = '4';
    const char id5 = '5';
    const char id6 = '6';
    const char id7 = '7';
    const char id8 = '8';
    const char id9 = '9';
  
    OS_ENTER_CRITICAL();
    timer0_open_tick( OSTickISR, OS_TICKS_PER_SEC );  /* Instala OSTickISR como RTI del timer0                     */
    pbs_open( OS_CPU_isr_pb );                        /* Instala OS_CPU_isr_pb como RTI por presi�n de pulsadores  */
    OS_EXIT_CRITICAL();

    // OSStatInit();                     /* Opcionalmente, arranca la tarea del kernel de recopilaci�n de estad�sticas de uso de CPU  */

    OSTaskCreate( Task1, (void *)&id1, &Task1Stk[TASK_STK_SIZE - 1], 6 );      /* Crea las tareas de la aplicaci�n */
    OSTaskCreate( Task2, (void *)&id2, &Task2Stk[TASK_STK_SIZE - 1], 1 );      /* Las tareas m�s frecuentes tienen mayor prioridad (criterio Rate-Monotonic-Scheduling) */
    OSTaskCreate( Task3, (void *)&id3, &Task3Stk[TASK_STK_SIZE - 1], 7 );
    OSTaskCreate( Task4, (void *)&id4, &Task4Stk[TASK_STK_SIZE - 1], 9 );
    OSTaskCreate( Task5, (void *)&id5, &Task5Stk[TASK_STK_SIZE - 1], 3 );
    OSTaskCreate( Task6, (void *)&id6, &Task6Stk[TASK_STK_SIZE - 1], 4 );
    OSTaskCreate( Task7, (void *)&id7, &Task7Stk[TASK_STK_SIZE - 1], 2 );
    OSTaskCreate( Task8, (void *)&id8, &Task8Stk[TASK_STK_SIZE - 1], 5 );
    OSTaskCreate( Task9, (void *)&id9, &Task9Stk[TASK_STK_SIZE - 1], 8 );

    OSTaskDel(OS_PRIO_SELF);             /* La tarea inicial de arranque se auto-elimina */
}

void Task1( void *id )
{
    INT8U err;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );
    
    led_on( LEFT_LED );
    led_off( RIGHT_LED );

    while( 1 )                         /* Cada 0,5 segundos (50 ticks) alterna el led que se enciende */
    {
        OSTimeDly( 50 );
        led_toggle( LEFT_LED );
        led_toggle( RIGHT_LED );
    }
}

void Task2( void *id)
{
    INT8U err;
    uint8 scancode;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada 50 ms (5 ticks) muestrea el keypad y env�a el scancode a otras tareas */
    {      
        while( !keypad_pressed() )
            OSTimeDly( 5 );
        scancode = keypad_scan();
        if( scancode != KEYPAD_FAILURE )
            OSMboxPostOpt( keypadMbox, (void *) &scancode, OS_POST_OPT_BROADCAST );
        while( keypad_pressed() )
            OSTimeDly( 5 );
    }
}

void Task3( void *id )
{
    INT8U err;
    rtc_time_t rtc_time;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada segundo (100 ticks) muestra por la UART0 la hora del RTC */
    {
        OSTimeDly( 100 );
        rtc_gettime( &rtc_time );
        OSSemPend( uart0Sem, 0, &err );
            uart0_puts( "  (Task" );
            uart0_putchar( *(char *)id );
            uart0_puts( ") Hora: " );
            uart0_putint( rtc_time.hour );
            uart0_putchar( ':' );
            uart0_putint( rtc_time.min );
            uart0_putchar( ':' );
            uart0_putint( rtc_time.sec );
            uart0_puts( "\n" );
        OSSemPost( uart0Sem );
    }
}

void Task4( void *id )
{
    INT8U err;
    INT32U ticks;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada 10 segundos (1000 ticks) muestra por la UART0 los ticks transcurridos */
    {
        OSTimeDly( 1000 );
        ticks = OSTimeGet();
        OSSemPend( uart0Sem, 0, &err );
            uart0_puts( "  (Task" );
            uart0_putchar( *(char *)id );
            uart0_puts( ") Ticks: " );
            uart0_putint( ticks );
            uart0_puts( "\n" );
        OSSemPost( uart0Sem );
    }
}

void Task5( void *id )
{
    INT8U err;
    uint8 scancode;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada vez que reciba un scancode lo muestra por la UART0 */
    {
        scancode = *((uint8 *) OSMboxPend( keypadMbox, 0, &err ));
        OSSemPend( uart0Sem, 0, &err );
            uart0_puts( "  (Task" );
            uart0_putchar( *(char *)id );
            uart0_puts( ") Tecla pulsada: " );
            uart0_puthex( scancode );
            uart0_puts( "\n" );
        OSSemPost( uart0Sem );
    }
}

void Task6( void *id )
{
    INT8U err;
    uint8 scancode;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada vez que reciba un scancode lo muestra por los 7 segmentos */
    {
        scancode = *((uint8 *) OSMboxPend( keypadMbox, 0, &err ));
        segs_putchar( scancode );
    }
}

void Task7( void *id )
{
    INT8U err;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada vez que se presione un pulsador lo avisa por la UART0 */
    {
        OSSemPend( flagPb, 0, &err );
        OSSemPend( uart0Sem, 0, &err );
            uart0_puts( "  (Task" );
            uart0_putchar( *(char *)id );
            uart0_puts( ") Se ha pulsado alg�n pushbutton...\n" );
        OSSemPost( uart0Sem );
    }
}

void Task8( void *id ) /* Muestra en el LCD cada una de las teclas pulsadas*/
{
    INT8U err;
    static char* str = "Tecla pulsada:  ";
    uint8 scancode;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada vez que reciba un scancode lo muestra en el LCD */
    {
        scancode = *((uint8 *) OSMboxPend( keypadMbox, 0, &err ));
        OSSemPend( lcdSem, 0, &err );
            str[15] = hexToString(scancode)[0];
            lcd_puts( LCD_WIDTH/2 - 64, LCD_HEIGHT/2, BLACK, str );
        OSSemPost( lcdSem );
    
    }
}

void Task9( void *id ) /* Muestra cada segundo en el LCD los segundos transcurridos */
{
    INT8U err;
    static char* str = "Segundos: ";
    static uint32 secs;

    OSSemPend( uart0Sem, 0, &err );    /* Muestra un mensaje inicial en la UART0 (protegida por un sem�foro) */
        uart0_puts( "  Task" );
        uart0_putchar( *(char *)id );
        uart0_puts( " iniciada.\n" );
        secs = 0;
    OSSemPost( uart0Sem );

    while( 1 )                         /* Cada vez que reciba un scancode lo muestra en el LCD */
    {
        OSSemPend( lcdSem, 0, &err );
            lcd_puts( 8, 8, BLACK, str );
            lcd_putint( 88, 8, BLACK, secs++);
        OSSemPost( lcdSem );
        OSTimeDly( 100 );
    }
}

/*******************************************************************/

void isr_pb( void )
{
    OSSemPost( flagPb );
    EXTINTPND = BIT_RIGHTPB | BIT_LEFTPB;
    I_ISPC = BIT_PB;
}

/*******************************************************************/
