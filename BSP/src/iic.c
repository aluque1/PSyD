
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <iic.h>

void iic_init( void )
{
    IICADD = 0x0; // Direccion como escalavo, no tiene direccion porque nunca se va a ver como esclavo
        /**
        *  IICCON[6] = 0    :: IICCLK = MCLK/16
        *  IICCON[3:0] = 15 :: TxCLK = IICCLK / (15+1) = 64MHz / 256
        */
    IICCON = 0xAF; // Establecemos la frecuencia de transmision a 250KHz
        /**
        *  IICSTAT[7:6] = XX :: La seleccion de modo que en el init nos da igual
        *  IICSTAT[4] = 15   :: Serial output enable
        */
    IICSTAT = 0X10; // Modo de transmision, en este caso habilitamos la lectura/escritura
}

void iic_start( uint8 mode, uint8 byte )
{
    /**
     * Como estamos comenzando el iic tenemos que primer tenemos que iniciar la transmision (tenemos que crear una transicion de 1 a 0 en SDA (esto es el start cond))
    */
    IICDS   = byte; // Escribimos el dato en el registro de transmision
    IICSTAT = (mode | 0x20);// Escribimos un 1 en ICCSTAT[5] (esto genera el start condition) y ponemos el modo IICSTAT[7:6] como se pasa por arg
    IICCON &= ~(1 << 4); // Arrancamos la transmision del dato (escribir 0 en IICCON[4])
    while((IICCON & 0x10) != 0x10); // Espermos a que termine la transmision (recepcion de ACK)
}


void iic_putByte( uint8 byte )
{
    IICDS   = byte;
    IICCON &= ~(1 << 4);
    while((IICCON & 0x10) != 0x10);    
}

uint8 iic_getByte( uint8 ack )
{
    IICCON  = ack;
    IICCON &= ~(1 << 4);
    //TODO no se si los whiles estan bien pero creo que si
    while((IICCON & 0x10) != 0x10); // espera la recepcion de un dato 
    return IICDS;
}

void iic_stop( uint16 ms )
{
    IICSTAT &= ~(1 << 5); // genera stop condition IICSTAT[5] = 0
    IICCON  &= ~(1 << 4); // arranca la tranmision del bit de stop
    sw_delay_ms( ms );
}
