/*
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

uint8 keypad_scan( void )
{
    uint8 aux;

    aux = *( KEYPAD_ADDR + 0x1c );
    if( (aux & 0x0f) != 0x0f )
    {
        if( (aux & 0x8) == 0 )
            return KEYPAD_KEY0;
        else if( (aux & 0x4) == 0 )
            return KEYPAD_KEY1;
        else if( (aux & 0x2) == 0 )
            return KEYPAD_KEY2;
        else if( (aux & 0x1) == 0 )
            return KEYPAD_KEY3;
    }
	aux = *( KEYPAD_ADDR + ... );
	if( (aux & 0x0f) != 0x0f )
	{
		if( (aux & 0x8) == 0 )
			return KEYPAD_KEY4;
		else if( (aux & 0x4) == 0 )
			return KEYPAD_KEY5;
		else if( (aux & 0x2) == 0 )
			return KEYPAD_KEY6;
		else if( (aux & 0x1) == 0 )
			return KEYPAD_KEY7;
	}
    ...

    return KEYPAD_FAILURE;
}

uint8 keypad_pressed( void )
{
    ...
}

void keypad_open( void (*isr)(void) )
{
    ...    
}

void keypad_close( void )
{
    ...    
}

#if KEYPAD_IO_METHOD == POOLING


void keypad_init( void )
{
    timers_init();  
};

uint8 keypad_getchar( void )
{
    ...    
}

uint8 keypad_getchartime( uint16 *ms )
{
    ...    
}

uint8 keypad_timeout_getchar( uint16 ms )
{
    ...    
}

#elif KEYPAD_IO_METHOD == INTERRUPT

static uint8 key = KEYPAD_FAILURE;

static void keypad_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void keypad_up_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_up_isr( void ) __attribute__ ((interrupt ("IRQ")));

void keypad_init( void )
{
    EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);	// Falling edge tiggered
    timers_init();
    keypad_open( keypad_down_isr );
};

uint8 keypad_getchar( void )
{
	uint8 scancode;

    while( key == KEYPAD_FAILURE );
    scancode = key;
    key = KEYPAD_FAILURE;
    return scancode;
}

static void keypad_down_isr( void )
{
	timer0_open_ms( timer0_down_isr, KEYPAD_KEYDOWN_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;
	I_ISPC	  = BIT_KEYPAD;
}

static void timer0_down_isr( void )
{
	key = keypad_scan();
	EXTINT = (EXTINT & ~(0xf<<4)) | (4<<4);
	keypad_open( keypad_up_isr );
	I_ISPC = BIT_TIMER0;
}

static void keypad_up_isr( void )
{
	timer0_open_ms( timer0_up_isr, KEYPAD_KEYUP_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;
	I_ISPC	  = BIT_KEYPAD;
}

static void timer0_up_isr( void )
{
	EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);
	keypad_open( keypad_down_isr );
	I_ISPC = BIT_TIMER0;
}

#else
	#error No se ha definido el metodo de E/S del keypad
#endif

*/
