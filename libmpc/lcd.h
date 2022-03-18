/**
 * @file lcd.h
 *
 * @author Hans Dulimarta  [dulimart@computer.org]
 *
 * @brief 
 * LCD interface on the CME-555 board.
 */
/*
 * $Id: lcd.h,v 2.3 2000/09/28 19:48:23 dulimart Exp $
 *
 * $Log: lcd.h,v $
 * Revision 2.3  2000/09/28 19:48:23  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.2  2000/06/30 13:01:04  dulimart
 * Renamed LCDinit to lcd_init
 *
 * Revision 2.1  2000/05/23 16:44:22  dulimart
 * Added lcd_init
 *
 * Revision 2.0  2000/04/24 21:12:37  dulimart
 * initial revision
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef LCD_H
#define LCD_H
/**
 * @var LCD_CMD
 * Global variable for accessing the LCD command port
 */
extern unsigned char LCD_CMD;
/**
 * @var LCD_DATA
 * Global variable for accessing the LCD data port
 */
extern unsigned char LCD_DATA;

#ifdef __cplusplus
extern "C" {
#endif
   /// LCD initialization routine
   void lcd_init(void);

   //void LCDTest(void);

   /// Write a character to the LCD
   void LCDputch(unsigned char);

   /// Get the character in the LCD current position
   unsigned char LCDgetch(void);

   /// Send a command byte to the LCD controller
   void LCDCommand (unsigned char);
#ifdef __cplusplus
}
#endif
#endif
