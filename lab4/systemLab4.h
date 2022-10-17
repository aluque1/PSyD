/*-------------------------------------------------------------------
**
**  Fichero:
**    systemLab4.h  26/8/2016
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
**    Versión reducida orientada al desarrollo del lab4
**
**-----------------------------------------------------------------*/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

/*
**  Inicializa el sistema (versión lab4)
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
*/
void sys_init( void );

#endif
