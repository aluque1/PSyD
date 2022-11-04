/*
 * common_functions.c
 *
 *  Created on: 30/10/2022
 *      Author: Alejandro Luque Villegas, Ignacio Sanchez Santatecla
 */

#include <common_functions.h>

char* int32ToString(int32 i){

	char buf[10 + 1];
	char *p = buf + 10;
	char signo;
	boolean negative = 0;

	*p = '\0';

	if (i < 0){
		signo = '-';
		negative = 1;
		i--;
		i = ~i;
	}

	do {
		*--p = i % 10 + '0';
		i /= 10;

	} while (i);

	if (negative) *--p = signo;

	return p;
}

int32 stringToInt32(char* num){
	int32 resul = 0;
	boolean negative = 0;

	if(*num == '-'){
		negative = 1;
		++num;
	}

	while(*num != '\0'){
		resul *= 10;
		resul += ((*num++) - 48);
	}

	if(negative){
		resul = ~resul;
		++resul;
	}

	return resul;
}

char* hexToString(int32 i){
	char buf[8 + 1];
	char *p = buf + 8;

	uint8 c;


	*p = '\0';

	do {
		c = i & 0xf;
		if( c < 10 )
			*--p = '0' + c;
		else
			*--p = 'a' + c - 10;
		i = i >> 4;
	} while( i );
	return p;
}

int32 stringToHex(char* num){
	int32 resul = 0;

	while(*num != '\0'){
		resul *= 16;
		if(*num > 70) *num -= 32;
		if(*num > 57) *num -= 7;
		resul += ((*num++) - 48);
	}

	return resul;
}

