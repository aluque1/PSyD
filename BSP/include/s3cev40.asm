/*-------------------------------------------------------------------
**
**  Fichero:
**    s3cev40.asm  24/4/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Creación de nemotécnicos para acceso a distintos elementos
**    de la placa de prototipado Embest S3CEV40
**
**  Notas de diseño:
**    Las direcciones usadas asumen un alineamiento little-endian
**
**-----------------------------------------------------------------*/

/* Mapa de memoria */

.equ ROM_START_ADDRESS, 0x00000000
.equ ROM_END_ADDRESS,   0x001fffff

.equ DEVICES_START,     0x01c00000
.equ DEVICES_END,       0x0bffffff

.equ RAM_START_ADDRESS, 0x0c000000
.equ RAM_END_ADDRESS,   0x0c7fffff

.equ SP_START_ADDRESS,  0x10000000
.equ SP_END_ADDRESS,    0x100001ff

.equ CACHE_LRU_START,   0x10004000
.equ CACHE_LRU_END,     0x100047ff

/* Direcciones de las bases de las pilas del sistema */

.equ USRSTACK,      0xc7ff000       
.equ SVCSTACK,      0xc7ff100
.equ UNDSTACK,      0xc7ff200
.equ ABTSTACK,      0xc7ff300
.equ IRQSTACK,      0xc7ff400
.equ FIQSTACK,      0xc7ff500

/* Tabla virtual de vectores de excepción */

.equ pISR_RESET,    0xc7fff00
.equ pISR_UNDEF,    0xc7fff04
.equ pISR_SWI,      0xc7fff08
.equ pISR_PABORT,   0xc7fff0c
.equ pISR_DABORT,   0xc7fff10
/* Reservado */
.equ pISR_IRQ,      0xc7fff18
.equ pISR_FIQ,      0xc7fff1c
.equ pISR_ADC,      0xc7fff20
.equ pISR_RTC,      0xc7fff24
.equ pISR_UTXD1,    0xc7fff28
.equ pISR_UTXD0,    0xc7fff2c
.equ pISR_SIO,      0xc7fff30
.equ pISR_IIC,      0xc7fff34
.equ pISR_URXD1,    0xc7fff38
.equ pISR_URXD0,    0xc7fff3c
.equ pISR_TIMER5,   0xc7fff40
.equ pISR_TIMER4,   0xc7fff44
.equ pISR_TIMER3,   0xc7fff48
.equ pISR_TIMER2,   0xc7fff4c
.equ pISR_TIMER1,   0xc7fff50
.equ pISR_TIMER0,   0xc7fff54
.equ pISR_UERR01,   0xc7fff58
.equ pISR_WDT,      0xc7fff5c
.equ pISR_BDMA1,    0xc7fff60
.equ pISR_BDMA0,    0xc7fff64
.equ pISR_ZDMA1,    0xc7fff68
.equ pISR_ZDMA0,    0xc7fff6c
.equ pISR_TICK,     0xc7fff70
.equ pISR_PB,       0xc7fff74 /* EINT4567 */
.equ pISR_ETHERNET, 0xc7fff78 /* EINT3    */
.equ pISR_TS,       0xc7fff7c /* EINT2    */
.equ pISR_KEYPAD,   0xc7fff80 /* EINT1    */
.equ pISR_USB,      0xc7fff84 /* EINT0    */

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
.equ BIT_PB,        (1<<21) /* EINT4567 */
.equ BIT_ETHERNET,  (1<<22) /* EINT3    */
.equ BIT_TS,        (1<<23) /* EINT2    */
.equ BIT_KEYPAD,    (1<<24) /* EINT1    */
.equ BIT_USB,       (1<<25) /* EINT0    */

/* Bits de EXTINTPND */
.equ BIT_RIGHTPB,   (1<<3)
.equ BIT_LEFTPB,    (1<<2)
.equ BIT_IDE_INTRQ, (1<<1)
.equ BIT_IDE_DMACK, (1<<0)
