#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <common_functions.h>
#include <system.h>
#include <segs.h>
#include <timers.h>
#include <lcd.h>
#include <uda1341ts.h>
#include <iis.h>
#include <dma.h>
#include <pbs.h>
#include <keypad.h>
#include <ts.h>

/* N�mero m�ximo de fotos distintas visualizables en el marco */

#define MAX_PHOTOS (40)

/* Direcciones en donde se encuentran cargados los BMPs tras la ejecuci�n del comando "script load_bmp.script" */

#define ARBOL       ((uint8 *)0x0c210000)
#define PICACHU     ((uint8 *)0x0c220000)
#define PULP        ((uint8 *)0x0c230000)
#define HARRY       ((uint8 *)0x0c240000)

#define ROSALINA    ((int16 *)0x0c703fff)

#define ROSALINA_SIZE           (1028174)

#define MINIARBOL   ((uint8 *)0x0c610000)
#define MINIPICACHU ((uint8 *)0x0c612000)
#define MINIPULP    ((uint8 *)0x0c614000)
#define MINIHARRY   ((uint8 *)0x0c616000)

/* Dimensiones de la pantalla para la realizaci�n de efectos */

#define LCD_COLS        (LCD_WIDTH / 2) // Para simplificar el procesamiento consideraremos una columna como la formada por 2 p�xeles adyacentes (1 byte)
#define LCD_ROWS        (LCD_HEIGHT)
#define MIN_HEIGHT      (108)
#define MIN_WIDTH       (144)
#define MIN_COLS        (MIN_WIDTH / 2)
#define MIN_ROWS        (MIN_HEIGHT)
#define MINIATRURE_SIZE (MIN_COLS * MIN_ROWS) // Tamaño de la miniatura en bytes

#define MIN_LEFT        (0)
#define MIN_RIGHT       (1)

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
    uint16 index;            // Pack actual
    pack_t pack[MAX_PHOTOS]; // Array de fotos
} album_t;



/* Declaraci�n de recursos */

extern uint8 lcd_buffer[LCD_BUFFER_SIZE]; // Buffer de v�deo

uint8 scancode; // Variable para almacenar el c�digo de tecla pulsada
uint8 volumen; // Variable para almacenar el volumen de reproducci�n
uint16 xTs; // Variables para almacenar las coordenadas del TS
uint16 yTs; // Variables para almacenar las coordenadas del TS

volatile boolean flagPb; // Flag para indicar que se ha pulsado el pulsador
volatile boolean flagKeyPad; // Flag para indicar que se ha pulsado una tecla del teclado
volatile boolean flagTs; // Flag para indicar que se ha pulsado el TS
volatile boolean flagExit; 


/* Declaraci�n de RTI */

void isr_pb( void )     __attribute__ ((interrupt ("IRQ")));
void isr_keyPad( void )    __attribute__ ((interrupt ("IRQ")));
void isr_ts( void )     __attribute__ ((interrupt ("IRQ")));

/* Declaraci�n de funciones auxiliares */
void lcd_dumpBmp(uint8 *bmp, uint8 *buffer, uint16 x, uint16 y, uint16 xsize, uint16 ysize);
void lcd_putBmp(uint8 *bmp, uint16 x, uint16 y, uint16 xsize, uint16 ysize);
void lcd_bmp2photo(uint8 *bmp, uint8 *photo);
void lcd_bmp2min(uint8 *bmp, uint8 *min);
void test(uint8 *photo); // Incluida para uso exclusivo en depuraci�n, deber� eliminarse del proyecto final
void photoSlider();
void menuPrincipal();
void menuSettings(uint8 index);
void menuPausa();
void menuImagen(uint8 index);

void lcd_shift(uint8 sense, uint16 initRow, uint16 initCol, uint16 endRow, uint16 endCol);
void lcd_putColumn(uint16 xLcd, uint8 *photo, uint16 xPhoto, uint16 yLcdUp, uint16 yLcdDown, uint16 yPhotoUp);
void lcd_putRow(uint16 yLcd, uint8 *photo, uint16 yPhoto, uint16 xLcdLeft, uint16 xLcdRight, uint16 xPhotoLeft);
void lcd_putPhoto(uint8 *photo);
void lcd_putMiniaturePhoto(uint8 *min, uint8 pos);
void zDMA_transfer(uint8 *src, uint8 *dst, uint32 size, uint8 mode);
void lcd_clearDMA();
void lcd_restore();
void lcd_backUp();
void keypad_action();


