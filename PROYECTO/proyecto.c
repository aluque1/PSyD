
#include <s3c44b0x.h>
#include <common_types.h>
#include <system.h>
#include <segs.h>
#include <timers.h>
#include <lcd.h>

/* N�mero m�ximo de fotos distintas visualizables en el marco */

#define MAX_PHOTOS (40)

/* Direcciones en donde se encuentran cargados los BMPs tras la ejecuci�n del comando "script load_bmp.script" */

#define ARBOL       ((uint8 *)0x0c210000)
#define PICACHU     ((uint8 *)0x0c220000)
#define MINIARBOL   ((uint8 *)0x0c280000)
#define MINIPICACHU ((uint8 *)0x0c288000)

/* Dimensiones de la pantalla para la realizaci�n de efectos */

#define LCD_COLS        (LCD_WIDTH / 2) // Para simplificar el procesamiento consideraremos una columna como la formada por 2 p�xeles adyacentes (1 byte)
#define LCD_ROWS        (LCD_HEIGHT)
#define MIN_HEIGHT      (96)
#define MIN_WIDTH       (128)
#define MIN_COLS        (MIN_WIDTH / 2)
#define MIN_ROWS        (MIN_HEIGHT)
#define MINIATRURE_SIZE (MIN_COLS * MIN_ROWS) // Tamaño de la miniatura en bytes

/* Sentidos de realizaci�n del efecto */

#define LEFT        (0)
#define RIGHT       (1)
#define UP          (2)
#define DOWN        (3)
#define NO_APPLY    (4)

/* Modos de transferencia DMA */

#define SRC_INCR    (0x01)
#define DES_INCR    (0x04)
#define SRC_DEC     (0x02)
#define DES_DEC     (0x08)

/* Declaraci�n de funciones auxiliares */
void lcd_dumpBmp(uint8 *bmp, uint8 *buffer, uint16 x, uint16 y, uint16 xsize, uint16 ysize);
void lcd_putBmp(uint8 *bmp, uint16 x, uint16 y, uint16 xsize, uint16 ysize);
void lcd_bmp2photo(uint8 *bmp, uint8 *photo);
void lcd_bmp2min(uint8 *bmp, uint8 *min);
void test(uint8 *photo); // Incluida para uso exclusivo en depuraci�n, deber� eliminarse del proyecto final


void lcd_putColumn(uint16 xLcd, uint8 *photo, uint16 xPhoto);
void lcd_putRow(uint16 yLcd, uint8 *photo, uint16 yPhoto); // Deber� recodificarse en el proyecto final
void lcd_putPhoto(uint8 *photo);                           // Deber� recodificarse en el proyecto final
void lcd_putMiniaturePhoto(uint8 *photo, uint16 x, uint16 y);
void zDMA_transfer(uint8 *src, uint8 *dst, uint32 size, uint8 mode);

void lcd_shift(uint8 sense);       // Deber� completarse y ampliarse en el proyecto final
void lcd_shift_noDMA(uint8 sense); // Incluida para uso exclusivo en la demo, deber� eliminarse del proyecto final


/* Declaraci�n de efectos de transici�n entre fotos */
void efectoNulo(uint8 *photo, uint8 sense);
void efectoEmpuje(uint8 *photo, uint8 sense);       // Deber� completarse en el proyecto final
void efectoEmpuje_noDMA(uint8 *photo, uint8 sense); // Incluida para uso exclusivo en la demo, deber� eliminarse del proyecto final
void efectoBarrido(uint8 *photo, uint8 sense);      // Deber� implementarse en el proyecto final
void efectoRevelado(uint8 *photo, uint8 sense);     // Deber� implementarse en el proyecto final
void efectoCobertura(uint8 *photo, uint8 sense);    // Deber� implementarse en el proyecto final


// Otros posibles efectos a implementar
void efectoDivisionEntrante(uint8 *photo, uint8 sense);
void efectoDivisionSaliente(uint8 *photo, uint8 sense);
void efectoCuadradoEntrante(uint8 *photo, uint8 sense);
void efectoCuadradoSaliente(uint8 *photo, uint8 sense);
void efectoBarras(uint8 *photo, uint8 sense);
void efectoPeine(uint8 *photo, uint8 sense);
void efectoDisolver(uint8 *photo, uint8 sense);
void efectoFlash(uint8 *photo, uint8 sense);
void efectoAleatorio(uint8 *photo, uint8 sense);


/* Declaraci�n de tipos */
typedef void (*pf_t)(uint8 *, uint8); // Tipo puntero a una funci�n efecto con 2 argumentos (foto y sentido del efecto)

