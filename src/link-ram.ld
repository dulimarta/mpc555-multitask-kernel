OUTPUT_ARCH(powerpc)

TARGET(elf32-powerpc)

ENTRY(START)

SEARCH_DIR (../libmpc)
SEARCH_DIR (../kernel)

STARTUP (crt0.o)
INPUT (stdlib.o stream.o)

PROVIDE (USIU     = 0x2FC000); /* SIU Module Configuration Register */

/* Table A-4 CMF (CDR MoneT Flash EEPROM) */
PROVIDE (CMF_A    = 0x2FC800);
PROVIDE (CMF_B    = 0x2FC840);

/*  Table A-5 DPTRAM (Dual-Port TPU RAM) */
PROVIDE (DPTRAM   = 0x300000); /* DPT Module Configuration Register */

/*  Time Processing Unit (TPU3) */
PROVIDE (TPU_A    = 0x304000); /* TPU_A */
PROVIDE (TPU_B    = 0x304400); /* TPU_B */

PROVIDE (QADC1    = 0x304800);
PROVIDE (QADC2    = 0x304C00);

/* QSMCM (Queued Serial Multi-Channel Module) */
PROVIDE (QSM      = 0x305000); /* Queued Serial Multi Channel Module */

PROVIDE (SCTQ     = 0x30502C); /* Transmit Queue Locations  */
PROVIDE (SCRQ     = 0x30504C); /* Receive Queue Locations  */
PROVIDE (RECRAM   = 0x305140); /* Receive Data RAM  */
PROVIDE (TRAN_RAM = 0x305180); /* Transmit Data RAM  */
PROVIDE (COMD_RAM = 0x3051C0); /* Command RAM  */

/* MIOS1 (Modular Input/Output Subsystem) */
PROVIDE (MIOS     = 0x306000);
PROVIDE (MBISM    = 0x306800);

/* UIMB (U-Bus to IMB3 Bus Interface) */
PROVIDE (UIMB     = 0x307F80);
PROVIDE (SRAM     = 0x380000);

PROVIDE (LCD_CMD  = 0x1000000);
PROVIDE (LCD_DATA = 0x1000001);
PROVIDE (KP_DATA  = 0x1000004);

STKTOP   = 0x400000;

MEMORY {
   DPTRAM   (RW) : ORIGIN = 0x302000, LENGTH =   6K
   DataSeg  (RW) : ORIGIN = 0x3FA000, LENGTH =  19K
   CodeSeg  (R) : ORIGIN = 0x800000, LENGTH = 512K
}

HEAP_END = 0x3FA000 + 19K; /* in IntRAM */
/* HEAP_END = 0x800000 + 512K - 4; /* in CodeSeg */

SECTIONS {
   /*--------------------*/
   /* Read Only sections */
   /*--------------------*/
   .text    : 
   { 
      *(.init)    
      *(.text)    
      *(.rodata)   
      *(.got)
      *(.got2)
      *(.gnu.linkonce.t.*)
      __CTOR_LIST__ = .;
      *(.ctors)   
      __CTOR_END__ = .;
      __DTOR_LIST__ = .;
      *(.dtors)   
      __DTOR_END__ = .;
   } >CodeSeg
   .stab : { *(.stab) } > CodeSeg
   .stabstr : { *(.stabstr) } > CodeSeg

   .sdata2  : 
   { 
      *(.sdata2)  
   } >CodeSeg

   /*---------------------*/
   /* Read/Write sections */
   /*---------------------*/
   .data    : 
   { 
      *(.data)    
      *(.gnu.linkonce.d.*)
   } >DPTRAM
   .sdata   : 
   { 
      *(.sdata)   
      *(.param)  
      *(.gnu.linkonce.s.*)
   } >DPTRAM
   .sbss    : 
   { 
      *(.sbss)    
   } >DPTRAM
   .bss     : { 
      *(.bss)     
   } >DPTRAM

   .heap : {
   HEAP_START = .;
   } >DataSeg

   /*-------------------*/
   /* Discarded Section */
   /*-------------------*/
   /DISCARD/ : {
     *(.comment)
/*
     *(.stab)
     *(.stabstr)
*/
     *(.fixup)
     *(.got2)    
   }
}

