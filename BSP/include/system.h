/*-------------------------------------------------------------------
**
**  Fichero:
**    system.h  23/3/2022
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de las funciones
**    para la inicialización del sistema
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <common_types.h>

#define NOP                                \
    asm volatile (                         \
        "nop"                              \
    )

#define SWI( num )                         \
    asm volatile (                         \
        "swi   %0                      "   \
        :                                  \
        : "i" (num)                        \
        :                                  \
    )

#define GET_SWI_NUMBER( num_p )            \
    asm volatile (                         \
        "ldr   r0, [lr, #-4]         \n"   \
        "bic   r0, r0, #0xff000000   \n"   \
        "str   r0, %0                  "   \
        : "=m" (*num_p)                    \
        :                                  \
        : "r0"                             \
    )

#define INT_DISABLE                        \
    asm volatile (                         \
        "mrs   r0, cpsr              \n"   \
        "stmfd sp!, {r0}             \n"   \
        "orr   r0, r0, #0b11000000   \n"   \
        "msr   cpsr_c, r0              "   \
        :                                  \
        :                                  \
        : "r0"                             \
    )
	
#define INT_ENABLE                         \
    asm volatile (                         \
        "ldmfd sp!, {r0}             \n"   \
        "msr   cpsr_c, r0              "   \
        :                                  \
        :                                  \
        : "r0"                             \
    )

#define IRQ_NESTING_ENABLE                 \
    asm volatile (                         \
        "mrs   lr, spsr              \n"   \
        "stmfd sp!, {lr}             \n"   \
        "msr   cpsr_c, #0b00011111   \n"   \
        "stmfd sp!, {lr}               "   \
    )

#define IRQ_NESTING_DISABLE                \
    asm volatile (                         \
        "ldmfd sp!, {lr}             \n"   \
        "msr   cpsr_c, #0b10010010   \n"   \
        "ldmfd sp!, {lr}             \n"   \
        "msr   spsr, lr                "   \
    )

#define LOCK_MUTEX( mutex_p )              \
    asm volatile (                         \
        "mov   r0, #1                \n"   \
        "swpb  r0, r0, [%0]          \n"   \
        "cmp   r0, #1                \n"   \
        "subeq pc, pc, #20             "   \
        :                                  \
        : "r" (mutex_p)                    \
        : "r0"                             \
    )

#define UNLOCK_MUTEX( mutex_p )            \
    asm volatile (                         \
        "mov   r0, #0               \n"    \
        "strb  r0, [%0]               "    \
        :                                  \
        : "r" (mutex_p)                    \
        : "r0"                             \
    )

#define MEMBAR                             \
    asm volatile (                         \
        ""                                 \
        :                                  \
        :                                  \
        : "memory"                         \
    )

/*
**  Inicializa el sistema
**    Deshabilita el watchdog
**    Enmascara todas las interrupciones
**    Configura el gestor de reloj
**      Tiempo de estabilización del PLL máximo
**      MCLK = 64 MHz
**      MCLK_SLOW = 500 KHz
**      Modo de funcionamiento normal
**      Reloj distribuido a todos los controladores
**    Arbitro de bus con prioridades por defecto
**    Cache deshabilitada
**    Configura los puertos de E/S
**    Configura el controlador de interrupciones
**      Fija prioridades por defecto
**      Todas las fuentes de interrupción en modo IRQ
**      Instala dummy ISR
**      Borra interrupciones pendientes externas e internas
**      IRQ vectorizadas, linea IRQ activada, linea FIQ desactivada
**  Inicializa el UART0
**  Muestra información del sistema por la UART0
*/
void sys_init( void );

/*
**  Pone el procesador en estado IDLE
*/
inline void sleep( void );

#endif
