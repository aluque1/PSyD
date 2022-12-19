/*-------------------------------------------------------------------
**
**  Fichero:
**    lab9-efectos.c  11/5/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Test del laboratorio 9 (1a. parte)
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <common_types.h>
#include <system.h>
#include <segs.h>
#include <keypad.h>
#include <uart.h>
#include <uda1341ts.h>
#include <iis.h>

#define AUDIOBUFFER_SIZE   (16000*10) // 16000sps * 10s = 160000 samples
#define AUDIODELAY_MAXSIZE (16000*2)  // 16000sps * 2s  = 32000 samples
#define WAVETABLE_SIZE     (36)       // 16000sps / 440wps (LA) = 36 samples/wave

typedef struct {
	int16 ch0[AUDIOBUFFER_SIZE];
	int16 ch1[AUDIOBUFFER_SIZE];
} audiobuffer_t;

typedef struct {
	int16 ch0[AUDIODELAY_MAXSIZE];
	int16 ch1[AUDIODELAY_MAXSIZE];
	uint16 size;
	uint16 head;
	uint16 tail;
} audiodelay_t;

audiobuffer_t buffer;
audiodelay_t delay_buffer;

/*
 * Tablas correspondientes a una onda de frecuencia 440 Hz (LA central) muestreada a 16000sps,
 * el patron de valores se repite cada 36 = 16000/440 muestras.
 *
 * Las muestras del audio codec son de 16 bits y los valores de la tabla requieren solo 11 bits,
 * por lo que la onda almacenada tiene un volumen bajo.
 */

int16 triangle[WAVETABLE_SIZE] =
{
      0,  100,  200,  300,  400,  500,  600,  700,  800,  900,  800,  700,  600,  500,  400,  300,  200,  100,
      0, -100, -200, -300, -400, -500, -600, -700, -800, -900, -800, -700, -600, -500, -400, -300, -200, -100
};


int16 square[WAVETABLE_SIZE] =
{
	900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,  900,
   -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900, -900
};

int16 sine[WAVETABLE_SIZE] =   // Calculado como sine[i] = 900 * sin(2*PI*i/36)
{
	  0,  156,  308,  450,  579,  689,  779,  846,  886,  900,  886,  846,  779,  689,  579,  450,  308,  156,
	  0, -156, -308, -450, -579, -689, -779, -846, -886, -900, -886, -846, -779, -689, -579, -450, -308, -156
};

void audiodelay_init( audiodelay_t *buffer, uint16 size );
void audiodelay_put( audiodelay_t *buffer, int16 ch0, int16 ch1 );
void audiodelay_get( audiodelay_t *buffer, int16 *ch0, int16 *ch1 );

