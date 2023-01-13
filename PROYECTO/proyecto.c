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
#include <stdlib.h>

/* N�mero m�ximo de fotos distintas visualizables en el marco */

#define MAX_PHOTOS (40)

/* Direcciones en donde se encuentran cargados los BMPs tras la ejecuci�n del comando "script load_bmp.script" */

#define ARBOL       ((uint8 *)0x0c210000)
#define PICACHU     ((uint8 *)0x0c220000)
#define PULP        ((uint8 *)0x0c230000)
#define HARRY       ((uint8 *)0x0c240000)

#define ROSALINA    ((int16 *)0x0c704FB0)

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
inline uint16 volToCoord(uint8 vol);
inline uint8 coordToVol(uint16 coord);
void initPack();

void lcd_shift(uint8 sense, uint16 initRow, uint16 initCol, uint16 endRow, uint16 endCol);
void lcd_putColumn(uint16 colLcd, uint16 colPhoto, uint16 yLcdUp, uint16 yLcdDown, uint16 yPhotoUp, uint8 *photo);
void lcd_putRow(uint16 yLcd, uint16 yPhoto, uint16 colLcdLeft, uint16 colLcdRight, uint16 colPhotoLeft, uint8 *photo);
void lcd_putSquare(uint16 colLeft, uint16 colRight, uint16 yUp, uint16 yDown, uint8 *photo);
void lcd_putBox(uint16 colLeft, uint16 colRight, uint16 yUp, uint16 yDown, uint8 *photo);
void lcd_putPhoto(uint8 *photo);
void lcd_putMiniaturePhoto(uint8 *min, uint8 pos);
void zDMA_transfer(uint8 *src, uint8 *dst, uint32 size, uint8 mode);
void lcd_clearDMA();
void lcd_restore();
void lcd_backUp();
void keypad_action();


/* Declaraci�n de efectos de transici�n entre fotos */
void efectoAleatorio(uint8 *photo, uint8 sense);
void efectoNulo(uint8 *photo, uint8 sense);
void efectoEmpuje(uint8 *photo, uint8 sense);
void efectoBarrido(uint8 *photo, uint8 sense);
void efectoRevelado(uint8 *photo, uint8 sense);
void efectoCobertura(uint8 *photo, uint8 sense);
void efectoDivisionEntrante(uint8 *photo, uint8 sense);
void efectoDivisionSaliente(uint8 *photo, uint8 sense);
void efectoDisolver(uint8 *photo, uint8 sense);
void efectoCuadradoEntrante(uint8 *photo, uint8 sense);
void efectoCuadradoSaliente(uint8 *photo, uint8 sense);
void efectoFlash(uint8 *photo, uint8 sense);
void efectoPeine(uint8 *photo, uint8 sense);
void efectoBarras(uint8 *photo, uint8 sense);


// variables globales
album_t album;
pf_t effectArray[] = {efectoAleatorio, efectoNulo, efectoEmpuje, efectoBarrido, efectoRevelado, efectoCobertura, efectoDivisionEntrante,
    efectoDivisionSaliente, efectoDisolver, efectoCuadradoEntrante, efectoCuadradoSaliente, efectoFlash, efectoPeine, efectoBarras}; // Array de puntero a funcion de ejecto
char* effectName[] = {"Aleatorio", "Nulo", "Empuje", "Barrido", "Revelado", "Cobertura", "DivisionIn", "DivisionOut",
    "Disolver", "CuadradoIn", "CuadradoOut", "Fade", "Peine", "Barras"}; // Array de nombres de efectos
