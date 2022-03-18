// ******************************************************************************
//                                                    Copyright (c) Motorola 1999
// File Name        : interface.h
//
// Engineer         : Sung Kim
//
// Location         : SCMC
//
// Date Created     : 28. Oct. 1999
//
// Current Revision : 0.2
//
// Notes            : None
//
// *******************************************************************************
// Motorola reserves the right to make changes without further notice to any
// product herein to improve reliability, function or design. Motorola does not
// assume any liability arising out of the application or use of any product,
// circuit, or software described herein; neither does it convey any license
// under its patent rights nor the rights of others. Motorola products are not
// designed, intended, or authorized for use as components in systems  intended for
// surgical implant into the body, or other applications intended to support life,
// or for any other application in which the failure of the  Motorola product
// could create a situation where personal injury or death may occur. Should
// Buyer purchase or use Motorola products for any such unintended or
// unauthorized application, Buyer shall idemnify and hold Motorola and its
// officers, employees, subsidiaries, affiliates, and distributors harmless
// against all claims costs, damages, and expenses, and reasonable attorney fees
// arising out of, directly or indirectly, any claim of personal injury or death
// associated with such unintended or unauthorized use, even if such claim alleges
// that Motorola was negligent regarding the design or manufacture of the part.
// Motorola and the Motorola logo* are registered trademarks of Motorola Ltd.
// ******************************************************************************/
//
// *************************************** CHANGES *****************************
//
//
//  0.1	10.28.1999		Sung Kim		Initial Revision
//
//  0.2 11.24.1999		Sung Kim		Update to unsigned int interface.
//
// ******************************************************************************/

#ifndef	_INTERFACE_H_
#define _INTERFACE_H_

//--------- Macro Codes from CmfParallelInit only --------------------------//
#define CMF_PROGRAM 					0
#define CMF_ERASE   					1
#define CMF_CENSOR_SET  				2 
#define CMF_CENSOR_CLEAR  				3

//--------- Censorbit definitions, can be used for censor set functions ------//
#define CMF_CLEAR_CENSOR 				0
#define CMF_NO_CENSOR_01 				1
#define CMF_NO_CENSOR_10 				2
#define CMF_INFORMATION_CENSOR 			3

//--------- Return Code from parallel TRD --------------------------//
#define CMF_OK              			0
#define CMF_PULSE_PENDING   			0x10
#define CMF_VERIFY_PENDING  			0x11
#define CMF_ERROR_PARAMETER_FUNCTION    0x81
#define CMF_ERROR_PARAMETER_BLOCKS  	0x82
#define CMF_ERROR_PARAMETER_OFFSET  	0x83
#define CMF_ERROR_PARAMETER_MODULE  	0x84
#define CMF_ERROR_PARAMETER_VALUE   	0x85
#define CMF_ERROR_SEQUENCE  			0x86
#define CMF_ERROR_NO_VPP    			0x87
#define CMF_ERROR_PROGRAM   			0x88
#define CMF_ERROR_ERASE     			0x89
#define CMF_ERROR_CENSOR    			0x90
#define CMF_ERROR_PROTECTED 			0x91
#define CMF_ERROR_BAD_HVS   			0x92
#define CMF_ERROR_NOT_IMPLEMENTED 		0x93
#define CMF_ERROR_BAD_DESCRIPTOR  		0x94

#define	PTRD_ERROR_INPUT_BUFFER_SIZE	0xC1
#define	PTRD_ERROR_NOT_IMPLEMENTED	    0xC2
#define PTRD_ERROR_ADDRESS_RANGE        0xC3
#define PTRD_ERROR_VERIFY               0xC4
#define PTRD_BLANK_CHECK_NOT_BLANK      0xC5
#define PTRD_BACKUP_CLOCK_ACTIVE        0xC6
#define PTRD_ERROR_ARRAY_BASE           0xC7
#define PTRD_ERROR_NOT_UNRESTRICTED_SPACE 0xC8
#define PTRD_ERROR_FIC                  0xC9
#define PTRD_ERROR_NO_RETURN            0xCA
#define PTRD_ERROR_CLEARED_CENSOR       0xCB
#define PTRD_ERROR_INFORMATION_CENSOR   0xCC
#define PTRD_BLANK_CHECK_BLANK          CMF_OK

