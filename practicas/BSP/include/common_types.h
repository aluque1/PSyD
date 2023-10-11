/*-------------------------------------------------------------------
**
**  Fichero:
**    common_types.h  22/1/2016
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Declaración de tipos enteros
**
**  Notas de diseño:
**    Válidos para la arquitectura ARM7TDMI y el compilador GCC
**
**-----------------------------------------------------------------*/

#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

typedef unsigned char          boolean;    
typedef signed char            int8;    /*                       -128 ... +127                        */
typedef signed short int       int16;   /*                    -32.768 ... +32.767                     */
typedef signed int             int32;   /*             -2.147.483.648 ... +2.147.483.647              */
typedef signed long long int   int64;   /* -9.223.372.036.854.775.808 ... +9.223.372.036.854.775.807  */
typedef unsigned char          uint8;   /*                          0 ... 255                         */
typedef unsigned short int     uint16;  /*                          0 ... 65.535                      */
typedef unsigned int           uint32;  /*                          0 ... 4.294.967.295               */
typedef unsigned long long int uint64;  /*                          0 ... +18.446.744.073.709.551.615 */

#define MAX_INT8  ((int8)0x7f)
#define MIN_INT8  ((int8)0x80)

#define MAX_INT16  ((int16)0x7fff)
#define MIN_INT16  ((int16)0x8000)

#define MAX_INT32  ((int32)0x7fffffff)
#define MIN_INT32  ((int32)0x80000000)

#define MAX_INT64  ((int64)0x7fffffffffffffffLL)
#define MIN_INT64  ((int64)0x8000000000000000LL)

#define MAX_UINT8  ((uint8)0xff)
#define MIN_UINT8  ((uint8)0)

#define MAX_UINT16  ((uint16)0xffff)
#define MIN_UINT16  ((uint16)0)

#define MAX_UINT32  ((uint32)0xffffffff)
#define MIN_UINT32  ((uint32)0)

#define MAX_UINT64  ((uint64)0xffffffffffffffffULL)
#define MIN_UINT64  ((uint64)0)

#define NULL  ((void *) 0)

#define TRUE  (1)
#define FALSE (0)

#define ON    (1)
#define OFF   (0)

#endif 
