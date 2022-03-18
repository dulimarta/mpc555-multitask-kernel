
#ifndef D_cmf
#define D_cmf

#include "diab.h"

/*------------
 CONSTANTS
------------*/

#define MAIN_ARRAY			0
#define SHADOW_ROW			1

#define MOD_BLOCKS			14
#define PAGE_SIZE			0x40

#define DRIVER_BASE_ADDR	0x3F9800
#define MOD_BLOCK_SIZE		0x8000
#define SHADOW_SIZE			0x100
#define SHADOW1_OFFSET		0x40000


/*------------
RAM Variables
------------*/

/*
	MAIN_ARRAY			0
	SHADOW_ROW			1

	MOD_BLOCK_SIZE		0x8000

	MOD_BLOCKS			14
	PAGE_SIZE			64

	buffer size = MOD_BLOCKS * PAGE_SIZE = 896
	shadow size = 512

	Blocks Bitmask
 	--------------
		CMF A 							|CMF B							|CMF C 						   |CMF D
Bit		00 	01 	02 	03 	04 	05 	06 	07 	08 	09 	10 	11 	12 	13 	14 	15 	16 	17 	18 	19 	20 	21 	22 	23	24 	25 	26 	27 	28 	29 	30 	31
Block	0	1 	2 	3	4 	5 	6	7 	0 	1	2 	3	4 	5 	6	7 	0	1	2	3	4	5	6	7	0	1	2	3	4	5	6   7

	Each block is 32K bytes.  A page from each block is programmed at the same time.  Blocks can be erased independantly.

*/
#define BLOCK_A0 0x80000000	// CMF A - address 00000 - 07FFF
#define BLOCK_A1 0x40000000	// CMF A - address 08000 - 0FFFF
#define BLOCK_A2 0x20000000	// CMF A - address 10000 - 17FFF
#define BLOCK_A3 0x10000000	// CMF A - address 18000 - 1FFFF
#define BLOCK_A4 0x08000000	// CMF A - address 20000 - 27FFF
#define BLOCK_A5 0x04000000	// CMF A - address 28000 - 2FFFF
#define BLOCK_A6 0x02000000	// CMF A - address 30000 - 37FFF
#define BLOCK_A7 0x01000000	// CMF A - address 38000 - 3FFFF

#define BLOCK_B0 0x00800000	// CMF B - address 40000 - 47FFF
#define BLOCK_B1 0x00400000	// CMF B - address 48000 - 4FFFF
#define BLOCK_B2 0x00200000	// CMF B - address 50000 - 57FFF
#define BLOCK_B3 0x00100000	// CMF B - address 58000 - 5FFFF
#define BLOCK_B4 0x00080000	// CMF B - address 60000 - 67FFF
#define BLOCK_B5 0x00040000	// CMF B - address 68000 - 6FFFF

#define BLOCK_ALL 0xFFFC0000	// CMF A&B - address 00000 - 6FFFF
#define BLOCK_EDALL 0x3FFC0000	// CMF A&B - address 10000 - 6FFFF

extern volatile unsigned int 	blockmask;

extern unsigned char PARALLEL_TRD_ARRAY[];
extern unsigned int 	*TABLE;

//extern UINT32	buffer[MOD_BLOCKS*PAGE_SIZE/4];     // total MOD_BLOCKS pages
//extern UINT32	shadow[512/4];                      // used to save shadow information

/*------------
 Functions
------------*/

void CMF_Menu1();
void CMF_Menu2();


#endif // D_cmf
