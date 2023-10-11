#-----------------------------------------------------------------
#
#  Fichero:
#    s3c44b0x.asm  24/4/2013
#
#    (c) J.M. Mendias
#    Programación de Sistemas y Dispositivos
#    Facultad de Informática. Universidad Complutense de Madrid
#
#  Propósito:
#    Creación de nemotécnicos para los registros de los
#    controladores de dispositivos internos del chip S3C44BOX
#
#  Notas de diseño:
#    Las direcciones usadas asumen un alineamiento little-endian
#
#-----------------------------------------------------------------

/* Controlador de memoria */
.equ BWSCON,    0x1c80000
.equ BANKCON0,  0x1c80004
.equ BANKCON1,  0x1c80008
.equ BANKCON2,  0x1c8000c
.equ BANKCON3,  0x1c80010
.equ BANKCON4,  0x1c80014
.equ BANKCON5,  0x1c80018
.equ BANKCON6,  0x1c8001c
.equ BANKCON7,  0x1c80020
.equ REFRESH,   0x1c80024
.equ BANKSIZE,  0x1c80028
.equ MRSRB6,    0x1c8002c
.equ MRSRB7,    0x1c80030

/* Gestor de reloj y energía */
.equ PLLCON,    0x1d80000
.equ CLKCON,    0x1d80004
.equ CLKSLOW,   0x1d80008
.equ LOCKTIME,  0x1d8000c

/* Controlador de cache */
.equ SYSCFG,    0x1c00000
.equ NCACHBE0,  0x1c00004
.equ NCACHBE1,  0x1c00008

/* Árbitro de bus */
.equ SBUSCON,   0x1c40000
 
/* Controlador de DMA */ 
.equ ZDCON0,    0x1e80000 /* canal ZDMA 0 */
.equ ZDISRC0,   0x1e80004
.equ ZDIDES0,   0x1e80008
.equ ZDICNT0,   0x1e8000c
.equ ZDCSRC0,   0x1e80010
.equ ZDCDES0,   0x1e80014
.equ ZDCCNT0,   0x1e80018
.equ ZDCON1,    0x1e80020 /* canal ZDMA 1 */
.equ ZDISRC1,   0x1e80024
.equ ZDIDES1,   0x1e80028
.equ ZDICNT1,   0x1e8002c
.equ ZDCSRC1,   0x1e80030
.equ ZDCDES1,   0x1e80034
.equ ZDCCNT1,   0x1e80038
.equ BDCON0,    0x1f80000 /* canal BDMA 0 */
.equ BDISRC0,   0x1f80004
.equ BDIDES0,   0x1f80008
.equ BDICNT0,   0x1f8000c
.equ BDCSRC0,   0x1f80010
.equ BDCDES0,   0x1f80014
.equ BDCCNT0,   0x1f80018
.equ BDCON1,    0x1f80020 /* canal BDMA 1 */
.equ BDISRC1,   0x1f80024
.equ BDIDES1,   0x1f80028
.equ BDICNT1,   0x1f8002c
.equ BDCSRC1,   0x1f80030
.equ BDCDES1,   0x1f80034
.equ BDCCNT1,   0x1f80038

/* Controlador de puertos de E/S */
.equ SPUCR,     0x1d2004c
.equ PCONA,     0x1d20000 /* puerto A */
.equ PDATA,     0x1d20004
.equ PCONB,     0x1d20008 /* puerto B */
.equ PDATB,     0x1d2000c
.equ PCONC,     0x1d20010 /* puerto C */
.equ PDATC,     0x1d20014
.equ PUPC,      0x1d20018
.equ PCOND,     0x1d2001c /* puerto D */
.equ PDATD,     0x1d20020
.equ PUPD,      0x1d20024
.equ PCONE,     0x1d20028 /* puerto E */
.equ PDATE,     0x1d2002c
.equ PUPE,      0x1d20030
.equ PCONF,     0x1d20034 /* puerto F */
.equ PDATF,     0x1d20038
.equ PUPF,      0x1d2003c
.equ PCONG,     0x1d20040 /* puerto G */
.equ PDATG,     0x1d20044
.equ PUPG,      0x1d20048
.equ EXTINT,    0x1d20050
.equ EXTINTPND, 0x1d20054
 
/* Bits de EXTINTPND */
.equ BIT_EINT4,  (1<<0)
.equ BIT_EINT5,  (1<<1)
.equ BIT_EINT6,  (1<<2)
.equ BIT_EINT7,  (1<<3)

/* Temporizadores */
.equ TCFG0,     0x1d50000 
.equ TCFG1,     0x1d50004
.equ TCON,      0x1d50008
.equ TCNTB0,    0x1d5000c /* temporizador 0 */
.equ TCMPB0,    0x1d50010
.equ TCNTO0,    0x1d50014
.equ TCNTB1,    0x1d50018 /* temporizador 1 */
.equ TCMPB1,    0x1d5001c
.equ TCNTO1,    0x1d50020
.equ TCNTB2,    0x1d50024 /* temporizador 2 */
.equ TCMPB2,    0x1d50028
.equ TCNTO2,    0x1d5002c
.equ TCNTB3,    0x1d50030 /* temporizador 3 */
.equ TCMPB3,    0x1d50034
.equ TCNTO3,    0x1d50038
.equ TCNTB4,    0x1d5003c /* temporizador 4 */
.equ TCMPB4,    0x1d50040
.equ TCNTO4,    0x1d50044
.equ TCNTB5,    0x1d50048 /* temporizador 5 */
.equ TCNTO5,    0x1d5004c
 