/* Declaraci�n de efectos de transici�n entre fotos */
void efectoNulo(uint8 *photo, uint8 sense);
void efectoEmpuje(uint8 *photo, uint8 sense);
void efectoBarrido(uint8 *photo, uint8 sense);
void efectoRevelado(uint8 *photo, uint8 sense);
void efectoCobertura(uint8 *photo, uint8 sense);


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

// variables globales
album_t album;
pf_t effectArray[] = {*efectoNulo, *efectoEmpuje, *efectoBarrido, *efectoRevelado, *efectoCobertura}; // Array de puntero a funcion de ejecto
uint8 *photoArray[] = {ARBOL, PICACHU, PULP, HARRY}; // Array de punteros a las fotos a visualizar
uint8 *minArray[] = {MINIARBOL, MINIPICACHU, MINIPULP, MINIHARRY}; // Array de punteros a las miniaturas de las fotos a visualizar
uint8 bkUpBuffer[LCD_BUFFER_SIZE];
const uint8 numPhotos = 4; // N�mero de fotos a visualizar

/*******************************************************************/

void main(void)
{
    sys_init();
    pbs_init();
    segs_init();
    timers_init();
    keypad_init();
    uda1341ts_init();
    iis_init(IIS_DMA);
    pbs_open(isr_pb);
    keypad_open(isr_keyPad);
    ts_open(isr_ts);
    ts_init();
    lcd_on();

    flagPb = FALSE;
    flagKeyPad = FALSE;
    flagTs = FALSE;
    flagExit = FALSE;

    uint8 i;
    for (i = 0; i < numPhotos; i++)
    {
        lcd_bmp2min(minArray[i], album.pack[i].data.min);
        lcd_bmp2photo(photoArray[i], album.pack[i].data.photoBuffer);
    }
    album.numPacks = numPhotos;
    album.index = 0;

    menuPrincipal();
    while (1)
    {
        if(flagPb)
        {
            flagPb = FALSE;
            menuPausa();
        }
        if(flagExit)
        {
            flagExit = FALSE;
            menuPrincipal();
        }
        photoSlider();
    }
}

void photoSlider()
{
    (*album.pack[album.index].effect)(album.pack[album.index].data.photoBuffer, album.pack[album.index].sense); // Ejecuta el efecto para visualizar la nueva foto
    test(album.pack[album.index].data.photoBuffer);                                   // Chequea que el resultado del efecto es el deseado
    timer3_delay_s(album.pack[album.index].secs);         // Mantiene la foto el tiempo indicado
    ++album.index; album.index %= album.numPacks;                                      // Avanza circularmente a la siguiente foto del album
}

void menuPrincipal()
{
    uint16 i;
    uint8 index = 0;
    
    lcd_clearDMA();
    lcd_puts(19, 0, BLACK, "Configura las fotos a visualizar:");
    lcd_draw_box(3, 18, LCD_WIDTH - 5, 19 + 31, BLACK, 2);
    lcd_puts(LCD_WIDTH/2 - 8, 29, BLACK, "UP");

    lcd_draw_box(3, 203, LCD_WIDTH - 5, LCD_HEIGHT - 5, BLACK, 2);
    lcd_puts(LCD_WIDTH/2 - 16, 211, BLACK, "DOWN");

    while (!flagPb)
    { 
        for (i = 0; i < 2; i++)
        lcd_putMiniaturePhoto(album.pack[index++].data.min, i);
        index -= 2;

        while (!flagTs && !flagPb);

        if (flagTs)
        {
            ts_getpos(&xTs, &yTs);
            flagTs = FALSE;
            if (yTs >= 18 && yTs <= 19 + 32) // "boton  up"
            {
                index -= (index == 0) ? 0 : 2;
            }
            else if (yTs >= 203 && yTs <= LCD_HEIGHT - 5) // "boton down"
            {
                index += 2; index %= album.numPacks;
            }
            else if (yTs >= 75 && yTs < 203)
            {
                lcd_backUp();
                menuSettings(index + (xTs > LCD_WIDTH/2));
                lcd_restore();
            }
        } 
    }
    flagPb = FALSE;
    

    uda1341ts_setvol(VOL_MED);
    iis_play(ROSALINA, ROSALINA_SIZE, TRUE);

    sw_delay_s(1);
}

