/*
 * leds.c
 *
 *  Created on: Sep 26, 2022
 *      Author: Alejandro Luque Villegas, Ignacio Sanchez Santatecla
 */

#include <s3c44b0x.h>
#include <s3cev40.h>
#include <leds.h>

static uint32 state;

void leds_init( void ){
	PCONB &= ~((1 << 10) | (1 << 9));  // PB[10] = out, PF[9] = out
	state = ~PDATB & ((1 << 10) | (1 << 9));
}

void led_on( uint8 led ){
	PDATB &= ~(led << 9);
	state = ~PDATB & ((1 << 10) | (1 << 9));
}

void led_off( uint8 led ){
	PDATB |= (led << 9);
	state = ~PDATB & ((1 << 10) | (1 << 9));
}

void led_toggle( uint8 led ){
	PDATB ^= (led << 9);
	state = ~PDATB & ((1 << 10) | (1 << 9));
}

uint8 led_status( uint8 led ){
	return (uint8)((state & (led << 9)) >> (8 + led));
}

