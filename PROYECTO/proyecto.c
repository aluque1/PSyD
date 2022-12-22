
#include <s3c44b0x.h>
#include <common_types.h>
#include <system.h>
#include <segs.h>
#include <timers.h>
#include <lcd.h>

/* Número máximo de fotos distintas visualizables en el marco */

#define MAX_PHOTOS (40)

/* Direcciones en donde se encuentran cargados los BMPs tras la ejecución del comando "script load_bmp.script" */

#define ARBOL       ((uint8 *)0x0c210000)
#define PICACHU     ((uint8 *)0x0c220000)
#define MINIARBOL   ((uint8 *)0x0c280000)
#define MINIPICACHU ((uint8 *)0x0c288000)

/* Dimensiones de la pantalla para la realización de efectos */

#define LCD_COLS   (LCD_WIDTH/2)        // Para simplificar el procesamiento consideraremos una columna como la formada por 2 píxeles adyacentes (1 byte)
#define LCD_ROWS   (LCD_HEIGHT)

/* Sentidos de realización del efecto */

#define LEFT       (0)
#define RIGHT      (1)
#define UP         (2)
#define DOWN       (3)
#define NO_APPLY   (4)

/* Declaración de funciones auxiliares */

void lcd_putBmp( uint8 *bmp, uint16 x, uint16 y, uint16 xsize, uint16 ysize );
void lcd_bmp2photo( uint8 *bmp, uint8 *photo );
void test( uint8 *photo );                                      // Incluida para uso exclusivo en depuración, deberá eliminarse del proyecto final

void lcd_putColumn( uint16 xLcd, uint8 *photo, uint16 xPhoto );
void lcd_putRow( uint16 yLcd, uint8 *photo, uint16 yPhoto );    // Deberá recodificarse en el proyecto final
void lcd_putPhoto( uint8 *photo );                              // Deberá recodificarse en el proyecto final

void lcd_shift( uint8 sense );                                  // Deberá completarse y ampliarse en el proyecto final
void lcd_shift_noDMA( uint8 sense );                            // Incluida para uso exclusivo en la demo, deberá eliminarse del proyecto final

/* Declaración de efectos de transición entre fotos */

void efectoNulo( uint8 *photo, uint8 sense );
void efectoEmpuje( uint8 *photo, uint8 sense );                 // Deberá completarse en el proyecto final
void efectoEmpuje_noDMA( uint8 *photo, uint8 sense );           // Incluida para uso exclusivo en la demo, deberá eliminarse del proyecto final
void efectoBarrido( uint8 *photo, uint8 sense );                // Deberá implementarse en el proyecto final
void efectoRevelado( uint8 *photo, uint8 sense );               // Deberá implementarse en el proyecto final
void efectoCobertura( uint8 *photo, uint8 sense );              // Deberá implementarse en el proyecto final

// Otros posibles efectos a implementar

void efectoDivisionEntrante( uint8 *photo, uint8 sense );
void efectoDivisionSaliente( uint8 *photo, uint8 sense );
void efectoCuadradoEntrante( uint8 *photo, uint8 sense );
void efectoCuadradoSaliente( uint8 *photo, uint8 sense );
void efectoBarras( uint8 *photo, uint8 sense );
void efectoPeine( uint8 *photo, uint8 sense );
void efectoDisolver( uint8 *photo, uint8 sense );
void efectoFlash( uint8 *photo, uint8 sense );
void efectoAleatorio( uint8 *photo, uint8 sense );

/* Declaración de tipos */

typedef void (*pf_t)( uint8 *, uint8 );    // Tipo puntero a una función efecto con 2 argumentos (foto y sentido del efecto)

typedef struct                             // Estructura con toda la información (pack) relativa a la visualización de una foto, podrá ampliarse según convenga
{  
    uint8 photo[LCD_BUFFER_SIZE];          // Foto
    pf_t  effect;                          // Efecto de transición a aplicar para visualizarla
    uint8 sense;                           // Sentido del efecto a aplicar
    uint8 secs;                            // Segundos que debe estar visualizada
} pack_t;

typedef struct                             // Estructura conteniendo las fotos a visualizar, podrá ampliarse según convenga 
{
    uint8  numPacks;                       // Número de packs que contiene el album
    pack_t pack[MAX_PHOTOS];               // Array de fotos
} album_t;