char* senseName[]  = {"LEFT", "RIGHT", "UP", "DOWN"};
uint8 tieneSentido[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1}; // Array de bool para saber si el efecto correspondiente tiene sentido o no.
uint8 *photoArray[] = {ARBOL, PICACHU, PULP, HARRY}; // Array de punteros a las fotos a visualizar
uint8 *minArray[] = {MINIARBOL, MINIPICACHU, MINIPULP, MINIHARRY}; // Array de punteros a las miniaturas de las fotos a visualizar
uint8 bkUpBuffer[LCD_BUFFER_SIZE];
uint8 scancode; // Variable para almacenar el c�digo de tecla pulsada
uint8 volumen; // Variable para almacenar el volumen de reproducci�n
uint16 xTs; // Variables para almacenar las coordenadas del TS
uint16 yTs; // Variables para almacenar las coordenadas del TS
boolean aleatorio; // Variable para indicar si se reproduce el album en modo aleatorio
uint8 volumen; // Variable para almacenar el volumen de reproducci�n

const uint8 numPhotos = 4; // N�mero de fotos a visualizar
const uint8 numEffects = 14; // N�mero de efectos de transici�n entre fotos

static unsigned long int next = 1;

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
    aleatorio = FALSE;

    timer3_start();

    uint8 i;
    for (i = 0; i < numPhotos; i++)
    {
        lcd_bmp2min(minArray[i], album.pack[i].data.min);
        lcd_bmp2photo(photoArray[i], album.pack[i].data.photoBuffer);
    }
    album.numPacks = numPhotos;
    album.index = 0;

    next = timer3_stop();
    initPack();

    uda1341ts_setvol(VOL_MED);
    volumen = VOL_MED;
    iis_playWawFile(ROSALINA, TRUE);

    //menuPrincipal();
    lcd_clear();
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
    test(album.pack[album.index].data.photoBuffer); // Chequea que el resultado del efecto es el deseado
    sw_delay_s(album.pack[album.index].secs); // Mantiene la foto el tiempo indicado
    album.index += aleatorio ? rand() : 1; album.index %= album.numPacks; // Avanza circularmente a la siguiente foto del album
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
            switch (yTs){
            case 18 ... 19 + 32:
                index -= (index == 0) ? 0 : 2;
                break;
            case 203 ... LCD_HEIGHT - 1:
                index += 2; index %= album.numPacks;
                break;
            case 75 ... 202:
                lcd_backUp();
                menuSettings(index + (xTs > LCD_WIDTH/2));
                lcd_restore();
                break;
            default:
                break;
            }
        } 
    }
    flagPb = FALSE;

    sw_delay_s(1);
}

void menuPausa()
{

    lcd_backUp();
    lcd_clearDMA();

    lcd_puts(LCD_WIDTH/2 - 32, 0, BLACK, "Volumen:");
    lcd_draw_box(20, 18, 300, 50, BLACK, 2); // tamaño caja 32
    lcd_draw_filled_box(20, 18, volToCoord(volumen), 50, BLACK);
    //espacio entre cajas de 24
    lcd_draw_box(20, 74, 300, 106, BLACK, 2);
    lcd_puts(LCD_WIDTH/2 - 40, 82, BLACK, "Siguiente");

    lcd_draw_box(20, 130, 300, 162, BLACK, 2);
    lcd_puts(LCD_WIDTH/2 - 24, 138, BLACK, "Salir");

    while (!flagExit && !flagPb)
    {
        while (!flagTs && !flagPb);

        if (flagTs)
        {
            ts_getpos(&xTs, &yTs);
            flagTs = FALSE;
            switch (yTs){
            case 18 ... 50:
                volumen = coordToVol(xTs);
                uda1341ts_setvol(volumen);
                lcd_draw_filled_box(22, 20, 299, 49, WHITE);
                lcd_draw_filled_box(20, 18, volToCoord(volumen), 19 + 31, BLACK);
                break;
            case 74 ... 105:
                aleatorio = !aleatorio;
                if (aleatorio)
                    lcd_puts(LCD_WIDTH/2 - 40, 82, BLACK, "Aleatorio");
                else
                    lcd_puts(LCD_WIDTH/2 - 40, 82, BLACK, "Siguiente");
                break;
            case 130 ... 161:
                flagExit = TRUE;
                break;
            default:
                break;
            }
        }
    }
    flagPb = FALSE;



    lcd_restore();
}

