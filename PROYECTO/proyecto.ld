ENTRY( main )
SECTIONS
{
    . = 0x0c100000;
    
    	_image_start = .;
    	
   		_code_start = .;
    .text : { * (.text) }
    	_code_end = .; 
    	_code_size = SIZEOF (.text);
    
    	_data_start = .;
    .rodata : { *(.rodata) }
    .data : { *(.data) } 
    .bss : { *(.bss) }
    	_data_end = .;
    	_data_size = SIZEOF(.rodata) + SIZEOF(.data) + SIZEOF(.bss);
       	
       	_total_size = _code_size + _data_size;
       	    	
    	_image_end = .;

    end = .;
}
