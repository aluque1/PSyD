/*-------------------------------------------------------------------
**
**  Fichero:
**    s3c44b0x.h  2/2/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Creación de nemotécnicos para los registros de los
**    controladores de dispositivos internos del chip S3C44BOX
**
**  Notas de diseño:
**    Las direcciones usadas asumen un alineamiento little-endian
**
**-----------------------------------------------------------------*/

#ifndef __S3C44BOX_H__
#define __S3C44BOX_H__

#include <common_types.h>

/* Controlador de memoria */
#define BWSCON      (*(volatile uint32 *)0x1c80000)
#define BANKCON0    (*(volatile uint32 *)0x1c80004)
#define BANKCON1    (*(volatile uint32 *)0x1c80008)
#define BANKCON2    (*(volatile uint32 *)0x1c8000c)
#define BANKCON3    (*(volatile uint32 *)0x1c80010)
#define BANKCON4    (*(volatile uint32 *)0x1c80014)
#define BANKCON5    (*(volatile uint32 *)0x1c80018)
#define BANKCON6    (*(volatile uint32 *)0x1c8001c)
#define BANKCON7    (*(volatile uint32 *)0x1c80020)
#define REFRESH     (*(volatile uint32 *)0x1c80024)
#define BANKSIZE    (*(volatile uint32 *)0x1c80028)
#define MRSRB6      (*(volatile uint32 *)0x1c8002c)
#define MRSRB7      (*(volatile uint32 *)0x1c80030)

/* Gestor de reloj y energía */
#define PLLCON      (*(volatile uint32 *)0x1d80000)
#define CLKCON      (*(volatile uint32 *)0x1d80004)
#define CLKSLOW     (*(volatile uint32 *)0x1d80008)
#define LOCKTIME    (*(volatile uint32 *)0x1d8000c)

/* Controlador de cache */
#define SYSCFG      (*(volatile uint32 *)0x1c00000)
#define NCACHBE0    (*(volatile uint32 *)0x1c00004)
#define NCACHBE1    (*(volatile uint32 *)0x1c00008)

/* Árbitro de bus */
#define SBUSCON     (*(volatile uint32 *)0x1c40000)
 
/* Controlador de DMA */ 
#define ZDCON0      (*(volatile uint32 *)0x1e80000) /* canal ZDMA 0 */
#define ZDISRC0     (*(volatile uint32 *)0x1e80004)
#define ZDIDES0     (*(volatile uint32 *)0x1e80008)
#define ZDICNT0     (*(volatile uint32 *)0x1e8000c)
#define ZDCSRC0     (*(volatile uint32 *)0x1e80010)
#define ZDCDES0     (*(volatile uint32 *)0x1e80014)
#define ZDCCNT0     (*(volatile uint32 *)0x1e80018)
#define ZDCON1      (*(volatile uint32 *)0x1e80020) /* canal ZDMA 1 */
#define ZDISRC1     (*(volatile uint32 *)0x1e80024)
#define ZDIDES1     (*(volatile uint32 *)0x1e80028)
#define ZDICNT1     (*(volatile uint32 *)0x1e8002c)
#define ZDCSRC1     (*(volatile uint32 *)0x1e80030)
#define ZDCDES1     (*(volatile uint32 *)0x1e80034)
#define ZDCCNT1     (*(volatile uint32 *)0x1e80038)
#define BDCON0      (*(volatile uint32 *)0x1f80000) /* canal BDMA 0 */
#define BDISRC0     (*(volatile uint32 *)0x1f80004)
#define BDIDES0     (*(volatile uint32 *)0x1f80008)
#define BDICNT0     (*(volatile uint32 *)0x1f8000c)
#define BDCSRC0     (*(volatile uint32 *)0x1f80010)
#define BDCDES0     (*(volatile uint32 *)0x1f80014)
#define BDCCNT0     (*(volatile uint32 *)0x1f80018)
#define BDCON1      (*(volatile uint32 *)0x1f80020) /* canal BDMA 1 */
#define BDISRC1     (*(volatile uint32 *)0x1f80024)
#define BDIDES1     (*(volatile uint32 *)0x1f80028)
#define BDICNT1     (*(volatile uint32 *)0x1f8002c)
#define BDCSRC1     (*(volatile uint32 *)0x1f80030)
#define BDCDES1     (*(volatile uint32 *)0x1f80034)
#define BDCCNT1     (*(volatile uint32 *)0x1f80038)