/* Declaración del buffer de vídeo */

extern uint8 lcd_buffer[LCD_BUFFER_SIZE];

/*******************************************************************/

void main( void )
{
    album_t album;
    uint16 i;  
    
    sys_init();
    segs_init();
    timers_init();
    lcd_init();

    lcd_clear();
    lcd_on();
    
    // Ejemplo de uso de las miniaturas (de este o distinto tamaño)  
   
    lcd_putBmp( MINIARBOL, 20, 20, 128, 96 );       // ARBOL al 40% editada con Paint de Windows         
    lcd_putBmp( MINIPICACHU, 168, 20, 128, 96 );    // PICACHU al 40% editada con Paint de Windows

    lcd_puts( 20, 130, BLACK, "Se usaran miniaturas para" );
    lcd_puts( 20, 146, BLACK, "configurar la transicion de fotos" );
    sw_delay_s( 10 );
    
    // Creación del album de fotos
    
    i = 0;
        
    lcd_bmp2photo( ARBOL, album.pack[i].photo );
    album.pack[i].secs = 0;
    album.pack[i].effect = efectoNulo;
    album.pack[i].sense = NO_APPLY;
    i++;

    lcd_bmp2photo( PICACHU, album.pack[i].photo );
    album.pack[i].secs = 3;
    album.pack[i].effect = efectoEmpuje_noDMA;
    album.pack[i].sense = LEFT;
    i++;

    lcd_bmp2photo( ARBOL, album.pack[i].photo );
    album.pack[i].secs = 0;
    album.pack[i].effect = efectoNulo;
    album.pack[i].sense = NO_APPLY;
    i++;
    
    lcd_bmp2photo( PICACHU, album.pack[i].photo );
    album.pack[i].secs = 3;
    album.pack[i].effect = efectoEmpuje;
    album.pack[i].sense = LEFT;
    i++;

    album.numPacks = i;
  
    // Carrusel de fotos demostrando que solo los efectos por DMA hacen transiciones aceptables
    
    i = 0;
    while( 1 )    
    {   
        (*album.pack[i].effect)( album.pack[i].photo, album.pack[i].sense );    // Ejecuta el efecto para visualizar la nueva foto
        test( album.pack[i].photo );                                            // Chequea que el resultado del efecto es el deseado
        sw_delay_s( album.pack[i].secs );                                       // Mantiene la foto el tiempo indicado
        i = ( i==album.numPacks-1 ? 0 : i+1 );                                  // Avanza circularmente a la siguiente foto del album
    }
}


/*******************************************************************/

/*
** Muestra un BMP de tamaño (xsize, ysize) píxeles en la posición (x,y) del LCD
** Esta función es una generalización de lcd_putWallpaper() ya que:
**     lcd_putWallpaper( bmp ) = lcd_putBmp( bmp, 0, 0, LCD_WIDTH, LCD_HEIGHT )
**
** NO puede hacerse por DMA porque requiere la manipulación de pixeles
*/

void lcd_putBmp( uint8 *bmp, uint16 x, uint16 y, uint16 xsize, uint16 ysize )
{
	uint32 headerSize;

	uint16 xSrc, ySrc, yDst;
	uint16 offsetSrc, offsetDst;

	headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

	bmp = bmp + headerSize;

	for( ySrc=0, yDst=ysize-1; ySrc<ysize; ySrc++, yDst-- )
	{
		offsetDst = (yDst+y)*LCD_WIDTH/2+x/2;
		offsetSrc = ySrc*xsize/2;
		for( xSrc=0; xSrc<xsize/2; xSrc++ )
			lcd_buffer[offsetDst+xSrc] = ~bmp[offsetSrc+xSrc];
	}
}

/*
** Respecto al buffer de vídeo, el formato BMP tiene cabecera, las filas están volteadas y el color invertido, 
** Esta función convierte los BMP en un array de pixeles directamente visualizable (foto) para facilitar su manipulación
** Es una adaptación de lcd_putWallpaper() que en lugar de copiar el BMP sobre el buffer de vídeo lo hace sobre el array photo
**
** NO puede hacerse por DMA porque requiere la manipulación de pixeles.
*/

void lcd_bmp2photo( uint8 *bmp, uint8 *photo )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);    // Los datos de cabecera están en little-endian

    bmp = bmp + headerSize;                                                       // Salta cabecera

    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )             // Voltea verticalmente e invierte los pixels
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            photo[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}

