/*-------------------------------------------------------------------
**
**  Fichero:
**    leds.h  5/4/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para el acceso a los leds de la placa de prototipado S3CEV40
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __LEDS_H__
#define __LEDS_H__

#include <common_types.h>

/*
** Identificadores de los leds
*/
#define LEFT_LED  (1)
#define RIGHT_LED (2)

/*
** Inicializa los leds y los apaga
*/
void leds_init( void );

/*
** Enciende el led indicado
*/
void led_on( uint8 led );

/*
** Apaga el led indicado
*/
void led_off( uint8 led );

/*
** Conmuta el led indicado
*/
void led_toggle( uint8 led );

/*
** Devuelve el estado (ON/OFF) del led indicado
*/
uint8 led_status( uint8 led );

#endif
