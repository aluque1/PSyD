
#include <s3c44b0x.h>
#include <lcd.h>

extern uint8 font[];
uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;

void lcd_init( void )
{      
	DITHMODE = 0x12210;
	DP1_2    = 0xA5A5;
	DP4_7    = 0xBA5DA65;
	DP3_5    = 0xA5A5F;
	DP2_3    = 0xD6B;
	DP5_7    = 0xEB7B5ED;
	DP3_4    = 0x7DBE;
	DP4_5    = 0x7EBDF;
	DP6_7    = 0x7FDFBFE;

	REDLUT   = 0x0;
	GREENLUT = 0x0;
	BLUELUT  = 0x0;

	LCDCON1  = 0x1C020;
	LCDCON2  = 0x13CEF;
	LCDCON3  = 0x0;

	LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
	LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
	LCDSADDR3 = 0x50;

	lcd_off();
}

void lcd_on( void )
{
	LCDCON1 |= (1 << 0);
	state = LCDCON1;
}

void lcd_off( void )
{
	LCDCON1 &= ~(1 << 0);
	state = LCDCON1;
}

uint8 lcd_status( void )
{
	return (uint8)state;
}

void lcd_clear( void )
{
	for(int x = 0; x < 320; ++x){
		for(int y = 0; y < 240; ++y){
			lcd_putpixel(x, y, 0xff);
		}
	}
}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
	uint8 byte, bit;
	uint16 i;

	i = x/2 + y*(LCD_WIDTH/2);
	bit = (1-x%2)*4;

	byte = lcd_buffer[i];
	byte &= ~(0xF << bit);
	byte |= c << bit;
	lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y ) //Revisar
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (x%2)*4;

    byte = lcd_buffer[i] >> bit;
    byte &= ~(0xF0);

    return byte;
}

void lcd_draw_hrow( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{
	...
}

void lcd_draw_vrow( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{
	...
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
	...
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
	uint8 row, col;
	uint8 *bitmap;

	bitmap = font + ch*16;
	for( row=0; row<16; row++ )
		for( col=0; col<8; col++ )
			if( bitmap[row] & (0x80 >> col) )
				lcd_putpixel( x+col, y+row, color );
			else
				lcd_putpixel( x+col, y+row, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	...
}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
	...
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
{
	...
}

void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )
{
	...
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
	...
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
	...
}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
{
	...
}

void lcd_putWallpaper( uint8 *bmp )
{
	uint32 headerSize;

	uint16 x, ySrc, yDst;
	uint16 offsetSrc, offsetDst;

	headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

	bmp = bmp + headerSize;

	for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )
	{
		offsetDst = yDst*LCD_WIDTH/2;
		offsetSrc = ySrc*LCD_WIDTH/2;
		for( x=0; x<LCD_WIDTH/2; x++ )
			lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
	}
}
