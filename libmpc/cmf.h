/* 
 * $Id: cmf.h,v 2.2 2000/10/02 20:14:18 dulimart Exp $
 *
 * Author: Hans Dulimarta, dulimart@computer.org
 * Description:
 *    This module defines the memory map of CMF (CDR MoneT Flash) 
 * EEPROM modules.
 *
 * $Log: cmf.h,v $
 * Revision 2.2  2000/10/02 20:14:18  dulimart
 * Changed header comment
 *
 * Revision 2.1  2000/08/02 15:41:34  dulimart
 * Added routines for CMF manipulation
 *
 * Revision 2.0  2000/04/27 10:09:04  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
typedef struct {
	unsigned int mcr; /* module configuration register */
	unsigned int tst; /* test register */
	unsigned int ctl; /* high voltage control register */
} CMF_map;

void cmf_write_page (CMF_map *cmf, short block, int offset, char *data);

extern CMF_map CMF_A, CMF_B;
