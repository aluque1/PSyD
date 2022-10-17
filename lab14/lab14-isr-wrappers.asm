/*-----------------------------------------------------------------
**
**  Fichero:
**    lab14-isr-wrappers.asm  24/1/2014
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Implementa wrappers para cada una de las RTI de la aplicación
**    ejecutada bajo el kernel de tiempo real uC/OS-II 
**
**  Notas de diseño:
**    - El wrapper para timer0 está implementado en os_cpu_a.asm
**
**---------------------------------------------------------------*/

.include "../os_port/os_cpu_isr_wrapper.asm"

  /* Referencias externas */

  .extern isr_pb         /* void isr_pb( void ) -- RTI por presión de un pushbutton */
  
  /* Funciones definidas en este módudo */

  .global OS_CPU_isr_pb       /* void OS_CPU_isr_pb( void ) */
  
.section .text

/*
** void OS_CPU_isr_pb( void )
**
** Wrapper + isr_pb
**
*/

OS_CPU_isr_pb:
  OS_CPU_ISR_WRAPPER isr_pb