/*
** Chequea que se está visualizando la foto indicada comparando pixel a pixel el buffer de vídeo y la foto
** Incluida para uso exclusivo en depuración, deberá eliminarse del proyecto final
**
** NO puede hacerse por DMA porque requiere la comparación de pixeles.
*/

void test( uint8 *photo )
{
    int16 x, y;    
    
    for( x=0; x<LCD_COLS; x++ )
        for( y=0; y<LCD_ROWS; y++ )
            if( lcd_buffer[y*LCD_COLS+x] != photo[y*LCD_COLS+x] )
            {
                segs_putchar( 0xE );
                while( 1 );
            }
}

/*
** Visualiza una columna de la foto en una columna dada de la pantalla
**
** NO puede hacerse por DMA porque los pixeles de una columna no ocupan posiciones contiguas de memoria
*/

void lcd_putColumn( uint16 xLcd, uint8 *photo, uint16 xPhoto )
{
    int16 y;

    for( y=0; y<LCD_ROWS; y++ )               
        lcd_buffer[(y*LCD_COLS)+xLcd] = photo[(y*LCD_COLS)+xPhoto];
}

/*
** Visualiza una linea de la foto en una linea dada de la pantalla
**
** Deberá recodificarse para que se realice mediante una única operación DMA ya que los pixeles de una linea son contiguos en memoria
*/

void lcd_putRow( uint16 yLcd, uint8 *photo, uint16 yPhoto )
{
    int16 x;

    for( x=0; x<LCD_COLS; x++ )
        lcd_buffer[(yLcd*LCD_COLS)+x] = photo[(yPhoto*LCD_COLS)+x];
}

/*
** Visualiza una foto en la pantalla
**
** Deberá recodificarse para que se realice mediante una única operación DMA ya que los pixeles de una imagen son contiguos en memoria
*/

void lcd_putPhoto( uint8 *photo )
{
    int16 x, y;
        
    for( y=0; y<LCD_ROWS; y++ )
        for( x=0; x<LCD_COLS; x++ )
            lcd_buffer[(y*LCD_COLS)+x] = photo[(y*LCD_COLS)+x];  
}

/*
** Scroll de una fila/columna por DMA
** Desplaza el contenido del LCD una linea en desplazamientos verticales, o una columna (formada por 2 pixeles adyacentes) en desplazamientos horizontales
**
** Deberá completarse y ampliarse para poder realizar otros efectos
*/

void lcd_shift( uint8 sense )
{
    int16 y;

    switch( sense )
    {
        case LEFT:                                                                              // Al ser un desplazamiento a izquierda, tener en cuenta que columnas consecutivas son contiguas en memoria pero los segmentos de fila no (excepto que sean filas completas y consecutivas)
            for( y=0; y<LCD_ROWS; y++ )                                                         // Recorre la pantalla por filas de arriba hacia abajo
            {
                ZDISRC0  = (0 << 30) | (1 << 28) | (uint32) (lcd_buffer + (y*LCD_COLS) + 1);    // datos de 8b, dirección POST-INCREMENTADA, origen: posición de la segunda columna de la fila correspondiente
                ZDIDES0  = (2 << 30) | (1 << 28) | (uint32) (lcd_buffer + (y*LCD_COLS));        // recomendada, dirección POST-INCREMENTADA, destino: posición de la primera columna de la fila correspondiente
                ZDICNT0  = (2 << 28) | (1 << 26) | (0 << 22) | (0 << 21) | (LCD_COLS-1);        // whole service, unit tranfer mode, pooling mode, no autoreload, tamaño: el de columnas menos una
                ZDICNT0 |= (1 << 20);                                                           // enable DMA (según manual debe hacerse en escritura separada a la escritura del resto de registros)
                ZDCON0   = 1;                                                                   // start DMA
                while( ZDCCNT0 & 0xFFFFF );                                                     // Espera a que la transferencia por DMA finalice
            }
            break;
        case RIGHT:                                                                             // Al ser un desplazamiento a izquierda, tener en cuenta que columnas consecutivas son contiguas en memoria pero los segmentos de fila no (excepto que sean filas completas y consecutivas)                                                                                            
            break;
        case UP:                                                                                // Al ser un desplazamiento hacia abajo, tener en cuenta que filas consecutivas son contiguas en memoria
            break;
        case DOWN:                                                                              // Al ser un desplazamiento hacia abajo, tener en cuenta que filas consecutivas son contiguas en memoria                                                                            
//          ZDISRC0  = ... ;                                                                    // datos de 8b, dirección POST-DECREMENTADA, origen: posición de la última columna de la penultima fila
//          ZDIDES0  = ... ;                                                                    // recomendada, dirección POST-DECREMENTADA, destino: posición de la última columna de la ultima fila
//          ZDICNT0  = ... ;                                                                    // whole service, unit tranfer mode, pooling mode, no autoreload, tamaño: el de columnas por el de filas menos una
//          ZDICNT0 |= ... ;                                                                    // enable DMA (según manual debe hacerse en escritura separada a la escritura del resto de registros)
//          ZDCON0   = ... ;                                                                    // start DMA
//          while( ... );                                                                       // Espera a que la transferencia por DMA finalice        
            break;
    }
}

