/*-------------------------------------------------------------------
**
**  Fichero:
**    kernelcoop.c  12/1/2021
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Kernel de planificación no expropiativa de tareas cooperativas
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/

#include <kernelcoop.h>
#include <s3c44b0x.h>

static task_t tasks[MAX_TASKS];   /* Lista ordenada de TCBs */

void delete_task( uint32 id )
{
    tasks[id].pfunction = NULL;
    tasks[id].period = 0; 
    tasks[id].ticks = 0;
    tasks[id].ready = FALSE;     
}

uint32 create_task( void (*pfunction)( void ), uint32 period )
{
    uint32 id;
    
    for( id=0; id<MAX_TASKS && tasks[id].pfunction ; id++ ); /* Busca la primera entrada libre (con función no NULL) de la lista de TCB */
    
    tasks[id].pfunction = pfunction;
    tasks[id].period = period; 
    tasks[id].ticks = 0;
    tasks[id].ready = FALSE;     
        
    (*pfunction)();     /* Asume que la primera llamada a la tarea la inicializa */

    return id;    
}

void scheduler_init( void )
{
    uint32 id;
    
    for( id=0; id<MAX_TASKS; id++ )
        delete_task( id );
};

void scheduler( void ) /* Esta funcion es la ISR del timer0 */
{
    uint32 id;
    
    I_ISPC = BIT_TIMER0;    /* Borra el bit de interrupción pendiente */

    for( id=0; id<MAX_TASKS; id++ )                     /* Recorre la lista completa de TCB */
        if( tasks[id].pfunction )
            if( ++tasks[id].ticks == tasks[id].period ) /* Si el contador de ticks equivale a su periodo de ejecución: */
            {
                tasks[id].ticks = 0;                    /* Inicializa a 0 su contador de ticks */
                tasks[id].ready = TRUE;                 /* Pasa la tarea a estado "preparado" */
            };
};
    
void dispacher( void )
{
    uint32 id;
    
    for( id=0; id<MAX_TASKS; id++ )                     /* Recorre la lista completa de TCB */
        if( tasks[id].ready == TRUE )                   /* Si la tarea está en estado "preparado": */
        {
            (*tasks[id].pfunction)();                   /* La ejecuta */
            tasks[id].ready = FALSE;                    /* Pasa la tarea a estado "no preparado" */
        }
};
