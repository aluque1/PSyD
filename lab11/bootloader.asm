/*-----------------------------------------------------------------
**
**  Fichero:
**    bootloader.asm  26/3/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Bootloader elemental genérico para aplicaciones desarrolladas
**    para la placa de prototipado Embest S3CEV40
**
**  Notas de diseño:
**    - Asume ser una unica imagen plana ubicada al inicio de la ROM
**    - Replica un mapa de memoria similar al existente en proyectos
**      volcados en RAM (pilas y tabla virtual de vectores de excepción)
**    - Permite el portado directo de proyectos volcados en RAM
**    - Asume que las RTI están a menos de 32MB de distancia, en otro
**      caso reemplazar "B etq" por "LDR PC,=etq"
**
**---------------------------------------------------------------*/

.include "s3c44b0x.asm"
.include "s3cev40.asm"

/*
** Modos de operación
*/
.equ USRMODE, 0x10
.equ FIQMODE, 0x11
.equ IRQMODE, 0x12
.equ SVCMODE, 0x13
.equ ABTMODE, 0x17
.equ UNDMODE, 0x1b
.equ SYSMODE, 0x1f

/*
** Macro para la generación de un prologo para una RTI que, dado un puntero a su
** comienzo (entrada de la tabla virtual de vectores de excepcon), salte a la RTI
*/
.macro HANDLER pISR
  sub   sp,  sp, #4    /* deja un hueco en pila para almacenar la dirección de la RTI */
  stmfd sp!, {r0}      /* apila r0 (registro de trabajo) */
  ldr   r0,  =\pISR    /* carga en r0 la dirección de entrada de la tabla virtual de vectores indicada */
  ldr   r0,  [r0]      /* accediendo a dicha entrada carga en r0 la direccion de la RTI */
  str   r0,  [sp,#4]   /* almacena en pila la direccion de la RTI */
  ldmfd sp!, {r0, pc}  /* restaura el r0 y salta a la RTI */
.endm

.extern _image_start
.extern _image_end   

.extern main

.section .text

/*
** Tabla de vectores de excepción: saltos a los prologos de cada RTE
*/
  b bootloader         /* salta al comienzo del programa de arranque */
  b handlerUndef           
  b handlerSWI
  b handlerPabort
  b handlerDabort
  nop                  /* Reservado */
  b handlerIRQ
  b handlerFIQ

/*
** Tabla de vectores de IRQ: saltos a los prologos de cada RTI
*/
  b handlerUSB         /* EINT0 */
  b handlerKEYPAD      /* EINT1 */
  b handlerTS          /* EINT2 */
  b handlerETHERNET    /* EINT3 */
  b handlerPB          /* EINT4567 */
  b handlerTICK
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerZDMA0
  b handlerZDMA1
  b handlerBDMA0
  b handlerBDMA1
  b handlerWDT
  b handlerUERR01
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerTIMER0
  b handlerTIMER1
  b handlerTIMER2
  b handlerTIMER3
  b handlerTIMER4
  b handlerTIMER5
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerURXD0
  b handlerURXD1
  b handlerIIC
  b handlerSIO
  b handlerUTXD0
  b handlerUTXD1
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerRTC
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  nop                  /* Reservado */
  b handlerADC

/*
** Prologos de RTE
*/  
  handlerUndef:    HANDLER pISR_UNDEF
  handlerSWI:      HANDLER pISR_SWI
  handlerPabort:   HANDLER pISR_PABORT
  handlerDabort:   HANDLER pISR_DABORT
  handlerIRQ:      HANDLER pISR_IRQ
  handlerFIQ:      HANDLER pISR_FIQ

/*
** Prologos de RTI
*/
  handlerUSB:      HANDLER pISR_USB      /* EINT0 */
  handlerKEYPAD:   HANDLER pISR_KEYPAD   /* EINT1 */
  handlerTS:       HANDLER pISR_TS       /* EINT2 */
  handlerETHERNET: HANDLER pISR_ETHERNET /* EINT3 */
  handlerPB:       HANDLER pISR_PB       /* EINT4567 */
  handlerTICK:     HANDLER pISR_TICK
  handlerZDMA0:    HANDLER pISR_ZDMA0
  handlerZDMA1:    HANDLER pISR_ZDMA1
  handlerBDMA0:    HANDLER pISR_BDMA0
  handlerBDMA1:    HANDLER pISR_BDMA1
  handlerWDT:      HANDLER pISR_WDT
  handlerUERR01:   HANDLER pISR_UERR01
  handlerTIMER0:   HANDLER pISR_TIMER0
  handlerTIMER1:   HANDLER pISR_TIMER1
  handlerTIMER2:   HANDLER pISR_TIMER2
  handlerTIMER3:   HANDLER pISR_TIMER3
  handlerTIMER4:   HANDLER pISR_TIMER4
  handlerTIMER5:   HANDLER pISR_TIMER5
  handlerURXD0:    HANDLER pISR_URXD0
  handlerURXD1:    HANDLER pISR_URXD1
  handlerIIC:      HANDLER pISR_IIC
  handlerSIO:      HANDLER pISR_SIO
  handlerUTXD0:    HANDLER pISR_UTXD0
  handlerUTXD1:    HANDLER pISR_UTXD1
  handlerRTC:      HANDLER pISR_RTC
  handlerADC:      HANDLER pISR_ADC

/*
** Comienzo del bootloader:
**   La CPU está en estado ARM y en modo SVC
**   IRQ y FIQ están deshabilitadas
**   Todos los registros de los controladores de dispositivo están inicializados al valor indicado por el fabricante
**   El bootloader esta ejecutandose desde ROM a 8 MHz
*/  
bootloader:

  /* Deshabilita el watchdog */
  ldr r0, =WTCON        
  ldr r1, =0x0     
  str r1, [r0]

  /* Configuracion del gestor de reloj */
  ldr r0, =LOCKTIME   /*  estabilización del PLL = 512 us */
  ldr r1, =0xfff
  str r1, [r0]      
  ldr r0, =PLLCON     /* MCLK = 64 MHz */
  ldr r1, =0x38021  
  str r1, [r0]
  ldr r0, =CLKSLOW    /* MCLK_SLOW = 500 KHz */
  ldr r1, =0x8  
  str r1, [r0]  
  ldr r0, =CLKCON     /* modo NORMAL y reloj distribuido a todos los controladores */  
  ldr r1, =0x7ff8
  str r1, [r0]

  /* Configuracion del gestor de memoria */
  ldr   r0, =memconf
  ldr   r1, =_image_start
  sub   r0, r0, r1         /* corrige la diferencia existente entre la dirección en ROM y la direccion indicada en el linker script */
  ldmia r0, {r1-r13}       /* El controlador se programa usando ldmia/stmia según lo indicado por el fabricante */
  ldr   r0, =BWSCON
  stmia r0, {r1-r13}

  /* Copia en RAM la imagen del programa cargado en ROM a partir de la dirección indicada en el linker script */
  ldr   r0, =ROM_START_ADDRESS
  ldr   r1, =_image_start
  ldr   r2, =_image_end
remap_loop:
  cmp   r1, r2
  ldrcc r3, [r0], #4    /* Lee una palabra de la ROM (post-incrementando indice)    */
  strcc r3, [r1], #4    /* Escribe la palabra en la RAM (post-incrementando indice) */
  bcc   remap_loop

  ldr pc, =ram_exec     /* A partir de este punto el bootoloader pasa a ejecutarse desde RAM   */
                        /* La dirección absoluta a la que se corresponde la etiqueta apunta a RAM */

ram_exec:

  /* Inicializacion de los SP correspondiente a los modos de ejecución privilegiados */
  mrs r0, cpsr
  bic r0, r0, #0x1f

  orr r1, r0, #UNDMODE  /* desde modo SVC cambia a modo UND e inicializa el SP_und */
  msr cpsr_c, r1    
  ldr sp, =UNDSTACK

  orr r1, r0, #ABTMODE  /* desde modo UND cambia a modo ABT e inicializa el SP_abt */
  msr cpsr_c, r1 
  ldr sp, =ABTSTACK

  orr r1, r0, #IRQMODE  /* desde modo ABT cambia a modo IRQ e inicializa el SP_abt */
  msr cpsr_c, r1
  ldr sp, =IRQSTACK

  orr r1, r0, #FIQMODE  /* desde modo IRQ cambia a modo FIQ e inicializa el SP_fiq */
  msr cpsr_c, r1
  ldr sp, =FIQSTACK

  orr r1, r0, #SVCMODE  /* desde modo FIQ cambia a modo SVC e inicializa el SP_svc */
  msr cpsr_c, r1
  ldr sp, =SVCSTACK

  /* Habilita las líneas IRQ/FIQ permaneciendo en modo SVC */
  mrs  r0, cpsr          
  bic  r0, r0, #0xc0 
  msr  cpsr_c, r0
  
  /* El bootloader cede el control al programa principal en C */
  bl  main
  b   .                 /* si retorna de main, ejecuta un bucle infinito */

/*
** Constantes de configuracion del controlador de memoria
*/ 
memconf:
  .word 0x01000000 /* BWSCON   */
  .word 0x00000400 /* BANKCON0 */
  .word 0x00007ffc /* BANKCON1 */
  .word 0x00000000 /* BANKCON2 */
  .word 0x00007ffc /* BANKCON3 */
  .word 0x00000000 /* BANKCON4 */  
  .word 0x00000000 /* BANKCON5 */
  .word 0x00018000 /* BANKCON6 */  
  .word 0x00000000 /* BANKCON7 */
  .word 0x0086041b /* REFRESH  */
  .word 0x00000016 /* BANKSIZE */
  .word 0x00000020 /* MRSR6    */
  .word 0x00000000 /* MRSR7    */

.ltorg
  
.end