typedef struct
{
    uint8 photoBuffer[LCD_BUFFER_SIZE];
    uint8 min[MINIATRURE_SIZE];
} photo;

typedef struct // Estructura con toda la informaci�n (pack) relativa a la visualizaci�n de una foto, podr� ampliarse seg�n convenga
{
    photo data;  // Foto
    pf_t effect; // Efecto de transici�n a aplicar para visualizarla
    uint8 sense; // Sentido del efecto a aplicar
    uint8 secs;  // Segundos que debe estar visualizada
} pack_t;

typedef struct // Estructura conteniendo las fotos a visualizar, podr� ampliarse seg�n convenga
{
    uint8 numPacks;          // N�mero de packs que contiene el album
    pack_t pack[MAX_PHOTOS]; // Array de fotos
} album_t;

/* Declaraci�n del buffer de v�deo */

extern uint8 lcd_buffer[LCD_BUFFER_SIZE];

/*******************************************************************/

void main(void)
{
    album_t album;
    uint16 i;

    sys_init();
    segs_init();
    timers_init();
    lcd_init();

    lcd_clear();
    lcd_on();

    // Ejemplo de uso de las miniaturas (de este o distinto tama�o)

    lcd_bmp2min(MINIARBOL, album.pack[0].data.min);   // ARBOL al 40%
    lcd_bmp2min(MINIPICACHU, album.pack[1].data.min); // PICACHU al 40%

    lcd_putMiniaturePhoto(album.pack[0].data.min, 20, 20); // ARBOL al 40%
    lcd_putMiniaturePhoto(album.pack[1].data.min, 20, 20); // PICACHU al 40%

    lcd_clear();

    lcd_putBmp(MINIARBOL, 20, 20, 128, 96);    // ARBOL al 40% editada con Paint de Windows
    lcd_putBmp(MINIPICACHU, 168, 20, 128, 96); // PICACHU al 40% editada con Paint de Windows

    lcd_puts(20, 130, BLACK, "Se usaran miniaturas para configurar la transicion de fotos");
    sw_delay_s(4);

    // Creaci�n del album de fotos

    i = 0;

    lcd_bmp2photo(ARBOL, album.pack[i].data.photoBuffer);
    album.pack[i].secs = 0;
    album.pack[i].effect = efectoBarrido;
    album.pack[i].sense = LEFT;
    i++;

    lcd_bmp2photo(PICACHU, album.pack[i].data.photoBuffer);
    album.pack[i].secs = 1;
    album.pack[i].effect = efectoBarrido;
    album.pack[i].sense = RIGHT;
    i++;

    lcd_bmp2photo(ARBOL, album.pack[i].data.photoBuffer);
    album.pack[i].secs = 1;
    album.pack[i].effect = efectoBarrido;
    album.pack[i].sense = UP;
    i++;

    lcd_bmp2photo(PICACHU, album.pack[i].data.photoBuffer);
    album.pack[i].secs = 1;
    album.pack[i].effect = efectoBarrido;
    album.pack[i].sense = DOWN;
    i++;

    album.numPacks = i;

    // Carrusel de fotos demostrando que solo los efectos por DMA hacen transiciones aceptables

    i = 0;
    while (1)
    {
        (*album.pack[i].effect)(album.pack[i].data.photoBuffer, album.pack[i].sense); // Ejecuta el efecto para visualizar la nueva foto
        test(album.pack[i].data.photoBuffer);                                         // Chequea que el resultado del efecto es el deseado
        sw_delay_s(album.pack[i].secs);                                               // Mantiene la foto el tiempo indicado
        i = (i == album.numPacks - 1 ? 0 : i + 1);                                    // Avanza circularmente a la siguiente foto del album
    }
}

/*******************************************************************/

/*
** Muestra un BMP de tama�o (xsize, ysize) p�xeles en la posici�n (x,y) del LCD
** Esta funci�n es una generalizaci�n de lcd_putWallpaper() ya que:
**     lcd_putWallpaper( bmp ) = lcd_putBmp( bmp, 0, 0, LCD_WIDTH, LCD_HEIGHT )
**
** NO puede hacerse por DMA porque requiere la manipulaci�n de pixeles
*/
void lcd_putBmp(uint8 *bmp, uint16 x, uint16 y, uint16 xsize, uint16 ysize)
{
    lcd_dumpBmp(bmp, lcd_buffer, x, y, xsize, ysize);
}