//--------- Message Buffer Index Definations --------------------//
#define	INDEX_CallinGate				0
#define	INDEX_pInputBuffer				1
#define	INDEX_pOutputBuffer				2
#define	INDEX_wStackPointer				3
#define	INDEX_wUserModeFlag				4

//--------- Command Index Definations --------------------------//
#define	COMMAND_CMFPARALLELINIT			1
#define	COMMAND_CMFPARALLELPROGRAMSTART	2
#define	COMMAND_CMFPARALLELERASESTART	3
#define	COMMAND_CMFPARALLELCENSORSTART	4
#define	COMMAND_CMFPARALLELCONTINUE		5
#define	COMMAND_CMFPARALLELEXIT			6
#define	COMMAND_FLASHINIT   			7
#define	COMMAND_PARALLELERASE       	8
#define	COMMAND_PARALLELPROGRAM			9
#define	COMMAND_SERIALPROGRAM			10
#define	COMMAND_BLANKCHECK		        11
#define	COMMAND_PARALLELVERIFY			12
#define	COMMAND_SERIALVERIFY			13
#define	COMMAND_GETREVISION				14
#define	COMMAND_GETFLASHTYPE			15
#define	COMMAND_CHANGECENSOR  			16
#define	COMMAND_CHECKSUM     			17
#define	COMMAND_GETDESCRIPTOR			18

//--------- Input Buffer Definations of all commands ---------------------//
typedef struct {
	unsigned int	size;
	unsigned int	command;
}InputStruct_Default;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	function;
}InputStruct_CmfParallelInit;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	enabledBlocks;
	unsigned int 	*data;
	unsigned int 	offset;
	unsigned int 	shadow;
} InputStruct_CmfParallelProgramStart;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	enabledBlocks;
} InputStruct_CmfParallelEraseStart;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	enabledModule;
	unsigned int 	value;
} InputStruct_CmfParallelCensorStart;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_CmfParallelContinue;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_CmfParallelExit;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_GetRevision;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_GetFlashType;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_FlashInit;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	enabledBlocks;
} InputStruct_ParallelErase;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int*	src;
	unsigned int 	enabledBlocks;
	unsigned int  offset;
	unsigned int  pagesetNum;
	unsigned int  shadow; 
} InputStruct_ParallelProgram;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int*	src;
	unsigned int 	dest;
	unsigned int  dataSize;
	unsigned int  shadow; 
} InputStruct_SerialProgram;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int 	dest;
	unsigned int  dataSize;
	unsigned int  shadow; 
} InputStruct_BlankCheck;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int*	src;
	unsigned int 	enabledBlocks;
	unsigned int  pagesetNum;
	unsigned int  offset;
	unsigned int  shadow; 
} InputStruct_ParallelVerify;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int*	src;
	unsigned int 	dest;
	unsigned int  dataSize;
	unsigned int  shadow; 
} InputStruct_SerialVerify;

typedef struct {
	unsigned int	size;
	unsigned int	command;
} InputStruct_GetDescriptor;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int  startAddress;
	unsigned int  dataSize;
	unsigned int  shadow;
} InputStruct_CheckSum;

typedef struct {
	unsigned int	size;
	unsigned int	command;
	unsigned int  enabledModule;
	unsigned int  value;
} InputStruct_ChangeCensor;


//--------- Output Buffer Definations of all commands ---------------------//
typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_Default;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelInit;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelProgramStart;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelEraseStart;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelCensorStart;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelContinue;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_CmfParallelExit;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned char *	revision;
} OutputStruct_GetRevision;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned char *	flashType;
} OutputStruct_GetFlashType;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_FlashInit;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_ParallelErase;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_ParallelProgram;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_SerialProgram;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned int  failedDestAddress;
	unsigned int  failedDestData;
} OutputStruct_BlankCheck;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned int  failedDestAddress;
	unsigned int  failedDestData;
	unsigned int  failedSrcData;
} OutputStruct_ParallelVerify;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned int  failedDestAddress;
	unsigned int  failedDestData;
	unsigned int  failedSrcData;
} OutputStruct_SerialVerify;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	tCMF_DESCRIPTOR*  descriptor;
} OutputStruct_GetDescriptor;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
	unsigned int   checkedSum;
} OutputStruct_CheckSum;

typedef struct {
	unsigned int	size;
	unsigned int	returnedValue;
} OutputStruct_ChangeCensor;

#endif // _INTERFACE_H_
