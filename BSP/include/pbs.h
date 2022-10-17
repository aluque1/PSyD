/*-------------------------------------------------------------------
**
**  Fichero:
**    pbs.h  29/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para el acceso a los push-buttons de la placa de prototipado 
**    S3CEV40  
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __PBS_H__
#define __PBS_H__

#include <common_types.h>

/* 
** Scancodes de los pulsadores
*/
#define PB_RIGHT   (1<<7)
#define PB_LEFT	   (1<<6)

/* 
** Errorcodes de los pulsadores
*/
#define PB_FAILURE (0xff)  /* Fallo durante el proceso de scan */
#define PB_TIMEOUT (0xfe)  /* Tiempo de espera sobrepasado */

/* 
** Status de un pulsador
*/
#define PB_DOWN   (1)
#define PB_UP     (0)

/*
** Inicializa los pulsadores
** Inicializa timers
*/
void pbs_init( void );

/* 
** Si hay un pulsador presionado devuelve su scancode, en otro caso devuelve PB_FAILURE
*/
uint8 pb_scan( void );

/*
** Si hay algún pulsador presionado devuelve TRUE, en otro caso devuelve FALSE
*/
uint8 pb_pressed( void );

/*
** Espera la presión y depresión de un pulsador y devuelve su scancode
*/
uint8 pb_getchar( void );

/*
** Espera la presión y depresión de un pulsador y devuelve su scancode y el intervalo en ms que ha estado pulsado (max. 6553ms)
*/
uint8 pb_getchartime( uint16 *ms );

/*
** Espera un máximo de ms (no mayor de 6553ms) la presión y depresión de un pulsador y devuelve su scancode, en caso contrario devuelve PB_TIMEOUT
*/
uint8 pb_timeout_getchar( uint16 ms );

/* 
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones por presión de un pulsador
** Borra interrupciones pendientes por presión de un pulsador
** Desenmascara globalmente las interrupciones y específicamente las interrupciones por presión de un pulsador
*/
void pbs_open( void (*isr)(void) );

/* 
** Enmascara las interrupciones por presión de un pulsador
** Desinstala la RTI por presión de un pulsador
*/
void pbs_close( void );

#endif