/*
** Scroll de una fila/columna sin DMA
**
** Incluida para uso exclusivo en la demo, deberá eliminarse del proyecto final
*/

void lcd_shift_noDMA( uint8 sense )
{
    int16 x, y;

    switch( sense )
    {
        case LEFT:
            for( y=0; y<LCD_ROWS; y++ )
                for( x=0; x<LCD_COLS-1; x++ )
                    lcd_buffer[(y*LCD_COLS)+x] = lcd_buffer[(y*LCD_COLS)+(x+1)];
            break;
        case RIGHT:
            break;
        case UP:
            break;
        case DOWN:      
            break;
    }
}


/*******************************************************************/

/*
** Efecto nulo: Muestra la foto sin hacer efecto alguno
**
** Incluido por homogeneidad
*/

void efectoNulo( uint8 *photo, uint8 sense )
{
    switch( sense )
    {
        default:
            lcd_putPhoto( photo );
            break;        
    }     
}

/*
** Efecto empuje: La nueva imagen hace un scroll conjunto con la imagen mostrada
**
** Deberá completarse en el proyecto final
*/

void efectoEmpuje( uint8 *photo, uint8 sense )
{
    int16 x;

    switch( sense )
    {
        case LEFT:
            for( x=0; x<=LCD_COLS-1; x++ )                // Recorre la foto por columnas de izquierda a derecha
            {
                lcd_shift( LEFT );                        // Desplaza toda la pantalla una columna a la izquierda
                lcd_putColumn( LCD_COLS-1, photo, x );    // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
            }
            break;
        case RIGHT:
            break;
        case UP:
            break;
        case DOWN:
//          for( ... )    // Recorre la foto por filas de abajo hacia arriba
//          {
//              ...;                  // Desplaza la pantalla una fila hacia abajo
//              ...;                  // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
//              sw_delay_ms( 10 );    // Retarda porque el efecto por DMA es demasiado rápido
//          }
            break;        
    }  
}

/*
** Efecto empuje sin DMA 
**
** Incluido para uso exclusivo en la demo, deberá eliminarse del proyecto final
*/

void efectoEmpuje_noDMA( uint8 *photo, uint8 sense )
{
    int16 x;

    switch( sense )
    {
        case LEFT:
            for( x=0; x<=LCD_COLS-1; x++ )                // Recorre la foto por columnas de izquierda a derecha
            {
                lcd_shift_noDMA( LEFT );                  // Desplaza toda la pantalla una columna a la izquierda
                lcd_putColumn( LCD_COLS-1, photo, x );    // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
            }
            break;
        case RIGHT:
            break;
        case UP:
            break;
        case DOWN:
            break;        
    }  
}

/*
** Efecto barrido: La nueva imagen se superpone progresivamente sobre la imagen mostrada desde un lateral al opuesto 
*/

void efectoBarrido( uint8 *photo, uint8 sense )
{
    
}

/*
** Efecto revelado: La nueva imagen aparece conforme la imagen mostrada desaparece haciendo scroll
*/

void efectoRevelado( uint8 *photo, uint8 sense )
{
    
}

/*
** Efecto cobertura: La nueva imagen se superpone haciendo scroll sobre la imagen mostrada 
*/

void efectoCobertura( uint8 *photo, uint8 sense )
{
    
}






