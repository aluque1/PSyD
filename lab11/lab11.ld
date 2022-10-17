/*---------------------------------------------------------------------
**
**  Fichero:
**    lab11.ld  2/2/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Script básico de enlazado para proyectos volcados en ROM
**
**  Notas de diseño:
**
**-------------------------------------------------------------------*/
ENTRY( main )
SECTIONS
{
    . = 0x0c000000;
        _image_start = .;
    .text : { bootloader.o * (.text) } 
    .rodata : { *(.rodata) } 
    .data : { *(.data) } 
    .bss : { *(.bss) }     
        _image_end = .;
    end = .;
}
