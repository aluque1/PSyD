/*-------------------------------------------------------------------
**
**  Fichero:
**    iis.h  12/5/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la comunicación por el bus IIS del chip S3C44BOX
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __IIS_H__
#define __IIS_H__

#include <common_types.h>

#define IIS_DMA     (1)
#define IIS_POLLING (2)

/*
** Configura el controlador de IIS según los siguientes parámetros 
**   Master mode en reposo (no transfer y todo desabilitado)
**   fs: 16000 KHz
**   CODECLK: 256fs 
**   SDCLK: 32fs
**   Bits por canal: 16
**   Protocolo de trasmisión de audio: iis
**   Señalizacion de canal izquierdo: a baja 
** Si mode = IIS_DMA
**   No transfer mode
**   Deshabilita Tx/Rx FIFOs
**   Deshabilita prescaler e IIS
**   Inicializa el BDMA0
**   Abre las interrupciones por BDMA0
** Si mode = IIS_POLLING
**   Transmit and receibe mode
**   Tx/Rx por pooling
**   Habilita Tx/Rx FIFOs
**   Habilita prescaler e IIS
*/
void iis_init( uint8 mode );

/*
** Envía por el bus IIS una muestra por pooling
*/
inline void iis_putSample( int16 ch0, int16 ch1 );

/*
** Almacena por pooling una muestra recibida por el bus IIS
*/
inline void iis_getSample( int16 *ch0, int16 *ch1 );

/*
** Envía por el bus IIS un flujo de length/2 muestras almacenado en el buffer indicado
** Si mode = IIS_POOLING
**   Transmite las muestras de 1 en 1 leyéndolas del buffer
** Si mode = IIS_DMA
**   Programa una transferencia por BDMA0 de length bits de buffer a IISFIF
**   Transmit mode
**   Habilita Tx FIFO
**   Tx por DMA
**   Habilita Tx DMA request, prescaler e IIS
** El parametro loop (TRUE/FALSE) permite indicar reproducción continua en caso de DMA. No aplica a pooling.
*/
void iis_play( int16 *buffer, uint32 length, uint8 loop );

/*
** Almacena en el buffer indicado por DMA un flujo de length/2 recibidas por el bus IIS
** utilizando el método de transferencia indicado al inicializar el dispositivo.
** Si mode = IIS_POOLING
**   Recibe las muestras de 1 en 1 almacenándolas en el buffer
** Si mode = IIS_DMA
**   Programa una transferencia por BDMA0 de length bits de buffer a IISFIF
**   Reciebe mode
**   Habilita Rx FIFO
**   Rx por DMA
**   Habilita Rx DMA request, prescaler e IIS
*/
void iis_rec( int16 *buffer, uint32 length );

/*
** Pausa la recepcion/transmision de muestras por bus IIS
** Aplica solo al caso de transferencias por DMA
*/
void iis_pause( void );

/*
** Continua la recepcion/transmision de muestras por bus IIS
** Aplica solo al caso de transferencias por DMA
*/
void iis_continue( void );

/*
** Devuelve (ON/OFF) para indicar si se está reproduciendo o no sonido
** Aplica solo al caso de transferencias por DMA
*/
uint8 iis_status( void );

/*
** Reproduce un fichero en formato WAV cargado en memoria a partir de la dirección indicada
** El parametro loop (TRUE/FALSE) permite indicar reproducción continua en caso de DMA. No aplica a pooling.
*/
void iis_playWawFile( int16 *wav, uint8 loop );


#endif
