/*-------------------------------------------------------------------
**
**  Fichero:
**    lab2.c  14/3/2016
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Test del laboratorio 2
**
**  Notas de diseño:
**    Presupone que todo el SoC ha sido previamente configurado
**    por el programa residente en ROM que se ejecuta tras reset
**
**-----------------------------------------------------------------*/

#define PCONB (*(volatile unsigned int *)0x01D20008)
#define PDATB (*(volatile unsigned int *)0x01D2000C)

#define PCONG (*(volatile unsigned int *)0x01D20040)
#define PDATG (*(volatile unsigned int *)0x01D20044)
#define PUPG  (*(volatile unsigned int *)0x01D20048)
    
void main(void) 
{
    PCONB &= ~( (1<<10) | (1<<9) );  // PB[10] = out, PF[9] = out
    PCONG &= ~( (3<<14) | (3<<16) );  // PG[7] = in, PG[6] = in
    PUPG  |= (3<<6);                 // Deshabilita pull-up de PG[7] y PG[6]

    while( 1 )
        PDATB = PDATG << 3;    // PB[10:9] = PG[7:6]
}
