#include <s3c44b0x.h>
#include <uart.h>

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
	while(UFSTAT0 & (15 << 0));
	return URXH0;
}

void uart0_puts( char *s )
{
	while(s[0] != '\0'){
		uart0_putchar(s[0]);
		s++;
	}
}

void uart0_gets( char *s )
{
	char aux;
	uint8 i = 1;
	while((aux = uart0_getchar()) != '\n'){
		s[0] = aux;
		s++;
		++i;
	}
	s[0] = '\0';
	s -= i;
}


void uart0_putint( int32 i )
{
    char buf[10 + 1];
    char *p = buf + 10;
    char signo;
    boolean negative = 0;

    *p = '\0';

    if (i < 0){
        signo = '-';
        negative = 1;
        i -= 1;
        i = ~i;
    }

    do {
        --p = i % 10 + '0';
        i /= 10;

    } while (i);

    if (negative)--p = signo;
    uart0_puts( p );

}

void uart0_puthex( uint32 i )
{
	char buf[8 + 1];
	char *p = buf + 8;
	uint8 c;

	*p = '\0';

	do {
		c = i & 0xf; // resto de la division por 16
		if( c < 10 )
			*--p = '0' + c;
		else
			*--p = 'a' + c - 10;
		i = i >> 4; // division por 16
	} while( i );

	uart0_puts( p );
}


int32 uart0_getint( void )
{
	return 10;
}

uint32 uart0_gethex( void )
{
	return 10;
}