/*
** Respecto al buffer de v�deo, el formato BMP tiene cabecera, las filas est�n volteadas y el color invertido,
** Esta funci�n convierte los BMP en un array de pixeles directamente visualizable (foto) para facilitar su manipulaci�n
** Es una adaptaci�n de lcd_putWallpaper() que en lugar de copiar el BMP sobre el buffer de v�deo lo hace sobre el array photo
**
** NO puede hacerse por DMA porque requiere la manipulaci�n de pixeles.
*/
void lcd_bmp2photo(uint8 *bmp, uint8 *photo)
{
    lcd_dumpBmp(bmp, photo, 0, 0, LCD_WIDTH, LCD_HEIGHT);
}

void lcd_dumpBmp(uint8 *bmp, uint8 *buffer, uint16 x, uint16 y, uint16 xsize, uint16 ysize)
{
    uint32 headerSize;

    uint16 xSrc, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;

    for (ySrc = 0, yDst = ysize - 1; ySrc < ysize; ySrc++, yDst--)
    {
        offsetDst = (yDst + y) * LCD_WIDTH / 2 + x / 2;
        offsetSrc = ySrc * xsize / 2;
        for (xSrc = 0; xSrc < xsize / 2; xSrc++)
            buffer[offsetDst + xSrc] = ~bmp[offsetSrc + xSrc];
    }
}

void lcd_bmp2min(uint8 *bmp, uint8 *min)
{
    uint32 headerSize;

    uint16 xSrc, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;

    for (ySrc = 0, yDst = MIN_HEIGHT - 1; ySrc < MIN_HEIGHT; ySrc++, yDst--)
    {
        offsetDst = yDst * MIN_WIDTH / 2;
        offsetSrc = ySrc * MIN_WIDTH / 2;
        for (xSrc = 0; xSrc < MIN_WIDTH / 2; xSrc++)
            min[offsetDst + xSrc] = ~bmp[offsetSrc + xSrc];
    }
}

/*
** Chequea que se est� visualizando la foto indicada comparando pixel a pixel el buffer de v�deo y la foto
** Incluida para uso exclusivo en depuraci�n, deber� eliminarse del proyecto final
**
** NO puede hacerse por DMA porque requiere la comparaci�n de pixeles.
*/
void test(uint8 *photo)
{
    int16 x, y;

    for (x = 0; x < LCD_COLS; x++)
        for (y = 0; y < LCD_ROWS; y++)
            if (lcd_buffer[y * LCD_COLS + x] != photo[y * LCD_COLS + x])
            {
                segs_putchar(0xE);
                while (1)
                    ;
            }
}

/*
** Visualiza una columna de la foto en una columna dada de la pantalla
**
** NO puede hacerse por DMA porque los pixeles de una columna no ocupan posiciones contiguas de memoria
*/
void lcd_putColumn(uint16 xLcd, uint8 *photo, uint16 xPhoto)
{
    int16 y;

    for (y = 0; y < LCD_ROWS; y++)
        lcd_buffer[(y * LCD_COLS) + xLcd] = photo[(y * LCD_COLS) + xPhoto];
}

/*
** Visualiza una linea de la foto en una linea dada de la pantalla
*/
void lcd_putRow(uint16 yLcd, uint8 *photo, uint16 yPhoto)
{
    zDMA_transfer(photo + (yPhoto * LCD_COLS), lcd_buffer + (yLcd * LCD_COLS), LCD_COLS, SRC_INCR| DES_INCR);
}

/*
** Visualiza una foto en la pantalla
*/
void lcd_putPhoto(uint8 *photo)
{
    zDMA_transfer(photo, lcd_buffer, LCD_BUFFER_SIZE, SRC_INCR | DES_INCR);
}

/*
** Visualiza una miniatura en la pantalla
*/
void lcd_putMiniaturePhoto(uint8 *min, uint16 x, uint16 y)
{
    uint16 i, auxX = x / 2, auxY = y;

    for(i = 0; i < MIN_ROWS; i++)
        zDMA_transfer(min + (i * MIN_COLS), lcd_buffer + ((auxY + i) * LCD_COLS) + auxX, MIN_COLS, SRC_INCR | DES_INCR);
}

