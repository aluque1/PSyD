/*-------------------------------------------------------------------
**
**  Fichero:
**    L3.h  24/5/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la comunicación por el interfaz L3 con el chip UDA1341TS 
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __L3_H__
#define __L3_H__

#include <common_types.h>

#define L3_ADDR_MODE (0)
#define L3_DATA_MODE (1)

/*
** Inicializa a 1 las lineas L3CLOCK y L3MODE  
*/
void L3_init( void );

/*
** Envía un byte por el interfaz L3 en el modo (ADDR/DATA) indicado 
*/
void L3_putByte( uint8 byte, uint8 mode );

#endif
