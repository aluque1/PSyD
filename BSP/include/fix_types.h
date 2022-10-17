/*-------------------------------------------------------------------
**
**  Fichero:
**    fix_types.h  22/1/2016
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Declaración de tipos reales en punto fijo y macros para
**    operar aritmeticamente con ellos
**
**  Notas de diseño:
**    - Válidos para la arquitectura ARM7TDMI y el compilador GCC
**    - Según recomendaciones del Application Note ARM DAI 0033A
**
**-----------------------------------------------------------------*/

#ifndef __FIX_TYPES_H__
#define __FIX_TYPES_H__

#include <common_types.h>

/* Operaciones para argumentos/resultado del mismo tipo (anchura) y formato Q (número de bits decimales) */
#define FADD(a,b) ((a)+(b))
#define FSUB(a,b) ((a)-(b))
#define FMUL(a,b,q) (((a)*(b))>>(q))
#define FDIV(a,b,q) (((a)<<(q))/(b))

/* Operaciones con un argumento (izquierdo) entero */
#define FADDI(a,i,q) ((a)+((i)<<(q)))
#define FSUBI(a,i,q) ((a)-((i)<<(q)))
#define FMULI(a,i) ((a)*(i))
#define FDIVI(a,i) ((a)/(i))

/* Conversión a otro formato Q distinto */
#define FCONV(a, q1, q2) (((q2)>(q1)) ? (a)<<((q2)-(q1)) : (a)>>((q1)-(q2)))

/* Operaciones para argumentos/resultado del mismo tipo (anchura) pero con un formato Q (número de bits decimales) diferente */
#define FADDG(a,b,q1,q2,q3) (FCONV(a,q1,q3)+FCONV(b,q2,q3))
#define FSUBG(a,b,q1,q2,q3) (FCONV(a,q1,q3)-FCONV(b,q2,q3))
#define FMULG(a,b,q1,q2,q3) FCONV((a)*(b), (q1)+(q2), q3)
#define FDIVG(a,b,q1,q2,q3) (FCONV(a, q1, (q2)+(q3))/(b))

/* Conversión de/a punto flotante. Usar solo con argumentos constantes para evitar el enlace de funciones en punto flotante */
#define TOFIX(t, d, q) ((t)( (d)*(double)(1ULL<<(q)) ))
#define TOFLT(a, q) ( (double)(a) / (double)(1ULL<<(q)) )

typedef int8   fix8;
typedef int16  fix16;
typedef int32  fix32;
typedef int64  fix64;
typedef uint8  ufix8;
typedef uint16 ufix16;
typedef uint32 ufix32;
typedef uint64 ufix64;

#define MAX_FIX8  ((fix8)0x7f)
#define MIN_FIX8  ((fix8)0x80)

#define MAX_FIX16  ((fix16)0x7fff)
#define MIN_FIX16  ((fix16)0x8000)

#define MAX_FIX32  ((fix32)0x7fffffff)
#define MIN_FIX32  ((fix32)0x80000000)

#define MAX_FIX64  ((fix64)0x7fffffffffffffffLL)
#define MIN_FIX64  ((fix64)0x8000000000000000LL)

#define MAX_UFIX8  ((ufix8)0xff)
#define MIN_UFIX8  ((ufix8)0)

#define MAX_UFIX16  ((ufix16)0xffff)
#define MIN_UFIX16  ((ufix16)0)

#define MAX_UFIX32  ((ufix32)0xffffffff)
#define MIN_UFIX32  ((ufix32)0)

#define MAX_UFIX64  ((ufix64)0xffffffffffffffffULL)
#define MIN_UFIX64  ((ufix64)0)

#endif 
