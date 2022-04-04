OUTPUT_ARCH( "riscv" )
ENTRY(_start)


SECTIONS
{
	. = 0x00000000;
	.text.init : {
			* (.text.init);
	}
	.text : {
	      * (.text);
	}
	. = SIZE;
	.data : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.data);
	      . = ALIGN(16);
	}
	.srodata : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.srodata);
	      . = ALIGN(16);
	}
	.rodata : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.rodata);
	      . = ALIGN(16);
	}
	.bss : SUBALIGN(16){
	     . = ALIGN(16);
	     * (.bss);
	     __bss_end = .;
         . = ALIGN(16);
	}

	PROVIDE( end = . );
    PROVIDE( _end = . );
    PROVIDE( __end = . );
}
