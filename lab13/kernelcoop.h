/*-------------------------------------------------------------------
**
**  Fichero:
**    cooperativo.c  12/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de las macros, tipos y prototipos 
**    de las funciones del kernel de planificación no expropiativa
**    de tareas cooperativas multiestado
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#include <common_types.h>

#define MAX_TASKS        (10)    /* Configuración del kernel */
#define TICKS_PER_SEC   (100)

typedef struct                   /* Definición del TCB (task control block) */
{
    void (*pfunction) (void);    /* Función que realiza la tarea */
    uint32 period;               /* Tiempo mínimo que debe transcurrir entre 2 ejecuciones consecutivas de la tarea (en ticks) */
    uint32 ticks;                /* Tiempo transcurrido desde la última ejecución de la tarea (en ticks) */
    boolean ready;               /* Estado de la tarea: ready = TRUE indica que ha transcurrido un periodo de desde su última ejecución */
} task_t;

uint32 create_task( void (*pfunction)(void), uint32 period );
void delete_task( uint32 i );

void scheduler_init( void );
void scheduler( void ) __attribute__ ((interrupt ("IRQ")));
void dispacher( void );
