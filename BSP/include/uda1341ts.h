/*-------------------------------------------------------------------
**
**  Fichero:
**    uda1341ts.h  24/5/2013
**
**    (c) J.M. Mendias
**    Programaci�n de Sistemas y Dispositivos
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Contiene las definiciones de los prototipos de funciones
**    para la reproduccion/grabacion de sonido usando el audio 
**    codec UDA1341TS
**
**  Notas de dise�o:
**
**-----------------------------------------------------------------*/

#ifndef __UDA1341TS_H__
#define __UDA1341TS_H__

#include <common_types.h>

#define UDA_DAC (1)
#define UDA_ADC (2)

#define VOL_MAX (0x3F)
#define VOL_MED (0x20)
#define VOL_MIN (0x0)

#define MUTE_ON  (1)
#define MUTE_OFF (0)

/*
** Inicializa el interfaz L3  
** Resetea el audio codec 
** Configura el audio codec seg�n los siguientes par�metros:
**   CODECLK = 256fs
**   Protocolo de trasmisi�n de audio: iis
**   Volumen de reproducci�n m�ximo
**   Selecciona el canal 1 como entrada
**   Habilita el ADC y DAC con 6 dB de ganancia de entrada
**   Fija el volumen m�ximo
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
** Fija el volumen de reproducci�n
*/
void uda1341ts_setvol( uint8 vol );

/*
** Devuelve el volumen de reproducci�n
*/
uint8 uda1341ts_getvol( void );

#endif
