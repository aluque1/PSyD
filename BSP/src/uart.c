
#include <s3c44b0x.h>
#include <uart.h>
#include <common_functions.h>

void uart0_init( void )
{
	UFCON0 = 0x1;
	UMCON0 = 0x0;
	ULCON0 = 0x3;
	UBRDIV0 = 0x22;
	UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
	while((UFSTAT0 & (1 << 9)));
	UTXH0 = ch;
}        

char uart0_getchar( void )
{
	while(!(UFSTAT0 & 0xf));
	return URXH0;
}

void uart0_puts( char *s )
{
	while(*s != '\0'){
		uart0_putchar(*s++);
	}
}

void uart0_gets( char *s )
{
	char aux;
	uint32 i = 0;
	while((aux = uart0_getchar()) != '\n'){
		s[i] = aux;
		++i;
	}
	s[i] = '\0';
}


void uart0_putint( int32 i )
{
	uart0_puts( int32ToString(i) );
}

void uart0_puthex( uint32 i )
{
	uart0_puts( hexToString(i) );
}


int32 uart0_getint( void )
{
	char buf[10 + 1];
	char *num = buf;

	uart0_gets(num);

	return stringToInt32(num);
}

uint32 uart0_gethex( void )
{
	char buf[8 + 1];
	char *num = buf;

	uart0_gets(num);

	return stringToHex(num);
}
