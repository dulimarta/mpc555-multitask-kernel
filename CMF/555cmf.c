/* ===========================================================================
    CMF Programming functions for PB-555-ED board utilities

    Version:        1.0
    Author:         Dusty Nidey, Axiom Mfg.
	Compiler:		Diab Data Compiler version 4.2

============================================================================*/

#include "cmf_parallel_descriptor.h"
#include "interface.h"
#include "555cmf.h"
#include "io.h"
#include "stdlib.h"
#include "serial.h"

unsigned int 	blockmask;
unsigned int 	*TABLE;

unsigned int	PageOffset ;

#define OUTBUFSIZE	64
unsigned char	outbuf[OUTBUFSIZE];
unsigned char	inbuf[4*OUTBUFSIZE];

void (* RomFunction)();				// this is a function pointer that holds the starting address in rom to execute


#define	MACRO_DEFPOINTER(FUNC_NAME)	\
	InputStruct_ ## FUNC_NAME * inParamsBuf = (InputStruct_ ## FUNC_NAME *) TABLE[INDEX_pInputBuffer];	\
	OutputStruct_ ## FUNC_NAME * outp = (OutputStruct_ ## FUNC_NAME *) TABLE[INDEX_pOutputBuffer];		\
	inParamsBuf->size = sizeof(InputStruct_ ## FUNC_NAME) - sizeof(unsigned int);


void TableInit(unsigned int driver_addr){
	TABLE = (unsigned int*)driver_addr;   /* assign ptr to driver address */

    TABLE[INDEX_CallinGate]+=(unsigned int)driver_addr; 	/* update the zero-based function offsets */
    TABLE[INDEX_pInputBuffer]+=(unsigned int)driver_addr;
    TABLE[INDEX_pOutputBuffer]+=(unsigned int)driver_addr;
}

unsigned int failmessage(char *msg, unsigned int code){
	fputs ("\n", STDERR);
	fputs (msg, STDERR);
	fputs (" Failed.  Status Code ", STDERR);
	my_fprintf (STDERR, "%x", code);
	fputs ("  ", STDERR);
	switch(code){
		case CMF_ERROR_NO_VPP:
			fputs ("NO VPP - check jumper", STDERR);
			break;
		case PTRD_ERROR_VERIFY:
			fputs ("Verify Failed", STDERR);
			break;
	}
	return code;
}

void Callout(void){

	// push all
			asm("	stwu    %r1,-0x78(%r1)");	// Backup register r2-r31 to Stack
			asm("	stmw    %r2,0(%r1)	");
			asm("	stwu    %r1,-0x4(%r1)"); // Backup register r0 to Stack
			asm("	stw	  %r0,0(%r1)	");

	// call
	((unsigned int(*)(void))(TABLE[INDEX_CallinGate])) ();
	// pop all
			asm("	lwz     %r0,0(%r1)	"); // Restore register r0 from Stack
			asm("	addi    %r1,%r1, 0x04");
			asm("	lmw     %r2,0(%r1)	"); // Restore register r2-r31 from Stack
			asm("	addi    %r1,%r1, 0x78");
}

tCMF_DESCRIPTOR* GetDescriptor(void){
	MACRO_DEFPOINTER(GetDescriptor)

	// fill out buffer
	inParamsBuf->size = sizeof(InputStruct_GetDescriptor) - sizeof(unsigned int);
	inParamsBuf->command = COMMAND_GETDESCRIPTOR;

	Callout();	// call out

	// return
	if (outp->returnedValue==CMF_OK) {
		return outp->descriptor;
	}
	else
		return NULL;
}

unsigned int ParallelErase( unsigned int enabledBlocks ){

	MACRO_DEFPOINTER(ParallelErase)

	// fill out buffer
	inParamsBuf->size = sizeof(InputStruct_ParallelErase) - sizeof(unsigned int);
	inParamsBuf->command = COMMAND_PARALLELERASE;
	inParamsBuf->enabledBlocks = enabledBlocks;

	Callout();	// call out

	return outp->returnedValue;
}

//  Erase all CMF memory
unsigned int CMF_EraseAll(){
	unsigned int 	status;
	//unsigned int 	offset;
	//unsigned int 	vfail_addr, vfail_data, vfail_srcdata;
	//unsigned int 	modBaseAddress = 0;

	// Erase all blocks including shadow
	fputs ("\nErasing Flash...", STDERR);
	status = ParallelErase (blockmask);
	if (status != CMF_OK){
		return (failmessage("Erase",status));
	}
	return CMF_OK;
}


unsigned int ParallelProgram( unsigned int *src, unsigned int enabledBlocks, unsigned int offset, unsigned int pagesetNum, unsigned int shadow ){

	MACRO_DEFPOINTER(ParallelProgram)

	// fill out buffer
	inParamsBuf->size = sizeof(InputStruct_ParallelProgram) - sizeof(unsigned int);
	inParamsBuf->command = COMMAND_PARALLELPROGRAM;
	inParamsBuf->src = src;
	inParamsBuf->enabledBlocks = enabledBlocks;
	inParamsBuf->offset = offset;
	inParamsBuf->pagesetNum = pagesetNum;
	inParamsBuf->shadow = shadow;

	Callout();	// call out

	return outp->returnedValue;
}

// Blockmask and Page calculation
// This will take the offset address and set the blockmask for parallel programming so that only a single block is programmed.
// This will also take the offset address and set the page offset for the block to be programmed in parallel mode
// The blockmask is returned and the global PageOffset is set to this blocks offset
unsigned int CMF_BlockCalc(unsigned int offset){
	unsigned int checkval;
	unsigned int blockval;

	blockval = BLOCK_A0;
	checkval = 0x0;

	while(checkval < 0x70000){
		if(offset < (checkval + 0x8000)){
			PageOffset = offset - checkval;	// calculate this blocks page address
			return(blockval);
		}
		checkval += 0x8000;
		blockval >>= 1;
	}
	return(0);	// here if an invalid offset value
}

//----------------------------------------------------------------------------
unsigned int CMF_ProgramPage(unsigned int *bfr, unsigned int offset){
	unsigned int 	status;
	unsigned char	count1;
	unsigned int *ptr;

	if(offset < 0x10000){
		return(CMF_ERROR_PROTECTED);	// The memory you are trying to program is in protected space and cannot be programed with this software.  See the board documention for more information.
	}
	blockmask = CMF_BlockCalc(offset);
	if(blockmask > 0){
		// buffer = source data address
		// blockmask = mask of blocks that are enabled to be parallel programmed (buffer must have data at each page)
		// offset = offset in this block to be programmed
		// 1 = number of pages to be written to a block
		// 0 = normal address space (1=shadow space)

		status = ParallelProgram (bfr,blockmask,PageOffset,1,0);
		if(status != CMF_OK)	return(status);

		// verify we can read back what was programmed
		ptr = (unsigned int *) offset;
		for(count1=0; count1<0x10; count1++){
			if(bfr[count1] != ptr[count1])	return(PTRD_ERROR_VERIFY);
		}
	}

	return(CMF_OK);
}
//---------------------------------------------------------------------------
unsigned int CMF_ProgramFile(){
	int count, count1, bcount, ipos;
	int addrh, addrl;
	long address, cur_address, new_address;
	char firstflag;
	unsigned int 	status;

	fputs ("\n\n Send Motorola S19 File Now...", STDERR);
	firstflag = 0;

	// clear output buffer
	for(count1=0; count1 < OUTBUFSIZE; count1++)	outbuf[count1] = 0xff;

	count = 0; addrl = 0; addrh = 0; cur_address = 0; address = 0;


	for( ; ;){

		gets(inbuf);		// get a string from the serial port

		if((inbuf[0] != 'S') && (inbuf[0] != 's')){
			fputs ("\n\n ERROR: Invalid S-Record Format.\n\n", STDERR);
			return(1);
		}
		if(inbuf[1] == '3'){
			bcount = StrHexNum(&inbuf[2], 1);	// get byte count
			bcount -=5;	// remove address and checksum from the count
			bcount *=2;	// adjust for ascii
			addrh = StrHexNum(&inbuf[4], 2);	// get high address word
			addrl = StrHexNum(&inbuf[8], 2);	// get low address word
			new_address = (addrh * 0x10000) + addrl;

			if(firstflag == 0){
				cur_address = new_address;
				address = new_address;
				firstflag = 1;
			}

			ipos = 0;	// start of input data
			do{
				if(count > 0 && new_address > cur_address){ // if there's a gap in the s record
					outbuf[count] = 0xFF;
				}
				else{
					outbuf[count] = StrHexNum(&inbuf[ipos+12], 1);
					ipos+=2;
				}
				++cur_address;
				++count;
				if(count > 63){	// if end of buffer reached, program it
					status = CMF_ProgramPage((unsigned int *) outbuf, address);	// program this page
					if (status != CMF_OK){
						failmessage("Programming",status);
						usleep(100000);	// delay in mil sec.
						//ClearSerial();
						return(status);
					}

//                    if(new_address >= (cur_address + 0xFF)){    // if large gap, skip to next block in s-record
                    if(new_address >= (address + 0xFF)){    // if large gap, skip to next block in s-record
						cur_address = new_address;
					}
					address = cur_address;
					count=0;
					for(count1=0; count1 < OUTBUFSIZE; count1++)	outbuf[count1] = 0xff;	// clear output buffer
				}
			}while(ipos < bcount);
		}

		//		scrchar = outputscroll(scrchar);	// displays a rotating slash character
		putchar('.');

		if(inbuf[1] == '7' || inbuf[1] == '9'){	// end of s19 record
			if(count > 0){
				status = CMF_ProgramPage((unsigned int *) outbuf, address);	// program this page
				if (status != CMF_OK)
 					failmessage("Programming",status);
			}
			usleep(50000);	// delay in mil sec.
			//ClearSerial();
			return(0);
		}
	}
}

//----------------------------------------------------------------------------
// CMF Programming menu for the 555-ED single-chip board

#define TRD_ARRAY_SIZE	0x21F2
//unsigned char TRD_RAM_ARRAY[TRD_ARRAY_SIZE];    // global array to hold CMF programming code
unsigned char *TRD_RAM_ARRAY;    // global array to hold CMF programming code

int main(){
	unsigned char choice;
	unsigned short	count1;

    // for this version, we must move the programming array to internal RAM
    // because the CMFI memory device cannot program itself from the same memory device
	fputs("\n\nInitializing and moving Parallel TRD Array...", STDERR);

    TRD_RAM_ARRAY = (unsigned char *) 0x3FC000;           // point to the bottom of internal SRAM B array
    for(count1=0; count1 < TRD_ARRAY_SIZE; count1++){
		TRD_RAM_ARRAY[count1] = PARALLEL_TRD_ARRAY[count1];
	}
	TableInit( (unsigned int) TRD_RAM_ARRAY );

	for( ; ;){
		fputs("555-ED single-chip board - CMF Flash Programming\n\n", STDERR);
		fputs("\n E. Erase CMF Flash", STDERR);
		fputs("\n P. Program CMF Flash from file", STDERR);
		fputs("\n T. Test run your program", STDERR);
		fputs("\n Q. Quit to Menu", STDERR);

		fputs("\n\n Select: ", STDERR);
		choice = tolower(getchar());
		fputs("\n\n", STDERR);
		switch(choice){
			case 'e':
                blockmask = BLOCK_EDALL; // select all blocks that are allowed by this software
                CMF_EraseAll(); // Erase CMF memory
				fputs("\n\n --- If finished remove VPP_EN to protect the board. ---\n", STDERR);
				break;

			case 'p':
				CMF_ProgramFile();
				fputs("\n\n --- If finished programming, remove VPP_EN to protect the board. ---\n", STDERR);
				fputs(" To boot your program, remove the PROG_EN jumper.\n", STDERR);
				break;

			case 't':
				RomFunction = (void (*)()) 0x10100;		// address of user program's reset vector
				RomFunction();							// go execute their code
				break;

            case 'q':
				return 0;
		}
	}
   return 0;
}