/* Controlador de puertos de E/S */
#define SPUCR       (*(volatile uint32 *)0x1d2004c)
#define PCONA       (*(volatile uint32 *)0x1d20000) /* puerto A */
#define PDATA       (*(volatile uint32 *)0x1d20004)
#define PCONB       (*(volatile uint32 *)0x1d20008) /* puerto B */
#define PDATB       (*(volatile uint32 *)0x1d2000c)
#define PCONC       (*(volatile uint32 *)0x1d20010) /* puerto C */
#define PDATC       (*(volatile uint32 *)0x1d20014)
#define PUPC        (*(volatile uint32 *)0x1d20018)
#define PCOND       (*(volatile uint32 *)0x1d2001c) /* puerto D */
#define PDATD       (*(volatile uint32 *)0x1d20020)
#define PUPD        (*(volatile uint32 *)0x1d20024)
#define PCONE       (*(volatile uint32 *)0x1d20028) /* puerto E */
#define PDATE       (*(volatile uint32 *)0x1d2002c)
#define PUPE        (*(volatile uint32 *)0x1d20030)
#define PCONF       (*(volatile uint32 *)0x1d20034) /* puerto F */
#define PDATF       (*(volatile uint32 *)0x1d20038)
#define PUPF        (*(volatile uint32 *)0x1d2003c)
#define PCONG       (*(volatile uint32 *)0x1d20040) /* puerto G */
#define PDATG       (*(volatile uint32 *)0x1d20044)
#define PUPG        (*(volatile uint32 *)0x1d20048)
#define EXTINT      (*(volatile uint32 *)0x1d20050)
#define EXTINTPND   (*(volatile uint32 *)0x1d20054)
 
/* Bits de EXTINTPND */
#define BIT_EINT4   (1<<0)
#define BIT_EINT5   (1<<1)
#define BIT_EINT6   (1<<2)
#define BIT_EINT7   (1<<3)

/* Temporizadores */
#define TCFG0       (*(volatile uint32 *)0x1d50000) 
#define TCFG1       (*(volatile uint32 *)0x1d50004)
#define TCON        (*(volatile uint32 *)0x1d50008)
#define TCNTB0      (*(volatile uint32 *)0x1d5000c) /* temporizador 0 */
#define TCMPB0      (*(volatile uint32 *)0x1d50010)
#define TCNTO0      (*(volatile uint32 *)0x1d50014)
#define TCNTB1      (*(volatile uint32 *)0x1d50018) /* temporizador 1 */
#define TCMPB1      (*(volatile uint32 *)0x1d5001c)
#define TCNTO1      (*(volatile uint32 *)0x1d50020)
#define TCNTB2      (*(volatile uint32 *)0x1d50024) /* temporizador 2 */
#define TCMPB2      (*(volatile uint32 *)0x1d50028)
#define TCNTO2      (*(volatile uint32 *)0x1d5002c)
#define TCNTB3      (*(volatile uint32 *)0x1d50030) /* temporizador 3 */
#define TCMPB3      (*(volatile uint32 *)0x1d50034)
#define TCNTO3      (*(volatile uint32 *)0x1d50038)
#define TCNTB4      (*(volatile uint32 *)0x1d5003c) /* temporizador 4 */
#define TCMPB4      (*(volatile uint32 *)0x1d50040)
#define TCNTO4      (*(volatile uint32 *)0x1d50044)
#define TCNTB5      (*(volatile uint32 *)0x1d50048) /* temporizador 5 */
#define TCNTO5      (*(volatile uint32 *)0x1d5004c)
 
