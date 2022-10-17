/*-------------------------------------------------------------------
**
**  Fichero:
**    keypad.h  29/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para el acceso al keypad de la placa de prototipado S3CEV40  
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <common_types.h>

#define KEYPAD_IO_METHOD POOLING

/* 
** Scancodes del keypad
*/
#define KEYPAD_KEY0 (0x0) /*  Distribución espacial:  */
#define KEYPAD_KEY1 (0x1)  
#define KEYPAD_KEY2 (0x2) /*  KEY0  KEY1  KEY2  KEY3  */
#define KEYPAD_KEY3 (0x3) /*  KEY4  KEY5  KEY6  KEY7  */
#define KEYPAD_KEY4 (0x4) /*  KEY8  KEY9  KEYA  KEYB  */
#define KEYPAD_KEY5 (0x5) /*  KEYC  KEYD  KEYE  KEYF  */
#define KEYPAD_KEY6 (0x6)
#define KEYPAD_KEY7 (0x7)
#define KEYPAD_KEY8 (0x8)
#define KEYPAD_KEY9 (0x9)
#define KEYPAD_KEYA (0xa)
#define KEYPAD_KEYB (0xb)
#define KEYPAD_KEYC (0xc)
#define KEYPAD_KEYD (0xd)
#define KEYPAD_KEYE (0xe)
#define KEYPAD_KEYF (0xf)

/* 
** Errorcodes del keypad
*/
#define KEYPAD_FAILURE (0xff)  /* Fallo durante el proceso de scan */
#define KEYPAD_TIMEOUT (0xfe)  /* Tiempo de espera sobrepasado */

/*
** Inicializa el keypad
** Inicializa timers
*/
void keypad_init( void );

/* 
** Si hay una tecla presionada devuelve su scancode, en otro caso devuelve KEYPAD_FAILURE
*/
uint8 keypad_scan( void );

/*
** Si hay una tecla presionada devuelve TRUE, en otro caso devuelve FALSE
*/
uint8 keypad_pressed( void );

/* 
** Espera la presión y depresión de una tecla del keypad y devuelve su scancode
*/ 
uint8 keypad_getchar( void );

/* 
** Espera la presión y depresión de una tecla del keypad y devuelve su scancode y el intervalo en ms que ha estado pulsada (max. 6553ms)
*/ 
uint8 keypad_getchartime( uint16 *ms );

/*
** Espera un máximo de ms (no mayor de 6553ms) la presión y depresión de una tecla del keypad y devuelve su scancode, en caso contrario devuelve KEYPAD_TIMEOUT
*/
uint8 keypad_timeout_getchar( uint16 ms );

/* 
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones por presión del keypad
** Borra interrupciones pendientes por presión del keypad
** Desenmascara globalmente las interrupciones y específicamente las interrupciones por presión del keypad
*/
void keypad_open( void (*isr)(void) );

/* 
** Enmascara las interrupciones por presión del keypad
** Desinstala la RTI por presión del keypad
*/
void keypad_close( void );

#endif
