
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
    while(((UFSTAT0 & (15 << 0)) == 0));
    return URXH0;
}

void uart0_puts( char *s )
{
	while(s[0] != '\0'){
		uart0_putchar(s++[0]);
	}
}

void uart0_gets( char *s )
{
	char aux;
	uint32 i = 1;
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
    	i--;
    	i = ~i;
    }

	do {
		*--p = i % 10 + '0';
		i /= 10;

	} while (i);

	if (negative) *--p = signo;
	uart0_puts( p );

}

void uart0_puthex( uint32 i )
{
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

    uart0_puts( p );
}


int32 uart0_getint( void )
{
	char buf[10 + 1];
	char *num = buf;
	int32 resul = 0;
	boolean negative = 0;

	uart0_gets(num);

	if(num[0] == '-'){
		negative = 1;
		++num;
	}

	while(num[0] != '\0'){
		resul *= 10;
		resul += (num++[0] - 48);
	}

	if(negative){
		resul = ~resul;
		resul++;
	}

    return resul;
}

uint32 uart0_gethex( void )
{
    char buf[8 + 1];
    char *num = buf;
	int32 resul = 0;

    uart0_gets(num);

    while(num[0] != '\0'){
    	resul *= 16;
    	if(num[0] > 70) num[0] -= 32;
    	if(num[0] > 57) num[0] -= 7;
    	resul += (num++[0] - 48);
    }

    return resul;
}