/* UART */
#define ULCON0      (*(volatile uint32 *)0x1d00000) /* canal 0 */
#define UCON0       (*(volatile uint32 *)0x1d00004)
#define UFCON0      (*(volatile uint32 *)0x1d00008)
#define UMCON0      (*(volatile uint32 *)0x1d0000c)
#define UTRSTAT0    (*(volatile uint32 *)0x1d00010)
#define UERSTAT0    (*(volatile uint32 *)0x1d00014)
#define UFSTAT0     (*(volatile uint32 *)0x1d00018)
#define UMSTAT0     (*(volatile uint32 *)0x1d0001c)
#define UTXH0       (*(volatile uint32 *)0x1d00020)
#define URXH0       (*(volatile uint32 *)0x1d00024)
#define UBRDIV0     (*(volatile uint32 *)0x1d00028)
#define ULCON1      (*(volatile uint32 *)0x1d04000) /* canal 1 */
#define UCON1       (*(volatile uint32 *)0x1d04004)
#define UFCON1      (*(volatile uint32 *)0x1d04008)
#define UMCON1      (*(volatile uint32 *)0x1d0400c)
#define UTRSTAT1    (*(volatile uint32 *)0x1d04010)
#define UERSTAT1    (*(volatile uint32 *)0x1d04014)
#define UFSTAT1     (*(volatile uint32 *)0x1d04018)
#define UMSTAT1     (*(volatile uint32 *)0x1d0401c)
#define UTXH1       (*(volatile uint32 *)0x1d04020)
#define URXH1       (*(volatile uint32 *)0x1d04024)
#define UBRDIV1     (*(volatile uint32 *)0x1d04028)

/* Controlador de interrupciones */
#define INTCON      (*(volatile uint32 *)0x1e00000)
#define INTPND      (*(volatile uint32 *)0x1e00004)
#define INTMOD      (*(volatile uint32 *)0x1e00008)
#define INTMSK      (*(volatile uint32 *)0x1e0000c)
#define I_PSLV      (*(volatile uint32 *)0x1e00010)
#define I_PMST      (*(volatile uint32 *)0x1e00014)
#define I_CSLV      (*(volatile uint32 *)0x1e00018)
#define I_CMST      (*(volatile uint32 *)0x1e0001c)
#define I_ISPR      (*(volatile uint32 *)0x1e00020)
#define I_ISPC      (*(volatile uint32 *)0x1e00024)
#define F_ISPC      (*(volatile uint32 *)0x1e0003c)

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
#define BIT_ADC     (1<<0)
#define BIT_RTC     (1<<1)
#define BIT_UTXD1   (1<<2)
#define BIT_UTXD0   (1<<3)
#define BIT_SIO     (1<<4)
#define BIT_IIC     (1<<5)
#define BIT_URXD1   (1<<6)
#define BIT_URXD0   (1<<7)
#define BIT_TIMER5  (1<<8)
#define BIT_TIMER4  (1<<9)
#define BIT_TIMER3  (1<<10)
#define BIT_TIMER2  (1<<11)
#define BIT_TIMER1  (1<<12)
#define BIT_TIMER0  (1<<13)
#define BIT_UERR01  (1<<14)
#define BIT_WDT     (1<<15)
#define BIT_BDMA1   (1<<16)
#define BIT_BDMA0   (1<<17)
#define BIT_ZDMA1   (1<<18)
#define BIT_ZDMA0   (1<<19)
#define BIT_TICK    (1<<20)
#define BIT_EINT4567 (1<<21)
#define BIT_EINT3   (1<<22)
#define BIT_EINT2   (1<<23)
#define BIT_EINT1   (1<<24)
#define BIT_EINT0   (1<<25)
#define BIT_GLOBAL  (1<<26)

