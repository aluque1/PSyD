/*-------------------------------------------------------------------
**
**  Fichero:
**    timers.h  26/2/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de las funciones
**    para la generación de retardos y la medida de tiempos
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <common_types.h>

/*
** Modo de funcionamiento del temporizador
*/
#define TIMER_ONE_SHOT (0)
#define TIMER_INTERVAL (1)

/*
** Pone a 0 los registros de configuración
** Pone a 0 todos los búfferes y registros de cuenta y comparación
** Para todos los temporizadores
** Inicializa las variables para retardos software
*/
void timers_init( void );

/*
** Realiza una espera de n milisegundos usando el timer3
*/
void timer3_delay_ms( uint16 n );

/*
** Realiza una espera de n milisegundos sin usar temporizadores
*/
void sw_delay_ms( uint16 n );

/*
** Realiza una espera de n segundos usando el timer3
*/
void timer3_delay_s( uint16 n );

/*
** Realiza una espera de n segundos sin usar temporizadores
*/
void sw_delay_s( uint16 n );

/*
** Arranca el timer3 a una frecuencia de 0,01 MHz 
** Permitirá medir tiempos con una resolución de 0,1 ms (100 us) hasta un máximo de 6.55s
*/
void timer3_start( void );

/*
** Detiene el timer3, devolviendo el número de décimas de milisegundo transcurridas desde que arrancó hasta un máximo de 6.55s
*/
uint16 timer3_stop( void );

/*
** Arranca el timer3 a una frecuencia de 0,01 MHz
** Permitirá contar n décimas de milisegundo (0,1 ms = 100 us) hasta un máximo de 6.55s
*/
void timer3_start_timeout( uint16 n );

/*
** Indica si el timer3 ha finalizado su cuenta
*/
uint16 timer3_timeout( void );

/*
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones del timer0
** Borra interrupciones pendientes del timer0
** Desenmascara globalmente las interrupciones y específicamente las interrupciones del timer0
** Configura el timer0 para que genere tps interrupciones por segundo
*/
void timer0_open_tick( void (*isr)(void), uint16 tps );

/*
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones del timer0
** Borra interrupciones pendientes del timer0
** Desenmascara globalmente las interrupciones y específicamente las interrupciones del timer0
** Configura el timer0 para que genere interrupciones en el modo y con la periodicidad indicadas
** Configura el timer0 para que genere interrupciones en el modo y con la periodicidad indicadas
*/
void timer0_open_ms( void (*isr)(void), uint16 ms, uint8 mode );

/*
** Para y pone a 0 todos sus bufferes y registros del timer0
** Deshabilita las interrupciones del timer0
** Desinstala la RTI del timer0
*/
void timer0_close( void );

#endif 
