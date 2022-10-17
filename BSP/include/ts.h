/*-------------------------------------------------------------------
**
**  Fichero:
**    ts.h  29/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para el acceso a la touchscreen de la placa de prototipado 
**    S3CEV40  
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __TS_H__
#define __TS_H__

#include <common_types.h>

/* 
** Errorcodes de la touchscreen
*/
#define TS_OK (1)          /* Sin error */
#define TS_FAILURE (0xff)  /* Fallo durante el proceso de scan */
#define TS_TIMEOUT (0xfe)  /* Tiempo de espera sobrepasado */

#define TS_OFF     (1)
#define TS_ON      (0)

/* 
** Inicializa y calibra la touchscreen
*/
void ts_init( void );

/* 
** Enciende el ADC que usa la touchscreen
*/
void ts_on( void );

/* 
** Apaga el ADC que usa la touchscreen
*/
void ts_off( void );

/*
** Devuelve el estado del ADC que usa la touchscreen
*/
uint8 ts_status( void );

/*
** Si la touchscreen está presionada devuelve TRUE, en otro caso devuelve FALSE
*/
uint8 ts_pressed( void );

/*
** Espera la pulsación de la touchscreen y devuelve la posicion
*/
void ts_getpos( uint16 *x, uint16 *y );

/*
** Espera la pulsación del touchscreen y devuelve la posicion y el intervalo en ms que ha estado pulsada (max. 65535ms)
*/
void ts_getpostime( uint16 *x, uint16 *y, uint16 *ms );

/*
** Espera un máximo de ms (no mayor de 6553ms) la pulsación y despulsación de la touchscreen y devuelve la posicion, en caso contrario devuelve TS_TIMEOUT
*/

uint8 ts_timeout_getpos( uint16 *x, uint16 *y, uint16 ms );

/* 
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones por pulsación de la touchscreen
** Borra interrupciones pendientes por pulsación de la touchscreen
** Desenmascara globalmente las interrupciones y específicamente las interrupciones por pulsación de la touchscreen
*/
void ts_open( void (*isr)(void) );

/* 
** Enmascara las interrupciones por pulsación de la touchscreen
** Desinstala la RTI por pulsación de la touchscreen
*/
void ts_close( void );

#endif
