/*
 * common_functions.h
 *
 *  Created on: 30/10/2022
 *      Author: nacho
 */

#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_

#include <common_types.h>

//Convierte un entero de 32bits a una cadena de caracteres en ascii.
char* int32ToString(int32 i);

//Convierte una cadena de caracteres en un entero de 32bits
int32 stringToInt32(char* num);

//Convierte un entero de 32bits a una cadena de caracteres en hexadecimal.
char* hexToString(int32 i);

//Convierte una cadena de caracteres escritos en hexadecimal a un entero de 32bits
int32 stringToHex(char* num);

#endif /* COMMON_FUNCTIONS_H_ */