/* UART */
.equ ULCON0,    0x1d00000 /* canal 0 */
.equ UCON0,     0x1d00004
.equ UFCON0,    0x1d00008
.equ UMCON0,    0x1d0000c
.equ UTRSTAT0,  0x1d00010
.equ UERSTAT0,  0x1d00014
.equ UFSTAT0,   0x1d00018
.equ UMSTAT0,   0x1d0001c
.equ UTXH0,     0x1d00020
.equ URXH0,     0x1d00024
.equ UBRDIV0,   0x1d00028
.equ ULCON1,    0x1d04000 /* canal 1 */
.equ UCON1,     0x1d04004
.equ UFCON1,    0x1d04008
.equ UMCON1,    0x1d0400c
.equ UTRSTAT1,  0x1d04010
.equ UERSTAT1,  0x1d04014
.equ UFSTAT1,   0x1d04018
.equ UMSTAT1,   0x1d0401c
.equ UTXH1,     0x1d04020
.equ URXH1,     0x1d04024
.equ UBRDIV1,   0x1d04028

/* Controlador de interrupciones */
.equ INTCON,    0x1e00000
.equ INTPND,    0x1e00004
.equ INTMOD,    0x1e00008
.equ INTMSK,    0x1e0000c
.equ I_PSLV,    0x1e00010
.equ I_PMST,    0x1e00014
.equ I_CSLV,    0x1e00018
.equ I_CMST,    0x1e0001c
.equ I_ISPR,    0x1e00020
.equ I_ISPC,    0x1e00024
.equ F_ISPC,    0x1e0003c

/* Bits de INTMASK, I_ISPC/F_ISPC y I_ISPR */
.equ BIT_ADC,    (1<<0)
.equ BIT_RTC,    (1<<1)
.equ BIT_UTXD1,  (1<<2)
.equ BIT_UTXD0,  (1<<3)
.equ BIT_SIO,    (1<<4)
.equ BIT_IIC,    (1<<5)
.equ BIT_URXD1,  (1<<6)
.equ BIT_URXD0,  (1<<7)
.equ BIT_TIMER5, (1<<8)
.equ BIT_TIMER4, (1<<9)
.equ BIT_TIMER3, (1<<10)
.equ BIT_TIMER2, (1<<11)
.equ BIT_TIMER1, (1<<12)
.equ BIT_TIMER0, (1<<13)
.equ BIT_UERR01, (1<<14)
.equ BIT_WDT,    (1<<15)
.equ BIT_BDMA1,  (1<<16)
.equ BIT_BDMA0,  (1<<17)
.equ BIT_ZDMA1,  (1<<18)
.equ BIT_ZDMA0,  (1<<19)
.equ BIT_TICK,   (1<<20)
.equ BIT_EINT4567, (1<<21)
.equ BIT_EINT3,  (1<<22)
.equ BIT_EINT2,  (1<<23)
.equ BIT_EINT1,  (1<<24)
.equ BIT_EINT0,  (1<<25)
.equ BIT_GLOBAL, (1<<26)

/* Controlador de LCD */
.equ LCDCON1,   0x1f00000
.equ LCDCON2,   0x1f00004
.equ LCDCON3,   0x1f00040
.equ LCDSADDR1, 0x1f00008
.equ LCDSADDR2, 0x1f0000c
.equ LCDSADDR3, 0x1f00010
.equ REDLUT,    0x1f00014
.equ GREENLUT,  0x1f00018
.equ BLUELUT,   0x1f0001c
.equ DP1_2,     0x1f00020
.equ DP4_7,     0x1f00024
.equ DP3_5,     0x1f00028
.equ DP2_3,     0x1f0002c
.equ DP5_7,     0x1f00030
.equ DP3_4,     0x1f00034
.equ DP4_5,     0x1f00038
.equ DP6_7,     0x1f0003c
.equ DITHMODE,  0x1f00044

/* Conversor analógico digital */
.equ ADCCON,    0x1d40000
.equ ADCPSR,    0x1d40004
.equ ADCDAT,    0x1d40008

/* Reloj de tiempo real */
.equ RTCCON,    0x1d70040
.equ RTCALM,    0x1d70050
.equ ALMSEC,    0x1d70054
.equ ALMMIN,    0x1d70058
.equ ALMHOUR,   0x1d7005c
.equ ALMDAY,    0x1d70060
.equ ALMMON,    0x1d70064
.equ ALMYEAR,   0x1d70068
.equ RTCRST,    0x1d7006c
.equ BCDSEC,    0x1d70070
.equ BCDMIN,    0x1d70074
.equ BCDHOUR,   0x1d70078
.equ BCDDAY,    0x1d7007c
.equ BCDDATE,   0x1d70080
.equ BCDMON,    0x1d70084
.equ BCDYEAR,   0x1d70088
.equ TICNT,     0x1d7008c

/* Watchdog timer */
.equ WTCON,     0x1d30000
.equ WTDAT,     0x1d30004
.equ WTCNT,     0x1d30008

/* Controlador de IIC */
.equ IICCON,    0x1d60000
.equ IICSTAT,   0x1d60004
.equ IICADD,    0x1d60008
.equ IICDS,     0x1d6000c

/* Controlador de IIS */
.equ IISCON,    0x1d18000
.equ IISMOD,    0x1d18004
.equ IISPSR,    0x1d18008
.equ IISFCON,   0x1d1800c
.equ IISFIF,    0x1d18010

/* Controlador de SIO */
.equ SIOCON,    0x1d14000
.equ SIODAT,    0x1d14004
.equ SBRDR,     0x1d14008
.equ IVTCNT,    0x1d1400c
.equ DCNTZ,     0x1d14010