void main( void )
{
	uint32 i;
	int16 ch0, ch1;
	int16 ch0_aux, ch1_aux;

	sys_init();
    uart0_init();
    segs_init();
    keypad_init();
    uda1341ts_init();
    iis_init( IIS_POLLING );

    uda1341ts_setvol(VOL_MAX);

    /************************************/

    uart0_puts( "\n\nReproducción de sonido tabulado (LA central 440 Hz)\n" );
    uart0_puts( "(para finalizar presione cualquier tecla)\n" );

    /************************************/

    uart0_puts( "  - Sinusoide: escuche... " );
    i = 0;
    while( !keypad_pressed() )
    {
     	iis_putSample( sine[i], sine[i] );
     	if( ++i > WAVETABLE_SIZE )
     		i = 0;
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Onda cuadrada: escuche... " );
	i = 0;
    while( !keypad_pressed() )
    {
     	iis_putSample( square[i], square[i] );
     	if( ++i > WAVETABLE_SIZE )
     		i = 0;
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Onda triangular: escuche... " );
    i = 0;
	while( !keypad_pressed() )
    {
     	iis_putSample( triangle[i], triangle[i] );
     	if( ++i > WAVETABLE_SIZE )
     		i = 0;
    }
    keypad_getchar();
	uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "\nEfectos de audio (dominio temporal)\n" );

    /************************************/

    uart0_puts( "  - Grabacion: hable mientras los 7 segmentos estén encendidos... " );
    segs_putchar( 8 );
    for( i=0; i<AUDIOBUFFER_SIZE; i++ )
     	iis_getSample( &(buffer.ch0[i]), &(buffer.ch1[i]) );
    segs_off();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción a velocidad normal: escuche... " );
    for( i=0; i<AUDIOBUFFER_SIZE; i++ )
     	iis_putSample( buffer.ch0[i], buffer.ch1[i] );
    uart0_puts( "OK\n" );

    uart0_puts( "  - Reproducción a media velocidad: escuche... " );
    for( i=0; i<AUDIOBUFFER_SIZE; i++ )
    {
     	iis_putSample( buffer.ch0[i], buffer.ch1[i] );	    // cada muestra almacenada se reproduce 2 veces
     	iis_putSample( buffer.ch0[i], buffer.ch1[i] );
     }
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción a doble velocidad: escuche... " );
    for( i=0; i<AUDIOBUFFER_SIZE; i+=2 )		 	// se reproduce 1 de cada 2 muestras almacenadas
     	iis_putSample( buffer.ch0[i], buffer.ch1[i] );
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Reproducción inversa: escuche... " );
    for( i=0; i<AUDIOBUFFER_SIZE; i++ )
     	iis_putSample( buffer.ch0[AUDIOBUFFER_SIZE-i], buffer.ch1[AUDIOBUFFER_SIZE-i] );
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "(para finalizar presione cualquier tecla)\n" );
    uart0_puts( "  - Loopback: hable y escuche... " );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
     	iis_putSample( ch0, ch1 );
     }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Echo (8 ms): hable y escuche... " );
    audiodelay_init( &delay_buffer, 128 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
     	ch0_aux = ch0 + (ch0_aux >> 1);
     	ch1_aux = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0_aux, ch1_aux );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Echo (45 ms): hable y escuche... " );
    audiodelay_init( &delay_buffer, 720 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
     	ch0_aux = ch0 + (ch0_aux >> 1);
     	ch1_aux = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0_aux, ch1_aux );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Echo (85 ms): hable y escuche... " );
    audiodelay_init( &delay_buffer, 1280 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
     	ch0_aux = ch0 + (ch0_aux >> 1);
     	ch1_aux = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0_aux, ch1_aux );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Echo (150 ms): hable y escuche... " );
    audiodelay_init( &delay_buffer, 2400 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
     	ch0_aux = ch0 + (ch0_aux >> 1);
     	ch1_aux = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0_aux, ch1_aux );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Echo & fade (1 s): hable y escuche... " );
    audiodelay_init( &delay_buffer, 16000 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
      	ch0 = ch0 + (ch0_aux >> 1);
      	ch1 = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0, ch1 );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
    }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    uart0_puts( "  - Reverberación (echo & fade 100 ms): hable y escuche... " );
    audiodelay_init( &delay_buffer, 1600 );
    while( !keypad_pressed() )
    {
     	iis_getSample( &ch0, &ch1 );
    	audiodelay_get( &delay_buffer, &ch0_aux, &ch1_aux );
      	ch0 = ch0 + (ch0_aux >> 1);
      	ch1 = ch1 + (ch1_aux >> 1);
     	iis_putSample( ch0, ch1 );
    	audiodelay_put( &delay_buffer, ch0, ch1 );
     }
    keypad_getchar();
    uart0_puts( "OK\n" );

    /************************************/

    while(1);

}

void audiodelay_init( audiodelay_t *buffer, uint16 size )
{
	uint16 i;

	for( i=0; i<size; i++ )
	{
		buffer->ch0[i] = 0;
		buffer->ch1[i] = 0;
	}
	buffer->head = 0;
	buffer->tail = 0;
	buffer->size = size;
}

void audiodelay_put( audiodelay_t *buffer, int16 ch0, int16 ch1 )
{
	buffer->ch0[buffer->head] = ch0;
	buffer->ch1[buffer->head] = ch1;
	if( ++(buffer->head) == buffer->size )
		buffer->head = 0;
}

void audiodelay_get( audiodelay_t *buffer, int16 *ch0, int16 *ch1 )
{
	*ch0 = buffer->ch0[buffer->tail];
	*ch1 = buffer->ch1[buffer->tail];
	if( ++(buffer->tail) == buffer->size )
		buffer->tail = 0;
}

