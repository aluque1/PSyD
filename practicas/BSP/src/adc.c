#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <adc.h>

static uint8 state;

extern void isr_ADC_dummy( void );

void adc_init( void )
{
    ADCPSR = 19;
    adc_off();
}

void adc_on( void )
{
    ADCCON &= ~(1 << 5);
    sw_delay_ms( 10 );
    state = ON;
}

void adc_off( void )
{
    ADCCON |= (1 << 5);
    state = OFF;
}

uint8 adc_status( void )
{
    return state;
}

uint16 adc_getSample( uint8 ch )
{
    uint32 sample;
    uint8 i;
    
    ADCCON = (1 << 0) | (0 << 1) | (ch & 0x3) << 2;
    sw_delay_ms( 10 );
    for( i=0, sample=0; i<5; i++ )
    {
        ADCCON |= (1 << 0);   
        while(ADCCON & (1 << 0));
        while(!(ADCCON & (1 << 6)));
        sample += ADCDAT & 0x3ff;
    }
    return sample / 5;
}

void adc_open( void (*isr)(void) )
{
    pISR_ADC = (uint32)isr;
    I_ISPC   = BIT_ADC;
    INTMSK  &= ~(BIT_GLOBAL | BIT_ADC );
}

void adc_close( void )
{
    INTMSK  |= BIT_ADC;
    pISR_ADC = (uint32)isr_ADC_dummy;
}