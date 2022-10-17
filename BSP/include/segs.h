/*-------------------------------------------------------------------
**
**  Fichero:
**    segs.h  5/4/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para el acceso al display 7-segmentos de la placa de
**    prototipado S3CEV40
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __SEGS_H__
#define __SEGS_H__

#include <common_types.h>

#define SEGS_OFF (0xff)

/*
** Inicializa el display 7-segmentos y lo apaga
*/
void segs_init( void );

/*
** Apaga el display 7-segmentos
*/
void segs_off( void );

/*
** Visualiza el número indicado en el display 7-segmentos
*/
void segs_putchar( uint8 n ); 

/*
** Devuelve el número que se está visualizando en el display 
** 7-segmentos o SEGS_OFF si está apagado
*/
uint8 segs_status( void );

#endif
