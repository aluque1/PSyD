/*-------------------------------------------------------------------
**
**  Fichero:
**    dma.h  11/5/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la inicialización del controlador BDMA0
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __DMA_H__
#define __DMA_H__

#include <common_types.h>

/* 
** Inicializa a 0 los registros de control del canal BDMA0
*/
void bdma0_init( void );

/* 
** Instala, en la tabla de vectores de interrupción, la función isr como RTI de interrupciones del canal BDMA0
** Borra interrupciones pendientes por BDMA0
** Desenmascara globalmente las interrupciones y específicamente las interrupciones del canal BDMA0
*/
void bdma0_open( void (*isr)(void) );

/* 
** Enmascara las interrupciones del canal BDMA0
** Desinstala la RTI del canal BDMA0
*/
void bdma0_close( void );

#endif