inline uint8 coordToVol(uint16 x)
{
    if (x < 20) return VOL_MIN;
    if (x > 300) return VOL_MAX;
    return (x - 20) * (VOL_MAX) / 280;
}

inline uint16 volToCoord(uint8 vol)
{
    return (vol * 280) / (VOL_MAX) + 20;
}

/**
 *  _________________________________________________
 * | configura la foto                              |
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
 * |  modo :                        Use el keypad   |                                                   
 * |________________________________________________|
*/
void menuSettings(uint8 index)
{
    uint8 secs = 3;
    uint8 sense = 0;
    uint8 indexEfecto;
    indexEfecto = 0;
    lcd_clearDMA();
    lcd_puts(88, 0, BLACK, "Configura la foto");

    lcd_putMiniaturePhoto(album.pack[index].data.min, 0);
    
    lcd_draw_box((MIN_WIDTH + 20), 65, (MIN_WIDTH + 160), 85, BLACK, 2);
    lcd_puts(174, 68, BLACK, "SEGUNDOS: " );
    lcd_putint(270, 68, BLACK, 3);

    lcd_draw_box((MIN_WIDTH + 20), 107, (MIN_WIDTH + 160), 143, BLACK, 2);
    lcd_puts(210, 110, BLACK, "EFECTO: " );
    lcd_puts(174, 125, BLACK, "Aleatorio");

    lcd_draw_box((MIN_WIDTH + 20), 165, (MIN_WIDTH + 160), 185, BLACK, 2);
    lcd_puts(174, 168, BLACK, "SENTIDO: " );
    lcd_puts(240, 168, BLACK, "No apply");

    lcd_puts(10, (LCD_HEIGHT - 15), BLACK, "CAMBIANDO:");
    
    while (!flagPb)
    { 
        while (!flagTs && !flagPb);

        if (flagTs)
        {
            ts_getpos(&xTs, &yTs);
            flagTs = FALSE;
            
            switch (yTs)
            {
            case 63 ... 88: // Segundos
                if(xTs < 160) break;
                lcd_puts(98, (LCD_HEIGHT - 15), BLACK, "SEGUNDOS");
                lcd_puts(LCD_WIDTH - 123, (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                while(!flagKeyPad);
                keypad_action();
                secs = scancode;
                lcd_puts(270, 68, BLACK, "  ");
                lcd_putint(270, 68, BLACK, secs);
                break;
            case 113 ... 145: // Efecto
                if(xTs < 160) break;
                lcd_puts(98, (LCD_HEIGHT - 15), BLACK, "EFFECTO");
                lcd_puts(LCD_WIDTH - 123, (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                while(!flagKeyPad);
                keypad_action();
                indexEfecto = scancode;
                lcd_puts(174, 125, BLACK, "               ");
                lcd_puts(174, 125, BLACK, effectName[indexEfecto]);
                if (!tieneSentido[indexEfecto])
                lcd_puts(240, 168, BLACK, "No apply");
                else{
                    lcd_puts(240, 168, BLACK, "        ");
                    lcd_puts(240, 168, BLACK, senseName[sense]);
                }
                break;
            case 160 ... 190: // Sentido  
                if(xTs < 160) break;
                if (tieneSentido[indexEfecto])
                {
                    lcd_puts(98, (LCD_HEIGHT - 15), BLACK, "SENTIDO");
                    lcd_puts(LCD_WIDTH - 123, (LCD_HEIGHT - 15), BLACK, "Use el keypad");
                    while(!flagKeyPad);
                    keypad_action();
                    sense = scancode;
                    lcd_puts(240, 168, BLACK, "        ");
                    lcd_puts(240, 168, BLACK, senseName[sense]);
                }
                break;
            default:
                break;
            }
            lcd_puts(98, (LCD_HEIGHT - 15), BLACK, "         ");
            lcd_puts(LCD_WIDTH - 123, (LCD_HEIGHT - 15), BLACK, "              ");
        }
    }
    flagPb = FALSE;
    album.pack[index].secs = secs;
    album.pack[index].effect = effectArray[indexEfecto];
    album.pack[index].sense = sense;
}

void keypad_action(){
    flagKeyPad = FALSE;
    scancode = keypad_getchar();
}

void initPack()
{
    uint8 i = 0;
    for (i = 0; i < numPhotos; i++)
    {
        album.pack[i].effect = efectoAleatorio;
        album.pack[i].secs = 3;
        album.pack[i].sense = i % 4;
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
void lcd_putColumn(uint16 colLcd, uint16 colPhoto, uint16 yLcdUp, uint16 yLcdDown, uint16 yPhotoUp, uint8 *photo)
{
    for (; yLcdUp <= yLcdDown; yLcdUp++, yPhotoUp++)
        lcd_buffer[(yLcdUp * LCD_COLS) + colLcd] = photo[(yPhotoUp * LCD_COLS) + colPhoto];
}

/*
** Visualiza una linea de la foto en una linea dada de la pantalla
*/
void lcd_putRow(uint16 yLcd, uint16 yPhoto, uint16 colLcdLeft, uint16 colLcdRight, uint16 colPhotoLeft, uint8 *photo)
{
    zDMA_transfer(photo + (yPhoto * LCD_COLS) + colLcdLeft , lcd_buffer + (yLcd * LCD_COLS) + colPhotoLeft, (colLcdRight - colLcdLeft) + 1, SRC_INCR| DES_INCR);
}

/*
** Visualiza un cuadrado de la foto en un cuadrado dado de la pantalla
*/
void lcd_putSquare(uint16 colLeft, uint16 colRight, uint16 yUp, uint16 yDown, uint8 *photo)
{
    while (yUp <= yDown)
    {
        lcd_putRow(yUp, yUp, colLeft, colRight, colLeft, photo);
        yUp++;
    }
}

void lcd_putBox(uint16 colLeft, uint16 colRight, uint16 yUp, uint16 yDown, uint8 *photo)
{
    lcd_putRow(yUp, yUp, colLeft, colRight, colLeft, photo);
    lcd_putRow(yDown, yDown, colLeft, colRight, colLeft, photo);
    lcd_putColumn(colLeft, colLeft, yUp, yDown, yUp, photo);
    lcd_putColumn(colRight, colRight, yUp, yDown, yUp, photo);
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
        if (initCol == 0 && endCol == LCD_COLS - 1)
            zDMA_transfer(lcd_buffer + (LCD_COLS * (endRow + 1)), lcd_buffer + (LCD_COLS * endRow), (initRow - endRow) * LCD_COLS, SRC_INCR | DES_INCR); // Desplaza una fila hacia arriba
        else
            for (y = initRow; y < endRow; y++) // Recorre la pantalla por filas de arriba hacia abajo
            {
                zDMA_transfer(lcd_buffer + ((y + 1) * LCD_COLS) + initCol, lcd_buffer + (y * LCD_COLS) + initCol, (endCol - initCol) + 1, SRC_INCR | DES_INCR); // Desplaza la fila una columna a la izquierda
            }
        break;
    case DOWN:
        if (initCol == 0 && endCol == LCD_COLS - 1)
            zDMA_transfer(lcd_buffer + (LCD_COLS * endRow) - 1, lcd_buffer + (LCD_COLS * (endRow + 1) - 1), (endRow - initRow) * LCD_COLS, SRC_DEC | DES_DEC); // Desplaza una fila hacia abajo
        else 
            for (y = endRow; y > initRow; y--) // Recorre la pantalla por filas de abajo hacia arriba
            {
                zDMA_transfer(lcd_buffer + ((y - 1) * LCD_COLS) + initCol, lcd_buffer + (y * LCD_COLS) + initCol, (endCol - initCol) + 1, SRC_INCR | DES_INCR); // Desplaza la fila una columna a la derecha
            }
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

int rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed)
{
    next = seed;
}

/*******************************************************************/

/*
** Efecto nulo: Muestra la foto sin hacer efecto alguno
**
** Incluido por homogeneidad
*/
void efectoNulo(uint8 *photo, uint8 sense)
{
    lcd_putPhoto(photo);
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
            lcd_putColumn(LCD_COLS - 1, x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(RIGHT, 0, 0, LCD_ROWS - 1, LCD_COLS - 1);                      // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(0, x, 0, 239, 0, photo);            // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (x = 0; x <= LCD_ROWS - 1; x++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(UP, LCD_ROWS - 1, 0, 0, LCD_COLS - 1);                         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(LCD_ROWS - 1, x, 0, LCD_COLS - 1, 0, photo);    // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (x = LCD_ROWS - 1; x >= 0; x--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(DOWN, 0, 0, LCD_ROWS - 1, LCD_COLS - 1);                        // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(0, x, 0, LCD_COLS - 1, 0, photo);                // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
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
            lcd_putColumn(x, x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case RIGHT:
        for (x = LCD_COLS - 1; x >= 0; x--) // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_putColumn(x, x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la columna de la pantalla que corresponde
            sw_delay_ms(5);            // Espera un tiempo para que se vea el efecto
        }
        break;
    case UP:
        for (y = 0; y <= LCD_ROWS - 1; y++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_putRow(y, y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
            sw_delay_ms(6);        // Espera un tiempo para que se vea el efecto
        }
        break;
    case DOWN:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_putRow(y, y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la fila de la pantalla que corresponde
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
            lcd_putColumn(x, x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = 0; x < LCD_COLS; x++)  // Recorre la foto por columnas de derecha a izquierda
        {
            lcd_shift(RIGHT, 0, x, LCD_ROWS - 1, LCD_COLS - 1);     // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(x, x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(UP, y, 0, 0, LCD_COLS - 1);         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(y, y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (y = 0; y < LCD_ROWS; y++) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(DOWN, y, 0, LCD_ROWS - 1, LCD_COLS - 1);      // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(y, y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
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
            lcd_putColumn(LCD_COLS - 1, (LCD_COLS - 1) - x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la ultima columna de la pantalla
        }
        break;
    case RIGHT:
        for (x = 0; x < LCD_COLS; x++) // Recorre la foto por columnas de izquierda a derecha
        {
            lcd_shift(RIGHT, 0, 0, LCD_ROWS - 1, x);     // Desplaza toda la pantalla una columna a la derecha
            lcd_putColumn(0, (LCD_COLS - 1) - x, 0, 239, 0, photo); // Visualiza la columna de la foto que corresponde en la primera columna de la pantalla
        }
        break;
    case UP:
        for (y = LCD_ROWS - 1; y >= 0; y--) // Recorre la foto por filas de abajo hacia arriba
        {
            lcd_shift(UP, LCD_ROWS - 1, 0, y, LCD_COLS - 1);         // Desplaza toda la pantalla una fila hacia arriba
            lcd_putRow(LCD_ROWS - 1, (LCD_ROWS - 1) - y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la ultima fila de la pantalla
        }
        break;
    case DOWN:
        for (y = 0; y < LCD_ROWS; y++) // Recorre la foto por filas de arriba hacia abajo
        {
            lcd_shift(DOWN, 0, 0, y, LCD_COLS - 1);      // Desplaza toda la pantalla una fila hacia abajo
            lcd_putRow(0, (LCD_ROWS - 1) - y, 0, LCD_COLS - 1, 0, photo); // Visualiza la fila de la foto que corresponde en la primera fila de la pantalla
        }
        break;
    }
}

/*
** Efecto division: La nueva imagen se superpone desde los extremos de la pantalla hacia el centro
*/
void efectoDivisionEntrante(uint8 *photo, uint8 sense)
{
    uint16 i, j;
    switch (sense)
    {
    case LEFT: case RIGHT:
        for (i = 0, j = LCD_COLS - 1; j >= LCD_COLS/2 && i < LCD_COLS/2; i++, j--)
        {
            lcd_putColumn(j, j, 0, 239, 0, photo);
            lcd_putColumn(i, i, 0, 239, 0, photo);
            sw_delay_ms(10);
        }
        break;
    case UP: case DOWN:
    for (i = 0, j = LCD_ROWS - 1; j >= LCD_ROWS/2 && i < LCD_ROWS/2; i++, j--)
        {
            lcd_putRow(j, j, 0, LCD_COLS - 1, 0, photo);
            lcd_putRow(i, i, 0, LCD_COLS - 1, 0, photo);
            sw_delay_ms(13);
        }
        break;
    }
}

/*
** Efecto divisionSaliente: La nueva imagen se superpone desde el centro de la pantalla hacia los extremos
*/
void efectoDivisionSaliente(uint8 *photo, uint8 sense)
{
    uint16 i , j;
    switch (sense)
    {
    case LEFT: case RIGHT:
        for (i = LCD_COLS/2, j = LCD_COLS/2 - 1; j >= 0 && i < LCD_COLS; i++, j--)
        {
            lcd_putColumn(j, j, 0, 239, 0, photo);
            lcd_putColumn(i, i, 0, 239, 0, photo);
            sw_delay_ms(10);
        }
        break;
    case UP: case DOWN:
        for (i = LCD_ROWS/2, j = LCD_ROWS/2 - 1; j >= 0 && i < LCD_ROWS; i++, j--)
        {
            lcd_putRow(j, j, 0, LCD_COLS - 1, 0, photo);
            lcd_putRow(i, i, 0, LCD_COLS - 1, 0, photo);
            sw_delay_ms(13);
        }
        break;
    }
}

/*
** Efecto Disolver: La nueva imagen se va mostrando con cuadrados de 40x30 pixeles
*/
void efectoDisolver(uint8 *photo, uint8 sense)
{
    uint8 cuadrados[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    uint8 i, j;

    for(i = 0; i < 64; i++)
    {
        do 
        {
            j = rand() % 64;
        } while (cuadrados[j / 8] & (1 << (j % 8)));
        cuadrados[j / 8] |= (1 << (j % 8));

        lcd_putSquare(((j % 8) * 20), ((j % 8) * 20 + 19), (j / 8) * 30, ((j / 8) * 30 + 29), photo);
        sw_delay_ms(13);
    }
}

/*
** Efecto CuadradoEntrante: la nueva imagen se va mostrando desde los extremos de la pantalla hacia el centro
*/
void efectoCuadradoEntrante(uint8 *photo, uint8 sense)
{
    uint8 i, j;

    for (i = 0, j = 0; i < LCD_COLS/2 && j < LCD_ROWS/2; i++, j++)
    {
        lcd_putRow(j, j, i, (LCD_COLS - 1) - i, i, photo);
        lcd_putRow((LCD_ROWS - 1) - j, (LCD_ROWS - 1) - j, i, (LCD_COLS - 1) - i, i, photo);
        lcd_putColumn(i, i, j, (LCD_ROWS - 1) - j, j, photo);
        lcd_putColumn((LCD_COLS - 1) - i, (LCD_COLS - 1) - i, j, (LCD_ROWS - 1) - j, j, photo);
        sw_delay_ms(15);
    }

}

/*
** Efecto CuadradoSaliente: la nueva imagen se va mostrando desde el centro de la pantalla hacia los extremos
*/
void efectoCuadradoSaliente(uint8 *photo, uint8 sense)
{
    uint16 i, j;

    for (i = LCD_COLS/2, j = LCD_ROWS/2; i < LCD_COLS || j < LCD_ROWS; i+= j%2, j++)
    {
        j = (j >= LCD_ROWS - 1) ? LCD_ROWS - 1 : j;
        lcd_putRow(j, j, (LCD_COLS - 1) - i, i, (LCD_COLS - 1) - i, photo);
        lcd_putRow((LCD_ROWS - 1) - j, (LCD_ROWS - 1) - j, (LCD_COLS - 1) - i, i, (LCD_COLS - 1) - i, photo);
        lcd_putColumn((LCD_COLS - 1) - i, (LCD_COLS - 1) - i, (LCD_ROWS - 1) - j, j, (LCD_ROWS - 1) - j, photo);
        lcd_putColumn(i, i, (LCD_ROWS - 1) - j, j, (LCD_ROWS - 1) - j, photo);
        sw_delay_ms(15);
    }
}

/*
** Efecto Flash:la imagen se oscurece y luego se muestra progresivamente la nueva imagen
*/
void efectoFlash(uint8 *photo, uint8 sense)
{
    uint16 i, j;
    uint16 aux;
    for (i = 0; i < 4; i++)
    {
        lcd_backUp();
        for (j = 0; j < LCD_BUFFER_SIZE; j++)
        {
            aux = bkUpBuffer[j];
            aux += ((aux & 0xF0) + 0x50 <= 0xF0) ? 0x50 : 0xF0 - (aux & 0xF0);
            aux += ((aux & 0x0F) + 0x05 <= 0x0F) ? 0x05 : 0x0F - (aux & 0x0F);
            bkUpBuffer[j] = aux;
        }
        lcd_restore();
    }
    for (i = 0; i < 4; i++)
    {
        lcd_backUp();
        for(j = 0; j < LCD_BUFFER_SIZE; j++)
        {
           aux = bkUpBuffer[j];
           aux -= ((aux & 0xF0) - 0x50 >= (photo[j] & 0xF0)) ? 0x50 : (aux & 0xF0) - (photo[j] & 0xF0);
           aux -= ((aux & 0x0F) - 0x05 >= (photo[j] & 0x0F)) ? 0x05 : (aux & 0x0F) - (photo[j] & 0x0F);
           bkUpBuffer[j] = aux;            
        }
        lcd_restore();
    }
}

/*
** Efecto Peine: la imagen se va mostrando con barras que desplazan a la imagen anterior
*/
void efectoPeine(uint8 *photo, uint8 sense)
{
    int16 col1 = 0, col2 = 0, col3 = 0, col4 = 0, col5 = 0, col6 = 0;
    int16 row1 = 0, row2 = 0, row3 = 0, row4 = 0, row5 = 0, row6 = 0, row7 = 0, row8 = 0;

    switch (sense)
    {
    case LEFT: case RIGHT:
        while (col1 < LCD_COLS)
        {
            switch (rand() % 6)
            {
            case 0:
                if (col1 > LCD_COLS - 1) break;
                lcd_shift(RIGHT, 0, col1, 39, LCD_COLS - 1);
                lcd_putColumn(col1, col1, 0, 39, 0, photo);
                col1++;
            case 1:
                if (col2 > LCD_COLS - 1) break;
                lcd_shift(LEFT, 40, (LCD_COLS - 1) - col2, 79, 0);
                lcd_putColumn((LCD_COLS - 1) - col2, (LCD_COLS - 1) - col2, 40, 79, 40, photo);
                col2++;
            case 2:
                if (col3 > LCD_COLS - 1) break;
                lcd_shift(RIGHT, 80, col3, 119, LCD_COLS - 1);
                lcd_putColumn(col3, col3, 80, 119, 80, photo);
                col3++;
            case 3:
                if (col4 > LCD_COLS - 1) break;
                lcd_shift(LEFT, 120, (LCD_COLS - 1) - col4, 159, 0);
                lcd_putColumn((LCD_COLS - 1) - col4, (LCD_COLS - 1) - col4, 120, 159, 120, photo);
                col4++;
            case 4:
                if (col5 > LCD_COLS - 1) break;
                lcd_shift(RIGHT, 160, col5, 199, LCD_COLS - 1);
                lcd_putColumn(col5, col5, 160, 199, 160, photo);
                col5++;
            case 5:
                if (col6 > LCD_COLS - 1) break;
                lcd_shift(LEFT, 200, (LCD_COLS - 1) - col6, 239, 0);
                lcd_putColumn((LCD_COLS - 1) - col6, (LCD_COLS - 1) - col6, 200, 239, 200, photo);
                col6++;
            }
        }
        break;
    
    case UP: case DOWN:
        while (row1 < LCD_ROWS)
        {
            switch (rand() % 8)
            {
            case 0:
                if (row1 > LCD_ROWS - 1) break;
                lcd_shift(DOWN, row1, 0, LCD_ROWS - 1, 19);
                lcd_putRow(row1, row1, 0, 19, 0, photo);
                row1++;
            case 1:
                if (row2 > LCD_ROWS - 1) break;
                lcd_shift(UP, (LCD_ROWS - 1) - row2, 20, 0, 39);
                lcd_putRow((LCD_ROWS - 1) - row2, (LCD_ROWS - 1) - row2, 20, 39, 20, photo);
                row2++;
            case 2:
                if (row3 > LCD_ROWS - 1) break;
                lcd_shift(DOWN, row3, 40, LCD_ROWS - 1, 59);
                lcd_putRow(row3, row3, 40, 59, 40, photo);
                row3++;
            case 3:
                if (row4 > LCD_ROWS - 1) break;
                lcd_shift(UP, (LCD_ROWS - 1) - row4, 60, 0, 79);
                lcd_putRow((LCD_ROWS - 1) - row4, (LCD_ROWS - 1) - row4, 60, 79, 60, photo);
                row4++;
            case 4:
                if (row5 > LCD_ROWS - 1) break;
                lcd_shift(DOWN, row5, 80, LCD_ROWS - 1, 99);
                lcd_putRow(row5, row5, 80, 99, 80, photo);
                row5++;
            case 5:
                if (row6 > LCD_ROWS - 1) break;
                lcd_shift(UP, (LCD_ROWS - 1) - row6, 100, 0, 119);
                lcd_putRow((LCD_ROWS - 1) - row6, (LCD_ROWS - 1) - row6, 100, 119, 100, photo);
                row6++;
            case 6:
                if (row7 > LCD_ROWS - 1) break;
                lcd_shift(DOWN, row7, 120, LCD_ROWS - 1, 139);
                lcd_putRow(row7, row7, 120, 139, 120, photo);
                row7++;
            case 7:
                if (row8 > LCD_ROWS - 1) break;
                lcd_shift(UP, (LCD_ROWS - 1) - row8, 140, 0, 159);
                lcd_putRow((LCD_ROWS - 1) - row8, (LCD_ROWS - 1) - row8, 140, 159, 140, photo);
                row8++;
            }
        }
        break;
    }
}

/*
* Efecto Barras: La imagen se va mostrando a en tiras dispuestas de manera aleatoria
*/
void efectoBarras(uint8 *photo, uint8 sense)
{
    uint16 i, j;
    uint16 columnas[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint16 filas[15] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    switch (sense)
    {
    case LEFT: case RIGHT:
        for (i = 0; i < LCD_COLS; i++)
        {
            do
            {
                j = rand() % 160;
            } while (columnas[j / 16] & (1 << (j % 16)));
            columnas[j / 16] |= (1 << (j % 16));

            lcd_putColumn(j, j, 0, 239, 0, photo);
            sw_delay_ms(6);
        }
        break;
    case UP: case DOWN:
        for (i = 0; i < LCD_ROWS; i++)
        {
            do
            {
                j = rand() % 240;
            } while (filas[j / 16] & (1 << (j % 16)));
            filas[j / 16] |= (1 << (j % 16));

            lcd_putRow(j, j, 0, 159, 0, photo);
            sw_delay_ms(6);
        }
        break;
    }
}

/*
* Efecto aleatorio: Se elige un efecto aleatorio y se le pasa un sentido aleatorio
*/
void efectoAleatorio(uint8 *photo, uint8 sense)
{
    effectArray[rand() % numEffects](photo, rand() % 4);
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
