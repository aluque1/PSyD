/*-------------------------------------------------------------------
**
**  Fichero:
**    uda1341ts.h  24/5/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de funciones
**    para la reproduccion/grabacion de sonido usando el audio 
**    codec UDA1341TS
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#ifndef __UDA1341TS_H__
#define __UDA1341TS_H__

#include <common_types.h>

#define UDA_DAC (1)
#define UDA_ADC (2)

#define VOL_MAX (0x3F)
#define VOL_MED    (0x20)
#define VOL_MIN (0x0)

#define MUTE_ON  (1)
#define MUTE_OFF (0)

/*
** Inicializa el interfaz L3  
** Resetea el audio codec 
** Configura el audio codec según los siguientes parámetros:
**   CODECLK = 256fs
**   Protocolo de trasmisión de audio: iis
**   Volumen de reproducción máximo
**   Selecciona el canal 1 como entrada
**   Habilita el ADC y DAC con 6 dB de ganancia de entrada
**   Fija el volumen máximo
*/
void uda1341ts_init( void );

/*
** Habilita/desabilita el silenciado del audio codec
*/
void uda1341ts_mute( uint8 on );

/*
** Enciende el conversor indicado
*/
void uda1341ts_on( uint8 converter );

/*
** Apaga el conversor indicado
*/
void uda1341ts_off( uint8 converter );

/*
** Devuelve el estado del conversor indicado
*/
uint8 uda1341ts_status( uint8 converter );

/*
** Fija el volumen de reproducción
*/
void uda1341ts_setvol( uint8 vol );

/*
** Devuelve el volumen de reproducción
*/
uint8 uda1341ts_getvol( void );

#endif
