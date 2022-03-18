; ===========================================================================
;	init.s
;	------
;	Axiom Utilities for the Motorola MPC555 Development Board
; Initialization file to boot from flash
;
;	Assembler:		Diab Data Assembler/Compiler version 4.2
;
; (C) Copyright 1999 Axiom Manufacturing.  All rights reserved.
; ===========================================================================
	.file	"INIT.S"
	PSECT
	XDEF	start
  .align	2

  .globl    __start
	.globl		_hwreset
	.globl gInterruptVectorTable
	.globl gInterruptVectorTableEnd


;------------------------------------------------------------------------------
	.ORG 0x0000		;	location of the device to be programed with this code

gInterruptVectorTable:
;		.asciz	"Axiom Manufacturing Monitor for the MPC555"


;------------------------------------------------------------------------------
	.ORG 0x100
;	0x0100 System Reset
;	This is the first instruction to be executed after the hardware power-on reset.

		b	__start			; use this when when debugging under the bdm or booting in single-chip (CMF)
;		b	(__start+0x400000)	; use this for the onboard flash version

;------------------------------------------------------------------------------
	.ORG 0x200
;	0x0200 Machine Check
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x200

;------------------------------------------------------------------------------
	.ORG 0X300
;	0x0300 Data Storage
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x300

;------------------------------------------------------------------------------
	.ORG 0X400
;	0x0400 Instruction Storage
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x400

;------------------------------------------------------------------------------
	.ORG 0X500
;	0x0500 External Interrupt
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x500

;------------------------------------------------------------------------------
	.ORG 0X600
;	0x0600 Alignment
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x600

;------------------------------------------------------------------------------
	.ORG 0X700
;	0x0700 Program
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x700

;------------------------------------------------------------------------------
	.ORG 0X800
;	0x0800 Floating Point Unavailable
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x800

;------------------------------------------------------------------------------
	.ORG 0X900
;	0x0900 Decrementer Interrupt
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x900

;------------------------------------------------------------------------------
	.ORG 0XC00
;	0x0C00 System Call
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0xc00

;------------------------------------------------------------------------------
	.ORG 0XD00
;	0x0D00 Trace
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0xd00

;------------------------------------------------------------------------------
	.ORG 0XE00
;	0x0E00 FP Assist
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0xe00

;------------------------------------------------------------------------------
	.ORG 0XF00
;	0x0F00 PPC740 and PPC750: Performance Monitor
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0xf00

;------------------------------------------------------------------------------
	.ORG 0X1000
;	0x1000 	MPC8xx and MPC505:	Software Emulation
;			PPC603e:			Instruction TLB Miss
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1000

;------------------------------------------------------------------------------
	.ORG 0X1100
;	0x1100 	MPC8xx:				Instruction TLB Miss
;			PPC603e:			Data Load TLB Miss
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1100

;------------------------------------------------------------------------------
	.ORG 0X1200
;	0x1200	MPC8xx:				Data TLB Miss
;			PPC603e:			Data Store TLB Miss
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1200

;------------------------------------------------------------------------------
	.ORG 0X1300
;	0x1300	MPC8xx:				Instruction TLB Error
;			PPC7xx and PPC603e:	Instruction address breakpoint
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1300

;------------------------------------------------------------------------------
	.ORG 0X1400
;	0x1400	MPC8xx:				Data TLB Error
;			PPC7xx and PPC603e:	System management
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1400

;------------------------------------------------------------------------------
	.ORG 0X1c00
;	0x1C00 MPC8xx and MPC505:	Data breakpoint
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1c00

;------------------------------------------------------------------------------
	.ORG 0X1d00
;	0x1D00 MPC8xx and MPC505:	Instruction breakpoint
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1d00

;------------------------------------------------------------------------------
	.ORG 0X1e00
;	0x1E00 MPC8xx and MPC505:	Peripheral breakpoint
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1e00

;------------------------------------------------------------------------------
	.ORG 0X1f00
;	0x1F00 MPC8xx and MPC505:	Non-maskable development port
;	Override the behavior by over-writing the handler code in RAM.
   b __usercode+0x1f00


;------------------------------------------------------------------------------
  .ORG 0x10000
__usercode:

;------------------------------------------------------------------------------
	.ORG 0x2000

__start:
start:
;-----------
_hwreset:
;-----------

; If using Hardware floating point enable the FP bit in the  MSR
; 	mfmsr r0
; 	ori  r0,r0,0x00002000
; 	mtmsr r0

        addis           r11,r0,__SP_INIT@ha     # Initialize stack pointer r1 to
        addi            r1,r11,__SP_INIT@l      # value in linker command file.
        addis           r13,r0,_SDA_BASE_@ha    # Initialize r13 to sdata base
        addi            r13,r13,_SDA_BASE_@l    # (provided by linker).
        addis           r2,r0,_SDA2_BASE_@ha    # Initialize r2 to sdata2 base
        addi            r2,r2,_SDA2_BASE_@l     # (provided by linker).
        addi            r0,r0,0                 # Clear r0.
        stwu            r0,-64(r1)		# Terminate stack.


				LIS     R0,0XFFF0 		; IMMR
				ori     r0,r0,0x800
				mtspr	638,r0

				li      r0,0x0000			; BBCMCR
				mtspr	560,r0

				bl		EPEEtest	; if Vpp jumper not on, go execute user code
				bl		inithw		; initialize hardware registers


        bl              __init_main     # Finishes initialization (copies .data
                                        # ROM to RAM, clears .bss), then calls
                                        # example main(), which calls exit(),
                                        # which halts.

#------------------------------------------------------------- .init section --
        .section        .init$00,2,C
        .globl          __init
        .align          2
__init:                                 # Entry to __init, called by
        mfspr           r0,8            # __init_main called above.
        stwu            r1,-64(r1)
        stw             r0,68(r1)

        # Linker places .init sections from other modules, containing
        # calls to initialize global objects, here.

        .section        .init$99,2,C
        lwz             r0,68(r1)       # Return from __init.
        addi            r1,r1,64
        mtspr           8,r0
        blr

#------------------------------------------------------------- .fini section --
        .section        .fini$00,2,C
        .globl          __fini
        .align          2
__fini:                                 # Entry to __fini, called by exit().
        mfspr           r0,8
        stwu            r1,-64(r1)
        stw             r0,68(r1)

        # Linker places .fini sections from other modules, containing
        # calls to destroy global objects, here.

        .section        .fini$99,2,C
        lwz             r0,68(r1)       # Return from __fini.
        addi            r1,r1,64
        mtspr           8,r0
        blr
