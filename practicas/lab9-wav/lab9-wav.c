/*-------------------------------------------------------------------
**
**  Fichero:
**    lab9-wav.c  12/5/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Test del laboratorio 9 (2a. parte)
**
**  Notas de diseño:
**    - Requiere que los ficheros de sonido (stéreo, 16b, 16KHz)
**      estén cargados en memoria en las posiciones indicadas.
**    - Para ello ejecutar en el depurador el comando: 
**      "script load_wavs.script"  el cual presupone que los 
**      ficheros *.wav están en la carpeta "..\common\wav16b16" 
**
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <common_types.h>
#include <system.h>
#include <uart.h>
#include <uda1341ts.h>
#include <iis.h>
#include <keypad.h>
#include <segs.h>
#include <timers.h>

/* Direcciones en donde se encuentran cargados los sonidos tras la ejecucion del comando "script load_wavs.script" */

#define DTMF0      ((int16 *)0x0c400000)
#define DTMF1      ((int16 *)0x0c402000)
#define DTMF2      ((int16 *)0x0c404000)
#define DTMF3      ((int16 *)0x0c406000)
#define DTMF4      ((int16 *)0x0c408000)
#define DTMF5      ((int16 *)0x0c40a000)
#define DTMF6      ((int16 *)0x0c40c000)
#define DTMF7      ((int16 *)0x0c40e000)
#define DTMF8      ((int16 *)0x0c410000)
#define DTMF9      ((int16 *)0x0c412000)
#define DTMFSTAR   ((int16 *)0x0c414000)
#define DTMFSHARP  ((int16 *)0x0c418000)
#define NOKIATUNE  ((int16 *)0x0c41a000)
#define INVITACION ((int16 *)0x0c47a000)
#define LLAMADA    ((int16 *)0x0c4fa000)
#define CONGESTION ((int16 *)0x0c54a000)
#define OCUPADO    ((int16 *)0x0c56a000)

/* Tamaños en bytes de los sonidos cargados */

#define DTMF_SIZE       (6400)    /* (0,1 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 6400 B   */
#define NOKIATUNE_SIZE  (352000)  /* (5,5 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 352000 B */
#define INVITACION_SIZE (512000)  /* (8,0 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 512000 B */
#define LLAMADA_SIZE    (288000)  /* (4,5 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 288000 B */
#define CONGESTION_SIZE (102400)  /* (1,6 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 102400 B */
#define OCUPADO_SIZE    (25600)   /* (0,4 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 25600 B  */

/* Direccion y tamaños del buffer de grabación */

#define REC_BUFFER      ((int16 *)0x0c700000)
#define REC_SIZE        (320000)  /* (5,0 s) * (2 canales) * (2 B/canal) * (16000 muestras/s) = 320000 B  */