/*
** Scroll de una fila/columna por DMA
** Desplaza el contenido del LCD una linea en desplazamientos verticales, o una columna (formada por 2 pixeles adyacentes) en desplazamientos horizontales
*/
void lcd_shift(uint8 sense)
{
    int16 y;

    switch (sense)
    {
    case LEFT:                         // Al ser un desplazamiento a izquierda, tener en cuenta que columnas consecutivas son contiguas en memoria pero los segmentos de fila no (excepto que sean filas completas y consecutivas)
        for (y = 0; y < LCD_ROWS; y++) // Recorre la pantalla por filas de arriba hacia abajo
        {
            zDMA_transfer(lcd_buffer + (y * LCD_COLS) + 1, lcd_buffer + (y * LCD_COLS), LCD_COLS - 1, SRC_INCR | DES_INCR); // Desplaza la fila una columna a la izquierda
        }
        break;
    case RIGHT:
        for (y = 0; y < LCD_ROWS; y++) // Recorre la pantalla por filas de arriba hacia abajo
        {
            zDMA_transfer(lcd_buffer + (y * LCD_COLS) + (LCD_COLS - 2), lcd_buffer + (y * LCD_COLS) + (LCD_COLS - 1), LCD_COLS - 1, SRC_DEC | DES_DEC); // Desplaza la fila una columna a la derecha
        }
        break;
    case UP:
            zDMA_transfer(lcd_buffer + LCD_COLS, lcd_buffer, LCD_BUFFER_SIZE - LCD_COLS, SRC_INCR | DES_INCR); // Desplaza una fila hacia arriba
        break;
    case DOWN:
            zDMA_transfer(lcd_buffer + (LCD_BUFFER_SIZE - LCD_COLS - 1), lcd_buffer + LCD_BUFFER_SIZE - 1, LCD_BUFFER_SIZE - LCD_COLS, SRC_DEC | DES_DEC); // Desplaza una fila hacia abajo
        break;
    }
}

/*
* Función de transferencia de datos por DMA
* @param src: dirección de origen
* @param dst: dirección de destino
* @param size: tamaño de la transferencia
* @param mode: modo de transferencia (dirección post incrementada o post decrementada)
*/
void zDMA_transfer(uint8 *src, uint8 *dst, uint32 size, uint8 mode)
{
    ZDISRC0 = (0 << 30) | ((mode & 0x3) << 28)        | (uint32)src;                     // datos de 8b
    ZDIDES0 = (2 << 30) | (((mode & 0xC) >> 2) << 28) | (uint32)dst;                     // recomendada
    ZDICNT0 = (2 << 28) | (1 << 26) | (0 << 22) | (0 << 21) | (size & 0xFFFFF); // whole service, unit tranfer mode, pooling mode, no autoreload, size
    ZDICNT0 |= (1 << 20);                                                       // enable DMA (seg�n manual debe hacerse en escritura separada a la escritura del resto de registros)
    ZDCON0 = 1;                                                                 // start DMA
    while (ZDCCNT0 & 0xFFFFF);                                                  // Espera a que la transferencia por DMA finalice
}


/*******************************************************************/

/*
** Efecto nulo: Muestra la foto sin hacer efecto alguno
**
** Incluido por homogeneidad
*/
void efectoNulo(uint8 *photo, uint8 sense)
{
    switch (sense)
    {
    default:
        lcd_putPhoto(photo);
        break;
    }
}

/*
** Efecto empuje: La nueva imagen hace un scroll conjunto con la imagen mostrada
*/
void efectoEmpuje(uint8 *photo, uint8 sense)
{
    int16 x;

    switch (sense)
    {
    case LEFT:
        for (x = 0; x <= LCD_COLS - 1; x++) // Recorre la foto por columnas de izquierda a derecha
        {
            lcd_shift(LEFT);                       // Desplaza toda la pantalla una columna a la izquierda
            lcd_putColumn(LCD_COLS - 1, photo, x); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(RIGHT);                      // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(0, photo, x);            // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (x = 0; x <= LCD_ROWS - 1; x++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(UP);                         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(LCD_ROWS - 1, photo, x);    // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (x = LCD_ROWS - 1; x >= 0; x--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(DOWN);                        // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(0, photo, x);                // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
        }
        break;
    }
}

/*
** Efecto barrido: La nueva imagen se superpone progresivamente sobre la imagen mostrada desde un lateral al opuesto
*/
void efectoBarrido(uint8 *photo, uint8 sense)
{
    int16 x, y;

    switch (sense)
    {
    case LEFT:
        for (x = 0; x <= LCD_COLS - 1; x++) // Recorre la foto por columnas de izquierda a derecha
        {
            lcd_putColumn(x, photo, x); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_putColumn(x, photo, x); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case UP:
        for (y = 0; y <= LCD_ROWS - 1; y++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_putRow(y, photo, y); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
            sw_delay_ms(6);        // Espera un tiempo para que se vea el efecto
        }
        break;
    case DOWN:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_putRow(y, photo, y); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
            sw_delay_ms(6);        // Espera un tiempo para que se vea el efecto
        }
        break;
    }
}

/*
** Efecto revelado: La nueva imagen aparece conforme la imagen mostrada desaparece haciendo scroll
*/
void efectoRevelado(uint8 *photo, uint8 sense)
{
}

/*
** Efecto cobertura: La nueva imagen se superpone haciendo scroll sobre la imagen mostrada
*/
void efectoCobertura(uint8 *photo, uint8 sense)
{
}
