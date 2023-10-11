/*-------------------------------------------------------------------
**
**  Fichero:
**    s3cev40.h  20/12/2016
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

#ifndef __S3CEV40_H__
#define __S3CEV40_H__

#include <common_types.h>

#define CPU   ("S3C44B0X")
#define CLKIN (8000000U)    /* Frecuencia base del oscilador (Hz) */

/* Mapa de memoria */

#define ROM_START_ADDRESS (0x00000000)
#define ROM_END_ADDRESS   (0x001fffff)
#define ROM_SIZE          (ROM_END_ADDRESS-ROM_START_ADDRESS+1)

#define DEVICES_START     (0x01c00000)
#define DEVICES_END       (0x0bffffff)

#define	SEGS              (*(volatile uint8 *)0x02140000)
#define KEYPAD_ADDR       ((volatile uint8 *)0x06000000)

#define RAM_START_ADDRESS (0x0c000000)
#define RAM_END_ADDRESS   (0x0c7fffff)
#define RAM_SIZE          (RAM_END_ADDRESS-RAM_START_ADDRESS+1)

#define SP_START_ADDRESS  (0x10000000)
#define SP_END_ADDRESS    (0x100001ff)
#define SP_SIZE           (SP_END_ADDRESS-SP_START_ADDRESS+1)

#define CACHE_LRU_START   (0x10004000)
#define CACHE_LRU_END     (0x100047ff)

/* Direcciones de las bases de las pilas del sistema */

#define USRSTACK          (0xc7ff000)
#define SVCSTACK          (0xc7ff100)
#define UNDSTACK          (0xc7ff200)
#define ABTSTACK          (0xc7ff300)
#define IRQSTACK          (0xc7ff400)
#define FIQSTACK          (0xc7ff500)

/* Tiempos de rebote en ms */

#define KEYPAD_KEYDOWN_DELAY  (30)
#define KEYPAD_KEYUP_DELAY    (100)
#define PB_KEYDOWN_DELAY      (30)
#define PB_KEYUP_DELAY        (100)
#define TS_DOWN_DELAY         (30)
#define TS_UP_DELAY           (100)

/* Tabla virtual de vectores de excepción */

#define pISR_RESET    (*(volatile uint32 *)0xc7fff00)
#define pISR_UNDEF    (*(volatile uint32 *)0xc7fff04)
#define pISR_SWI      (*(volatile uint32 *)0xc7fff08)
#define pISR_PABORT   (*(volatile uint32 *)0xc7fff0c)
#define pISR_DABORT   (*(volatile uint32 *)0xc7fff10)
/* Reservado */
#define pISR_IRQ      (*(volatile uint32 *)0xc7fff18)
#define pISR_FIQ      (*(volatile uint32 *)0xc7fff1c)
#define pISR_ADC      (*(volatile uint32 *)0xc7fff20)
#define pISR_RTC      (*(volatile uint32 *)0xc7fff24)
#define pISR_UTXD1    (*(volatile uint32 *)0xc7fff28)
#define pISR_UTXD0    (*(volatile uint32 *)0xc7fff2c)
#define pISR_SIO      (*(volatile uint32 *)0xc7fff30)
#define pISR_IIC      (*(volatile uint32 *)0xc7fff34)
#define pISR_URXD1    (*(volatile uint32 *)0xc7fff38)
#define pISR_URXD0    (*(volatile uint32 *)0xc7fff3c)
#define pISR_TIMER5   (*(volatile uint32 *)0xc7fff40)
#define pISR_TIMER4   (*(volatile uint32 *)0xc7fff44)
#define pISR_TIMER3   (*(volatile uint32 *)0xc7fff48)
#define pISR_TIMER2   (*(volatile uint32 *)0xc7fff4c)
#define pISR_TIMER1   (*(volatile uint32 *)0xc7fff50)
#define pISR_TIMER0   (*(volatile uint32 *)0xc7fff54)
#define pISR_UERR01   (*(volatile uint32 *)0xc7fff58)
#define pISR_WDT      (*(volatile uint32 *)0xc7fff5c)
#define pISR_BDMA1    (*(volatile uint32 *)0xc7fff60)
#define pISR_BDMA0    (*(volatile uint32 *)0xc7fff64)
#define pISR_ZDMA1    (*(volatile uint32 *)0xc7fff68)
#define pISR_ZDMA0    (*(volatile uint32 *)0xc7fff6c)
#define pISR_TICK     (*(volatile uint32 *)0xc7fff70)
#define pISR_PB       (*(volatile uint32 *)0xc7fff74) // EINT4567
#define pISR_ETHERNET (*(volatile uint32 *)0xc7fff78) // EINT3
#define pISR_TS       (*(volatile uint32 *)0xc7fff7c) // EINT2
#define pISR_KEYPAD   (*(volatile uint32 *)0xc7fff80) // EINT1
#define pISR_USB      (*(volatile uint32 *)0xc7fff84) // EINT0

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
#define BIT_PB        (1<<21) // EINT4567
#define BIT_ETHERNET  (1<<22) // EINT3
#define BIT_TS        (1<<23) // EINT2
#define BIT_KEYPAD    (1<<24) // EINT1
#define BIT_USB       (1<<25) // EINT2

/* Bits de EXTINTPND */
#define BIT_RIGHTPB   (1<<3)
#define BIT_LEFTPB    (1<<2)
#define BIT_IDE_INTRQ (1<<1)
#define BIT_IDE_DMACK (1<<0)

#endif 
