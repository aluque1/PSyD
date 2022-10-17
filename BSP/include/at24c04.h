/*-------------------------------------------------------------------
**
**  Fichero:
**    at24c04.h  17/5/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la lectura/escritura de la EEPROM serie AT24C04
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __AT24C04_H__
#define __AT24C04_H__

#include <common_types.h>

#define AT24C04_WIDTH (8)
#define AT24C04_DEPTH (512)

/*
** Borra al completo el contenido de la memoria
*/
void at24c04_clear( void );

/*
** Realiza una escritura aleatoria en la dirección indicada
*/
void at24c04_bytewrite( uint16 addr, uint8 data );

/*
** Realiza una lectura aleatoria de la dirección indicada
*/
void at24c04_byteread( uint16 addr, uint8 *data );

/*
** Realiza una lectura secuencial de la memoria completa
*/
void at24c04_load( uint8 *buffer );

/*
** Realiza una escritura paginada de la memoria completa
*/
void at24c04_store( uint8 *buffer );

#endif

