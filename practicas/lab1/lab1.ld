/*---------------------------------------------------------------------
**
**  Fichero:
**    lab1.ld  2/2/2013
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Script básico de enlazado para proyectos volcados en RAM
**
**  Notas de diseño:
**
**-------------------------------------------------------------------*/
ENTRY( main )
SECTIONS
{
    . = 0x0c100000;
    .text : { * (.text) } 
    .rodata : { *(.rodata) } 
    .data : { *(.data) } 
    .bss : { *(.bss) }     
    end = .;
}
