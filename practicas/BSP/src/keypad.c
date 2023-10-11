#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

uint8 keypad_scan( void )
{
    uint8 aux;
	uint8 i = 0;
	uint8 j = 0;
	uint8 keypadArr[16] = { KEYPAD_KEY0, KEYPAD_KEY1, KEYPAD_KEY2, KEYPAD_KEY3, KEYPAD_KEY4,
	KEYPAD_KEY5, KEYPAD_KEY6, KEYPAD_KEY7, KEYPAD_KEY8, KEYPAD_KEY9, KEYPAD_KEYA, KEYPAD_KEYB,
	KEYPAD_KEYC, KEYPAD_KEYD, KEYPAD_KEYE, KEYPAD_KEYF };

	while(i < 4)
	{
		aux = *(KEYPAD_ADDR + (0x1E ^ (1 << (i + 1))) );
		j = 0;
		if( (aux & 0x0f) != 0x0f )
		{
			while(aux & (0x8 >> j++));
			return keypadArr[(i*4) + (j - 1)];
		}
		++i;
	}
    return KEYPAD_FAILURE;
}

uint8 keypad_pressed( void )
{
	uint8 aux;
	uint8 i = 0;

	while(i < 4)
	{
		aux = *(KEYPAD_ADDR + (0x1E ^ (1 << (i++ + 1))) );
		if( (aux & 0x0f) != 0x0f )
			return TRUE;
	}
	return FALSE;
}

void keypad_open( void (*isr)(void) )
{
	pISR_KEYPAD	= (uint32)isr;
	I_ISPC		= BIT_KEYPAD;
	INTMSK 	   &= ~(BIT_GLOBAL | BIT_KEYPAD);
}

void keypad_close( void )
{
	INTMSK  |= BIT_KEYPAD;
	pISR_KEYPAD = (uint32)isr_KEYPAD_dummy;
}

#if KEYPAD_IO_METHOD == POOLING


void keypad_init( void )
{
    timers_init();
}

uint8 keypad_getchar( void )
{
	uint8 key;
	while( !keypad_pressed() );
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	key = keypad_scan();
	while( keypad_pressed() );
	sw_delay_ms( KEYPAD_KEYUP_DELAY );
	return key;
}

uint8 keypad_getchartime( uint16 *ms )
{
	uint8 scancode;

	while( !keypad_pressed() );
	timer3_start();
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );

	scancode = keypad_scan();

	while( keypad_pressed() );
	*ms = timer3_stop() / 10;
	sw_delay_ms( KEYPAD_KEYUP_DELAY );

	return scancode;
}

uint8 keypad_timeout_getchar( uint16 ms )
{
	uint8 scancode;

    timer3_start_timeout(ms * 10);
    while( !keypad_pressed() )
    {
        if( timer3_timeout() )
            return KEYPAD_TIMEOUT;
    }
    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
    scancode = keypad_scan();
    while( keypad_pressed() );
    sw_delay_ms( KEYPAD_KEYUP_DELAY );
    return scancode;
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


