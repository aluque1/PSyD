/*-------------------------------------------------------------------
**
**  Fichero:
**    iic.h  5/3/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la comunicación por el bus IIC del chip S3C44BOX 
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __IIC_H__
#define __IIC_H__

#include <common_types.h>

/*
** Modos de funcionamiento del controlador de IIC
*/
#define IIC_Rx      (2)
#define IIC_Tx      (3)

/*
** Macros para indicar si en recepción se envía ACK o no
*/
#define RxACK       (1)
#define NO_RxACK    (0)

/*
** Configura el controlador de IIC según los siguientes parámetros 
**   Interrupciones: habilitadas
**   Transmisión/recepción: activada
**   Frecuencia de comunicación: 250 KHz
**   Generación de ACK: automática
*/
void iic_init( void );

/*
** Inicia un flujo de transmición/recepción en la que el microcontrolador actúa como master:
**   Genenera la START condition
**   Envía la dirección del dispositivo esclavo
**   Espera la recepción de ACK 
*/
void iic_start( uint8 mode, uint8 byte );

/*
** Actuando el microcontrolador como master:
**   Envía el byte indicado 
**   Espera la recepción de ACK
*/
void iic_putByte( uint8 byte );

/*
** Actuando el microcontrolador como master:
**   Espera la recepción de un byte
**   Genera o no ACK 
**   Devuelve el byte recibido
*/
uint8 iic_getByte( uint8 ack );

/*
** Envía la stop condition y espera los ms indicados para que haga efecto
*/
void iic_stop( uint16 ms );

#endif