void main( void )
{
    uint8 vol;
    boolean flag;

    sys_init();
    uart0_init();
    segs_init();
    keypad_init();
    timers_init();
    uda1341ts_init();

    uda1341ts_setvol(VOL_MAX);

    /************************************/

    uart0_puts( "\n\nReproducción de audio no comprimido (estéreo, 16b, 16KHz) por pooling:\n" );

    iis_init( IIS_POLLING );

    /************************************/

    uart0_puts( "  - Reproducción a volumen medio: escuche... " );
    uda1341ts_setvol( VOL_MED );
    iis_play( NOKIATUNE, NOKIATUNE_SIZE, FALSE );
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción a volumen variable: escuche... " );
    for( vol=0; vol<63; vol++ )
    {
        uda1341ts_setvol( vol );
        iis_play( DTMF1, DTMF_SIZE, FALSE );
    }
    uart0_puts( "OK\n" );
    uda1341ts_setvol( VOL_MAX );

    /************************************/

    uart0_puts( "  - Grabacion: hable mientras los 7 segmentos estén encendidos... " );
    segs_putchar( 8 );
    iis_rec( REC_BUFFER, REC_SIZE );
    segs_off();
    uart0_puts( "OK\n" );
    uart0_puts( "  - Reproducción de grabación: escuche... " );
    iis_play( REC_BUFFER, REC_SIZE, FALSE );
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "\nReproducción de audio no comprimido (estéreo, 16b, 16KHz) por DMA:\n" );

    iis_init( IIS_DMA );

    /************************************/

    uart0_puts( "  - Reproducción a volumen medio: escuche..." );
    uda1341ts_setvol( VOL_MED );
    iis_play( NOKIATUNE, NOKIATUNE_SIZE, FALSE );
    while( iis_status() )
    {
        sw_delay_ms( 500 );
        uart0_putchar( '.' );
    }
    uart0_puts( " OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción a volumen variable: escuche..." );
    iis_play( NOKIATUNE, NOKIATUNE_SIZE, FALSE );
    vol = 0;
    while( iis_status() )
    {
        uda1341ts_setvol( vol );
        vol = ( vol != 63 ? vol+1 : 0 );
        sw_delay_ms( 20 );
    }
    uart0_puts( " OK\n" );
    uda1341ts_setvol( VOL_MAX );

    /************************************/

    uart0_puts( "  - Grabacion: hable mientras los 7 segmentos estén encendidos..." );
    segs_putchar( 8 );
    iis_rec( REC_BUFFER, REC_SIZE );
    while( iis_status() )
    {
        sw_delay_ms( 500 );
        uart0_putchar( '.' );
    }
    segs_off();
    uart0_puts( " OK\n" );
    uart0_puts( "  - Reproducción de grabación: escuche..." );
    iis_play( REC_BUFFER, REC_SIZE, FALSE );
    while( iis_status() )
    {
        sw_delay_ms( 500 );
        uart0_putchar( '.' );
    }
    uart0_puts( " OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción continua: escuche...\n" );
    uart0_puts( "    (para finalizar presione cualquier tecla)\n    " );
    uda1341ts_setvol( VOL_MED );
    iis_playWawFile( NOKIATUNE, TRUE );
    while( !keypad_pressed() )
    {
        sw_delay_ms( 500 );
        uart0_putchar( '.' );
    }
    keypad_getchar();
    iis_pause();
    uart0_puts( " OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción continua con pause: escuche...\n" );
    uart0_puts( "    (para finalizar presione la tecla 0)\n" );
    uart0_puts( "    (para parar/continuar presione cualquier otra tecla)\n" );
    iis_playWawFile( NOKIATUNE, TRUE );
    flag = FALSE;
    while( keypad_getchar() )
    {
        if( flag )
        {
            iis_continue();
            uart0_puts( "    PLAY\n" );
        }
        else
        {
           iis_pause();
           uart0_puts( "    PAUSE\n" );
        }
        flag = !flag;
    };
    iis_pause();
    uart0_puts( "    ... OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción continua con mute: escuche...\n" );
    uart0_puts( "    (para finalizar presione la tecla 0)\n" );
    uart0_puts( "    (para parar/continuar presione cualquier otra tecla)\n" );
    iis_playWawFile( NOKIATUNE, TRUE );
    flag = FALSE;
    while( keypad_getchar() )
    {
        if( flag )
        {
            uda1341ts_mute( MUTE_OFF );
            uart0_puts( "    MUTE OFF\n" );
        }
        else
        {
            uda1341ts_mute( MUTE_ON );
            uart0_puts( "    MUTE ON\n" );
        }
        flag = !flag;
    };
    iis_pause();
    uart0_puts( "    ... OK\n" );

    /************************************/

    uart0_puts( "  - Reproduccion libre de sonido: pulse el teclado y escuche...\n" );
    while( 1 )
        switch( keypad_getchar() )
        {
            case KEYPAD_KEY0:
                iis_playWawFile( DTMF1, FALSE );
                break;
            case KEYPAD_KEY1:
                iis_playWawFile( DTMF2, FALSE );
                break;
            case KEYPAD_KEY2:
                iis_playWawFile( DTMF3, FALSE );
                break;
            case KEYPAD_KEY3:
                iis_playWawFile( INVITACION, FALSE );
                break;
            case KEYPAD_KEY4:
                iis_playWawFile( DTMF4, FALSE );
                break;        
            case KEYPAD_KEY5:
                iis_playWawFile( DTMF5, FALSE );
                break;
            case KEYPAD_KEY6:
                iis_playWawFile( DTMF6, FALSE );
                break;
            case KEYPAD_KEY7:
                iis_playWawFile( LLAMADA, FALSE );
                iis_playWawFile( LLAMADA, FALSE );
                iis_playWawFile( LLAMADA, FALSE );
                iis_playWawFile( LLAMADA, FALSE );
                break;
            case KEYPAD_KEY8:
                iis_playWawFile( DTMF7, FALSE );
                break;
            case KEYPAD_KEY9:
                iis_playWawFile( DTMF8, FALSE );
                break;    
            case KEYPAD_KEYA:
                iis_playWawFile( DTMF9, FALSE );
                break;
            case KEYPAD_KEYB:
                iis_playWawFile( CONGESTION, FALSE );
                iis_playWawFile( CONGESTION, FALSE );
                iis_playWawFile( CONGESTION, FALSE );
                iis_playWawFile( CONGESTION, FALSE );
                break;
            case KEYPAD_KEYC:
                iis_playWawFile( DTMFSTAR, FALSE );
                break;
            case KEYPAD_KEYD:
                iis_playWawFile( DTMF0, FALSE );
                break;
            case KEYPAD_KEYE:
                iis_playWawFile( DTMFSHARP, FALSE );
                break;
            case KEYPAD_KEYF:
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                iis_playWawFile( OCUPADO, FALSE );
                break;            
            default:
                break;
        };       
}