void menuPausa()
{

}


/**
 *  _________________________________________________
 * |               configura la foto                |
 * |                                                |
 * |  _________________         _______________     |
 * | |                |        |___SEGUNDOS___|     |   (MIN_WIDTH + 20, 65), (MIN_WIDTH + 160, 85)
 * | |                |                             |
 * | |                |         _______________     |
 * | |      PHOTO     |        |____EFECTO____|     |   (MIN_WIDTH + 20, 115), (MIN_WIDTH + 160, 135)      
 * | |                |                             |
 * | |                |         _______________     |               
 * | |________________|        |____SENTIDO___|     |   (MIN_WIDTH + 20, 165), (MIN_WIDTH + 160, 185)
 * |                                                |
 * |  modo :         use el teclado para seleccionar|                                                   
 * |________________________________________________|
 * 
*/

void menuSettings(uint8 index)
{ 
    lcd_clearDMA();
    lcd_puts(19, 0, BLACK, "Configura la foto:");

    lcd_putMiniaturePhoto(album.pack[index].data.min, 0);
    
    lcd_draw_box((MIN_WIDTH + 20), 65, (MIN_WIDTH + 160), 85, BLACK, 2);
    lcd_puts(174, 68, BLACK, "SEGUNDOS: " );

    lcd_draw_box((MIN_WIDTH + 20), 115, (MIN_WIDTH + 160), 135, BLACK, 2);
    lcd_puts(174, 118, BLACK, "EFECTOS: " );

    lcd_draw_box((MIN_WIDTH + 20), 165, (MIN_WIDTH + 160), 185, BLACK, 2);
    lcd_puts(174, 168, BLACK, "SENTIDO: " );
    
    while (!flagPb)
    { 
        while (!flagTs && !flagPb);

        if (flagTs)
        {
            ts_getpos(&xTs, &yTs); // TODO maybe streamline with a case ????
            flagTs = FALSE;
            if((yTs > 65) && (yTs < 85) && (xTs > 164) && (xTs < 304)) // "boton de segundos"
            {
                lcd_puts(10, (LCD_HEIGHT - 15), BLACK, "CAMBIANDO : SEGUNDOS");
                lcd_puts(((LCD_WIDTH/2) + 25), (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                keypad_action();
                album.pack[index].secs = scancode;
                lcd_putint(270, 68, BLACK, scancode);
            }
            else if((yTs > 115) && (yTs < 135) && (xTs > 164) && (xTs < 304)) // "boton de efectos"
            {
                lcd_puts(10, (LCD_HEIGHT - 15), BLACK, "CAMBIANDO : EFFECTO");
                lcd_puts(((LCD_WIDTH/2) + 25), (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                keypad_action();
                album.pack[index].effect = effectArray[scancode];
                lcd_putint(270, 118, BLACK, scancode);
            }
            else if((yTs > 165) && (yTs < 185) && (xTs > 164) && (xTs < 304)) // "boton de sentido"
            {
                lcd_puts(10, (LCD_HEIGHT - 15), BLACK, "CAMBIANDO : SENTIDO");
                lcd_puts(((LCD_WIDTH/2) + 25), (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                keypad_action();
                album.pack[index].sense = scancode;
                lcd_putint(270, 168, BLACK, scancode);
            }
        }
    }
    flagPb = FALSE;
}

void keypad_action(){
    flagKeyPad = FALSE;
    uint16 ticks;
    ticks = 0;
    while( ticks != 5 )
    {
        while( !flagKeyPad );
        flagKeyPad = FALSE;
        scancode = keypad_getchar();
        ticks++;
    }
}

void menuImagen(uint8 index)
{
    lcd_clearDMA();

    index = 0;
    lcd_bmp2photo(ARBOL, album.pack[index].data.photoBuffer);
    album.pack[index].secs = 0;
    album.pack[index].effect = efectoCobertura;
    album.pack[index].sense = LEFT;
    index++;

    lcd_bmp2photo(PICACHU, album.pack[index].data.photoBuffer);
    album.pack[index].secs = 1;
    album.pack[index].effect = efectoCobertura;
    album.pack[index].sense = RIGHT;
    index++;

    lcd_bmp2photo(PULP, album.pack[index].data.photoBuffer);
    album.pack[index].secs = 1;
    album.pack[index].effect = efectoCobertura;
    album.pack[index].sense = UP;
    index++;

    lcd_bmp2photo(HARRY, album.pack[index].data.photoBuffer);
    album.pack[index].secs = 1;
    album.pack[index].effect = efectoCobertura;
    album.pack[index].sense = DOWN;
    index++;
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
void lcd_putColumn(uint16 xLcd, uint8 *photo, uint16 xPhoto, uint16 yLcdUp, uint16 yLcdDown, uint16 yPhotoUp)
{
    for (; yLcdUp <= yLcdDown; yLcdUp++, yPhotoUp++)
        lcd_buffer[(yLcdUp * LCD_COLS) + xLcd] = photo[(yPhotoUp * LCD_COLS) + xPhoto];
}

/*
** Visualiza una linea de la foto en una linea dada de la pantalla
*/
void lcd_putRow(uint16 yLcd, uint8 *photo, uint16 yPhoto, uint16 xLcdLeft, uint16 xLcdRight, uint16 xPhotoLeft)
{
    zDMA_transfer(photo + (yPhoto * LCD_COLS) + xLcdLeft, lcd_buffer + (yLcd * LCD_COLS) + xPhotoLeft, xLcdRight - xLcdLeft, SRC_INCR| DES_INCR);
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
void lcd_putMiniaturePhoto(uint8 *min, uint8 pos)
{
    uint16 i, x = 8, y = 75;
    x += (pos % 2) ? MIN_WIDTH + 8 : 0;
    x = x >> 1;

    for(i = 0; i < MIN_ROWS; i++)
        zDMA_transfer(min + (i * MIN_COLS), lcd_buffer + ((y + i) * LCD_COLS) + x, MIN_COLS, SRC_INCR | DES_INCR);
}

/* 
Copia el contenido del LCD en un buffer de backup 
*/
void lcd_backUp()
{
    zDMA_transfer(lcd_buffer, bkUpBuffer, LCD_BUFFER_SIZE, SRC_INCR | DES_INCR);
}

/*
* Restaura el contenido del LCD a partir del buffer de backup
*/
void lcd_restore()
{
    zDMA_transfer(bkUpBuffer, lcd_buffer, LCD_BUFFER_SIZE, SRC_INCR | DES_INCR);
}

void lcd_clearDMA()
{
    uint32 clear = WHITE;
    ZDISRC0 = (2 << 30) | (3 << 28) | (uint32)&clear;                            // datos de 8b
    ZDIDES0 = (2 << 30) | (1 << 28) | (uint32)lcd_buffer;                       // recomendada
    ZDICNT0 = (2 << 28) | (1 << 26) | (0 << 22) | (0 << 21) | (LCD_BUFFER_SIZE & 0xFFFFF); // whole service, unit tranfer mode, pooling mode, no autoreload, size
    ZDICNT0 |= (1 << 20);                                                       // enable DMA (seg�n manual debe hacerse en escritura separada a la escritura del resto de registros)
    ZDCON0 = 1;                                                                 // start DMA
    while (ZDCCNT0 & 0xFFFFF);
}

/*
** Scroll de una fila/columna por DMA
** Desplaza el contenido del LCD una linea en desplazamientos verticales, o una columna (formada por 2 pixeles adyacentes) en desplazamientos horizontales
*/
void lcd_shift(uint8 sense, uint16 initRow, uint16 initCol, uint16 endRow, uint16 endCol)
{
    int16 y;

    switch (sense)
    {
    case LEFT:                         // Al ser un desplazamiento a izquierda, tener en cuenta que columnas consecutivas son contiguas en memoria pero los segmentos de fila no (excepto que sean filas completas y consecutivas)
        for (y = initRow; y <= endRow; y++) // Recorre la pantalla por filas de arriba hacia abajo
        {
            zDMA_transfer(lcd_buffer + (y * LCD_COLS) + endCol + 1, lcd_buffer + (y * LCD_COLS) + endCol, initCol - endCol, SRC_INCR | DES_INCR); // Desplaza la fila una columna a la izquierda
        }
        break;
    case RIGHT:
        for (y = initRow; y <= endRow; y++) // Recorre la pantalla por filas de arriba hacia abajo
        {
            zDMA_transfer(lcd_buffer + (y * LCD_COLS) + (endCol - 1), lcd_buffer + (y * LCD_COLS) + (endCol), endCol - initCol, SRC_DEC | DES_DEC); // Desplaza la fila una columna a la derecha
        }
        break;
    case UP:
            zDMA_transfer(lcd_buffer + (LCD_COLS * (endRow + 1)), lcd_buffer + (LCD_COLS * endRow), (initRow - endRow) * LCD_COLS, SRC_INCR | DES_INCR); // Desplaza una fila hacia arriba
        break;
    case DOWN:
            zDMA_transfer(lcd_buffer + (LCD_COLS * endRow) - 1, lcd_buffer + (LCD_COLS * (endRow + 1) - 1), (endRow - initRow) * LCD_COLS, SRC_DEC | DES_DEC); // Desplaza una fila hacia abajo
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
            lcd_shift(LEFT, 0, LCD_COLS - 1, LCD_ROWS - 1, 0);                       // Desplaza toda la pantalla una columna a la izquierda
            lcd_putColumn(LCD_COLS - 1, photo, x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(RIGHT, 0, 0, LCD_ROWS - 1, LCD_COLS - 1);                      // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(0, photo, x, 0, 239, 0);            // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (x = 0; x <= LCD_ROWS - 1; x++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(UP, LCD_ROWS - 1, 0, 0, LCD_COLS - 1);                         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(LCD_ROWS - 1, photo, x, 0, 319, 0);    // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (x = LCD_ROWS - 1; x >= 0; x--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(DOWN, 0, 0, LCD_ROWS - 1, LCD_COLS - 1);                        // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(0, photo, x, 0, 319, 0);                // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
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
            lcd_putColumn(x, photo, x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_putColumn(x, photo, x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case UP:
        for (y = 0; y <= LCD_ROWS - 1; y++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_putRow(y, photo, y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
            sw_delay_ms(6);        // Espera un tiempo para que se vea el efecto
        }
        break;
    case DOWN:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_putRow(y, photo, y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
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
    int16 x, y;

    switch (sense)
    {
    case LEFT:
        for (x = LCD_COLS - 1; x >= 0; x--)  // Recorre la foto por columnas de izquierda a derecha
        {
            lcd_shift(LEFT, 0, x, LCD_ROWS - 1, 0);      // Desplaza toda la pantalla una columna a la izquierda
            lcd_putColumn(x, photo, x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = 0; x < LCD_COLS; x++)  // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(RIGHT, 0, x, LCD_ROWS - 1, LCD_COLS - 1);     // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(x, photo, x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(UP, y, 0, 0, LCD_COLS - 1);         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(y, photo, y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (y = 0; y < LCD_ROWS; y++) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(DOWN, y, 0, LCD_ROWS - 1, LCD_COLS - 1);      // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(y, photo, y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
        }
        break;
    }
}

/*
** Efecto cobertura: La nueva imagen se superpone haciendo scroll sobre la imagen mostrada
*/
void efectoCobertura(uint8 *photo, uint8 sense)
{
    int16 x, y;

    switch (sense)
    {
    case LEFT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(LEFT, 0, LCD_COLS - 1, LCD_ROWS - 1, x);      // Desplaza toda la pantalla una columna a la izquierda
            lcd_putColumn(LCD_COLS - 1, photo, (LCD_COLS - 1) - x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = 0; x < LCD_COLS; x++) // Recorre la foto por columnas de izquierda a derecha
        {
            lcd_shift(RIGHT, 0, 0, LCD_ROWS - 1, x);     // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(0, photo, (LCD_COLS - 1) - x, 0, 239, 0); // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(UP, LCD_ROWS - 1, 0, y, LCD_COLS - 1);         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(LCD_ROWS - 1, photo, (LCD_ROWS - 1) - y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (y = 0; y < LCD_ROWS; y++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(DOWN, 0, 0, y, LCD_COLS - 1);      // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(0, photo, (LCD_ROWS - 1) - y, 0, 319, 0); // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
        }
        break;
    }
}

void isr_pb( void )
{
    flagPb = TRUE;
    EXTINTPND = BIT_RIGHTPB | BIT_LEFTPB;
    I_ISPC = BIT_PB;
}

void isr_keyPad( void )
{
    flagKeyPad = TRUE;
    I_ISPC = BIT_KEYPAD;
}

void isr_ts( void )
{
    flagTs = TRUE;
    INTPND &= ~(BIT_EINT2);
    I_ISPC = BIT_TS;
}
