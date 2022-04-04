OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY {
    ICCM    : ORIGIN = 0xee000000, LENGTH = 0xa0000
    DCCM    : ORIGIN = 0xf0040000, LENGTH = 0x20000
}

SECTIONS
{
	.text : {
	      * (.text.*);
	} > ICCM
	.data : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.data .data.*);
          . = ALIGN(16);
	} > DCCM
	.sdata : SUBALIGN(16){
    	      . = ALIGN(16);
    	      * (.srodata);
    	      * (.srodata.*);
              * (.sdata);
              . = ALIGN(16);
    	} > DCCM
	.rodata : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.rodata .rodata.*);
	      . = ALIGN(16);
	} > DCCM
	 .bss : SUBALIGN(16){
	      . = ALIGN(16);
	      * (.bss .bss.*);
	      __bss_end = .;
	      . = ALIGN(16);
	} > DCCM

	PROVIDE( end = . );
	PROVIDE( _end = . );
	PROVIDE( __end = . );

}
