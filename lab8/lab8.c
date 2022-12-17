/*-------------------------------------------------------------------
**
**  Fichero:
**    lab8.c  17/5/2013
**
**    (c) J.M. Mendias
**    Programaci�n de Sistemas y Dispositivos
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Test del laboratorio 8
**
**  Notas de dise�o:
**
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <system.h>
#include <timers.h>
#include <uart.h>
#include <iic.h>
#include <at24c04.h>

void main( void )
{
    static uint8 buffer[AT24C04_DEPTH], buffer_aux[AT24C04_DEPTH];
    uint16 i, j, delay;

    static uint32 addr, data;
    
    sys_init();
    uart0_init();
    timers_init();
    iic_init();

    /************************************/

    uart0_puts( "\n\nCargando en RAM el contenido de IIC-EEPROM AT24C04:\n" );

    /************************************/

    timer3_start(); 
    for( i=0; i<AT24C04_DEPTH; i++ )
        at24c04_byteread( i, &buffer[i] ); 
    delay = timer3_stop( );
    uart0_puts( " - Lectura aleatoria (" );
    uart0_putint( AT24C04_DEPTH );
    uart0_puts( " bytes): " );
    uart0_putint( delay/10 );
    uart0_puts( " ms\n" );

    /************************************/

    timer3_start(); 
    at24c04_load( buffer_aux );
    delay = timer3_stop( );
    uart0_puts( " - Lectura secuencial (" );
    uart0_putint( AT24C04_DEPTH );
    uart0_puts( " bytes): " );
    uart0_putint( delay/10 );
    uart0_puts( " ms\n" );

    /************************************/

    for( i=0; i<AT24C04_DEPTH; i++ )
        if( buffer[i] != buffer_aux[i] )
            uart0_puts( "ERROR: lectura aleatoria y secuencial no son coincidentes\n" ); 

    /************************************/

    uart0_puts( "\nVolcado del contenido inicial de la memoria IIC-EEPROM AT24C04:\n" );
        
    for( i=0; i < (AT24C04_DEPTH>>4); i++ ){
        uart0_puts( "\n" );
        if( i<<4 <= 0xf )
            uart0_puts( "00" );
        else if( i<<4 <= 0xff )
            uart0_putchar( '0' );
        uart0_puthex( i<<4 );
        uart0_puts( "h:"  );
        for( j=0; j<16; j++ )
        {
            if( buffer[(i<<4)+j] <= 0xf )
                uart0_putchar( '0' );
            uart0_puthex( buffer[(i<<4)+j] );
        }
        uart0_puts( " ; " );
        for( j=0; j<16; j++ )
              uart0_putchar( ( (31<buffer[(i<<4)+j]) && (buffer[(i<<4)+j]<127) ? buffer[(i<<4)+j] : '.' ) );
    };

    // BORRAR ESTO
    uart0_puts( "\nVolcado su puta madre (secuencial):\n" );

    for( i=0; i < (AT24C04_DEPTH>>4); i++ ){
            uart0_puts( "\n" );
            if( i<<4 <= 0xf )
                uart0_puts( "00" );
            else if( i<<4 <= 0xff )
                uart0_putchar( '0' );
            uart0_puthex( i<<4 );
            uart0_puts( "h:"  );
            for( j=0; j<16; j++ )
            {
                if( buffer_aux[(i<<4)+j] <= 0xf )
                    uart0_putchar( '0' );
                uart0_puthex( buffer_aux[(i<<4)+j] );
            }
            uart0_puts( " ; " );
            for( j=0; j<16; j++ )
                  uart0_putchar( ( (31<buffer_aux[(i<<4)+j]) && (buffer_aux[(i<<4)+j]<127) ? buffer_aux[(i<<4)+j] : '.' ) );
        };

    /************************************/

    uart0_puts( "\n\nModificacion del contenido de la memoria IIC-EEPROM AT24C04:\n" );    

    for( i=0; i<5; i++ )
    {
        uart0_puts( "  - Introduzca una direccion (0h-1ffh): " );
        addr = uart0_gethex( );
        uart0_puts( "  - Introduzca el dato (0h-ffh): " );
        data = uart0_gethex( );
        buffer[addr] = data;
    }

    /************************************/

    uart0_puts( "\nVolcando en la IIC-EEPROM AT24C04 el contenido de la RAM:\n" );

    /************************************/

    timer3_start(); 
    for( i=0; i<AT24C04_DEPTH; i++ )
        at24c04_bytewrite( i, buffer[i] ); 
    delay = timer3_stop( );
    uart0_puts( " - Escritura aleatoria (" );
    uart0_putint( AT24C04_DEPTH );
    uart0_puts( " bytes): " );
    uart0_putint( delay/10 );
    uart0_puts( " ms\n" );

    at24c04_load( buffer_aux );
    for( i=0; i<AT24C04_DEPTH; i++ )
        if( buffer[i] != buffer_aux[i] )
            uart0_puts( "ERROR: La escritura aleatoria falla.\n" );     

    /************************************/

    timer3_start(); 
    at24c04_store( buffer );
    delay = timer3_stop( );
    uart0_puts( " - Escritura secuencial (" );
    uart0_putint( AT24C04_DEPTH );
    uart0_puts( " bytes): " );
    uart0_putint( delay/10 );
    uart0_puts( " ms\n" );

    at24c04_load( buffer_aux );
    for( i=0; i<AT24C04_DEPTH; i++ )
        if( buffer[i] != buffer_aux[i] )
            uart0_puts( "ERROR: La escritura secuencial falla.\n" );     

    /************************************/

    uart0_puts( "\nApague la placa y vuelva a cargar y ejecutar el programa.\n" );    

    /************************************/

    while( 1 );
}