/* Controlador de LCD */
#define LCDCON1     (*(volatile uint32 *)0x1f00000)
#define LCDCON2     (*(volatile uint32 *)0x1f00004)
#define LCDCON3     (*(volatile uint32 *)0x1f00040)
#define LCDSADDR1   (*(volatile uint32 *)0x1f00008)
#define LCDSADDR2   (*(volatile uint32 *)0x1f0000c)
#define LCDSADDR3   (*(volatile uint32 *)0x1f00010)
#define REDLUT      (*(volatile uint32 *)0x1f00014)
#define GREENLUT    (*(volatile uint32 *)0x1f00018)
#define BLUELUT     (*(volatile uint32 *)0x1f0001c)
#define DP1_2       (*(volatile uint32 *)0x1f00020)
#define DP4_7       (*(volatile uint32 *)0x1f00024)
#define DP3_5       (*(volatile uint32 *)0x1f00028)
#define DP2_3       (*(volatile uint32 *)0x1f0002c)
#define DP5_7       (*(volatile uint32 *)0x1f00030)
#define DP3_4       (*(volatile uint32 *)0x1f00034)
#define DP4_5       (*(volatile uint32 *)0x1f00038)
#define DP6_7       (*(volatile uint32 *)0x1f0003c)
#define DITHMODE    (*(volatile uint32 *)0x1f00044)

/* Conversor analógico digital */
#define ADCCON      (*(volatile uint32 *)0x1d40000)
#define ADCPSR      (*(volatile uint32 *)0x1d40004)
#define ADCDAT      (*(volatile uint32 *)0x1d40008)

/* Reloj de tiempo real */
#define RTCCON      (*(volatile uint32 *)0x1d70040)
#define RTCALM      (*(volatile uint32 *)0x1d70050)
#define ALMSEC      (*(volatile uint32 *)0x1d70054)
#define ALMMIN      (*(volatile uint32 *)0x1d70058)
#define ALMHOUR     (*(volatile uint32 *)0x1d7005c)
#define ALMDAY      (*(volatile uint32 *)0x1d70060)
#define ALMMON      (*(volatile uint32 *)0x1d70064)
#define ALMYEAR     (*(volatile uint32 *)0x1d70068)
#define RTCRST      (*(volatile uint32 *)0x1d7006c)
#define BCDSEC      (*(volatile uint32 *)0x1d70070)
#define BCDMIN      (*(volatile uint32 *)0x1d70074)
#define BCDHOUR     (*(volatile uint32 *)0x1d70078)
#define BCDDAY      (*(volatile uint32 *)0x1d7007c)
#define BCDDATE     (*(volatile uint32 *)0x1d70080)
#define BCDMON      (*(volatile uint32 *)0x1d70084)
#define BCDYEAR     (*(volatile uint32 *)0x1d70088)
#define TICNT       (*(volatile uint32 *)0x1d7008c)

/* Watchdog timer */
#define WTCON       (*(volatile uint32 *)0x1d30000)
#define WTDAT       (*(volatile uint32 *)0x1d30004)
#define WTCNT       (*(volatile uint32 *)0x1d30008)

/* Controlador de IIC */
#define IICCON      (*(volatile uint32 *)0x1d60000)
#define IICSTAT     (*(volatile uint32 *)0x1d60004)
#define IICADD      (*(volatile uint32 *)0x1d60008)
#define IICDS       (*(volatile uint32 *)0x1d6000c)

/* Controlador de IIS */
#define IISCON      (*(volatile uint32 *)0x1d18000)
#define IISMOD      (*(volatile uint32 *)0x1d18004)
#define IISPSR      (*(volatile uint32 *)0x1d18008)
#define IISFCON     (*(volatile uint32 *)0x1d1800c)
#define IISFIF      (*(volatile uint32 *)0x1d18010)

/* Controlador de SIO */
#define SIOCON      (*(volatile uint32 *)0x1d14000)
#define SIODAT      (*(volatile uint32 *)0x1d14004)
#define SBRDR       (*(volatile uint32 *)0x1d14008)
#define IVTCNT      (*(volatile uint32 *)0x1d1400c)
#define DCNTZ       (*(volatile uint32 *)0x1d14010)

#endif 

