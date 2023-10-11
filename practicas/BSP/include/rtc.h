/*-------------------------------------------------------------------
**
**  Fichero:
**    rtc.h  29/4/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de las funciones
**    para la gestión básica del Real Time Clock del chip S3C44BOX
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __RTC_H__
#define __RTC_H__

#include <common_types.h>

typedef struct 
{
    uint8 sec;   /* segundo (0-59)*/
    uint8 min;   /* minuto (0-59) */
    uint8 hour;  /* hora (0-23) */
    uint8 mday;  /* día del mes (1-31) */
    uint8 wday;  /* día de la semana (1-7) comenzado por el domingo */
    uint8 mon;   /* mes (1-12) */
    uint8 year;  /* año (0-99)*/
} rtc_time_t;

/*
** Configura el RTC desactivando alarma, round reset y generación de ticks
** Inicializa el RTC a las 00:00:00 horas del martes 1 de enero de 2013
** Inicializa a 0 los registros de alarma 
*/
void rtc_init( void );

/*
** Actualiza la fecha y hora mantenida por el RTC 
*/
void rtc_puttime( rtc_time_t *rtc_time );

/* 
** Recupera en la fecha y hora mantenida por el RTC
*/
void rtc_gettime( rtc_time_t *rtc_time );

/* 
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones por ticks del RTC
** Borra interrupciones pendientes por ticks del RTC
** Desenmascara globalmente las interrupciones y específicamente las interrupciones por ticks del RTC
** Habilita la generación de ticks y fija el valor inicial del contador que los genera: perido = (tick_count+1)/128 segundos
*/
void rtc_open( void (*isr)(void), uint8 tick_count );

/* 
** Deshabilita la generación de ticks
** Enmascara las interrupciones por ticks del RTC
** Desinstala la RTI por ticks del RTC 
*/
void rtc_close( void );

#endif //__RTC_H__
