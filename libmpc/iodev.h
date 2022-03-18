/**
 * @file iodev.h
 * @author Hans Dulimarta [dulimart@computer.org]
 * @brief low-level I/D device interface
 */
/*
 * $Id: iodev.h,v 2.3 2000/09/28 19:45:30 dulimart Exp $
 *
 * $Log: iodev.h,v $
 * Revision 2.3  2000/09/28 19:45:30  dulimart
 * Modified comment lines to doxygen enabled ones
 *
 * Revision 2.2  2000/08/03 22:48:04  dulimart
 * Defined new key names.
 *
 * Revision 2.1  2000/07/03 19:25:50  dulimart
 * Added KPReady() and RXReady() to test availability of data
 * from KeyPad or Receiver.
 *
 * Revision 2.0  2000/06/22 14:00:07  dulimart
 * Routines for handling receiver and keypad data.
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#ifndef MY_IODEV_H
#define MY_IODEV_H

/**
 * @var KP_DATA
 * Global variable for accessing Keypad data port
 */
extern unsigned char KP_DATA;

#define KP_ENTER    '\n'
#define KP_YES      '\n'
#define KP_NO       '0'
#define KP_PLUS     'A'
#define KP_FORWARD  'A'
#define KP_MINUS    'B'
#define KP_BACKWARD 'B'
#define KP_UP       'C'
#define KP_ON       '1'
#define KP_OFF      '0'
#define KP_SUCTION  '*'
#define KP_VALVE    '#'

#ifdef __cplusplus
extern "C" {
#endif
   /// Get a character from Keypad
   unsigned char KPGetKey();
   /// Check if any character availabel on the Keypad port
   int KPReady();
   /// Get a character from Receiver
   unsigned char RXGetKey();
   /// Check if any character availabel on the receiver port
   int RXReady();
#ifdef __cplusplus
}
#endif
#endif
