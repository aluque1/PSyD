
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>
#include <uart.h>

extern void isr_TICK_dummy( void );

uint8 uint8_to_BCD(uint8 num);
uint8 BCD_to_uint8(uint8 num);

void rtc_init( void )
{
	TICNT   = 0x0;
	RTCALM  = 0x0;
	RTCRST  = 0x0;

	RTCCON  = 0x9;

	BCDYEAR = 0x25;
	BCDMON  = 0x5;
	BCDDAY  = 0x15;
	BCDDATE = 0x4;
	BCDHOUR = 0x19;
	BCDMIN  = 0x33;
	BCDSEC  = 0x12;

	ALMYEAR = 0x0;
	ALMMON  = 0x0;
	ALMDAY  = 0x0;
	ALMHOUR = 0x0;
	ALMMIN  = 0x0;
	ALMSEC  = 0x0;

	RTCCON &= 0;
}

void rtc_puttime( rtc_time_t *rtc_time )
{
	RTCCON |= 0x9;

	BCDYEAR = uint8_to_BCD(rtc_time->year);
	BCDMON  = uint8_to_BCD(rtc_time->mon);
	BCDDAY  = uint8_to_BCD(rtc_time->mday);
	BCDDATE = uint8_to_BCD(rtc_time->wday);
	BCDHOUR = uint8_to_BCD(rtc_time->hour);
	BCDMIN  = uint8_to_BCD(rtc_time->min);
	BCDSEC  = uint8_to_BCD(rtc_time->sec);

	RTCCON &= 0;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
	RTCCON |= 0x01;

	rtc_time->year = BCD_to_uint8(BCDYEAR);
	rtc_time->mon  = BCD_to_uint8(BCDMON);
	rtc_time->mday = BCD_to_uint8(BCDDAY);
	rtc_time->wday = BCD_to_uint8(BCDDATE);
	rtc_time->hour = BCD_to_uint8(BCDHOUR);
	rtc_time->min  = BCD_to_uint8(BCDMIN);
	rtc_time->sec  = BCD_to_uint8(BCDSEC);
	if( ! rtc_time->sec ){
		rtc_time->year = BCD_to_uint8(BCDYEAR);
		rtc_time->mon  = BCD_to_uint8(BCDMON);
		rtc_time->mday = BCD_to_uint8(BCDDAY);
		rtc_time->wday = BCD_to_uint8(BCDDATE);
		rtc_time->hour = BCD_to_uint8(BCDHOUR);
		rtc_time->min  = BCD_to_uint8(BCDMIN);
		rtc_time->sec  = BCD_to_uint8(BCDSEC);
	};

	RTCCON &= 0;
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
	pISR_TICK = (uint32) isr;
	I_ISPC    = 1;
	INTMSK   &= ~(1 << 20);
	TICNT     = 1 << 7 | tick_count;
}

void rtc_close( void )
{
	TICNT     = 0x7F;
	INTMSK   |= 1 << 20;
	pISR_TICK = (uint32) isr_TICK_dummy;
}

uint8 uint8_to_BCD(uint8 num){
	uint8 resul = 0, i = 0;
	while (num){
		resul |= ((num % 10) << (i * 4));
		num /= 10;
		++i;
	}
	return resul;
}

uint8 BCD_to_uint8(uint8 num){
	uint8 resul = 0, i = 0;
	while(i < sizeof(num) * 2){
		resul *= 10;
		resul += (num >> sizeof(num) * 4);
		num = num << 4;
		++i;
	}
	return resul;
}