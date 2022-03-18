/*
 * $Id: mios.c,v 2.4 2000/09/26 17:14:22 dulimart Exp $
 *
 * Filename: mios.c
 *
 * Author: Hans Dulimarta, Ph.D.
 *
 * Description:
 *    This file contains routines for controlling PWM submodules
 * in the Modular I/O Subsystem (MIOS) on the MPC555 chip.
 *
 * $Log: mios.c,v $
 * Revision 2.4  2000/09/26 17:14:22  dulimart
 * Split mios_init() into mios_preinit() and mios_init()
 *
 * Revision 2.3  2000/09/12 20:46:08  dulimart
 * Put debugging message in #ifdef DEBUG
 *
 * Revision 2.2  2000/06/30 13:06:02  dulimart
 * Capped debugging messages within #ifdef DEBUG
 *
 * Revision 2.1  2000/05/23 16:49:01  dulimart
 * Used all the MPWM submodules for generating PWM signal
 *
 * Revision 2.0  2000/04/24 21:18:26  dulimart
 * Modular I/O Subsystem of MPC555
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include "mpc555.h"
#include "mios.h"

/*
 * Take precaution when changing the following contants. These selected
 * values correspond to a 9-bit digitization of PWM signal.
 */
#define PWM_DIVISOR  1
#define CLK_PRESCALER 2
#define MC_PRESCALER 256

void mios_preinit () {
   int i;

   for (i=0; i<4; i++) {
      MIOS.pwm0[i].scr |= BITSET (1,16);
      MIOS.pwm16[i].scr |= BITSET (1,16);
   }
   /* General purpose I/O */
   MIOS.io.ddr = 0xFFFF; /* set initially as digital output */
   MIOS.io.dr = 0; /* set output to low */
}

void mios_init() {
   int i;

	/* Enable MIOS Counter Prescaler (Table 15-10) */
   MIOS.cp.scr = BITSET(0,16) | CLK_PRESCALER; 

   /* Table 15-23: Set PWM divisor */
   for (i=0; i<4; i++) {
      /* All channels for PWM generation */
      MIOS.pwm0[i].scr = BITSET(1,16) | (-PWM_DIVISOR & 0xFF); 
      MIOS.pwm16[i].scr = BITSET(1,16) | (-PWM_DIVISOR & 0xFF); 
   }
   /* Setup Modulus Counter */
   MIOS.mc6.scr = (BITSET(5,16) | BITSET(6,16)) | (-MC_PRESCALER & 0xFF);
#if 0
   MIOS.ics[0].lvl = (MIOSLVL << 8) | (MIOSTM0 << 6);
   MIOS.ics[0].ier = BITSET(9,16); /* enable interrupt on module 6 */
   MIOS.ics[1].lvl = (MIOSLVL << 8) | (MIOSTM1 << 6);
   MIOS.ics[1].ier = 0;
#endif
}

void EnablePWM (unsigned char chan) {
   if (chan < 4) {
      /*MIOS.pwm0[chan].scr &= ~BITSET(4,16); set polarity to low */
      MIOS.pwm0[chan].scr |= BITSET(5,16);
   }
   else
      MIOS.pwm16[chan-16].scr |= BITSET(5,16);
}

void DisablePWM (unsigned char chan) {
   if (chan < 4)
      MIOS.pwm0[chan].scr &= ~BITSET(5,16);
   else
      MIOS.pwm16[chan-16].scr &= ~BITSET(5,16);
}

void SetPWMWidth (unsigned char chan, unsigned short width) {
   if (chan < 4) {
		if (width > MIOS.pwm0[chan].perr)
			width = MIOS.pwm0[chan].perr;
		MIOS.pwm0[chan].pulr = width;
	}
	else {
		chan -= 16;
		if (width > MIOS.pwm16[chan].perr)
			width = MIOS.pwm16[chan].perr;
		MIOS.pwm16[chan].pulr = width;
	}
}

void SetPWMDutyCycle (unsigned char chan, unsigned short width) {
	/* width value is expressed in percentage */
   if (chan < 4)
		MIOS.pwm0[chan].pulr = width * MIOS.pwm0[chan].perr / 100;
	else {
		chan -= 16;
		MIOS.pwm16[chan].pulr = width * MIOS.pwm16[chan].perr / 100;
	}
}

/* PWM period will be set to 'per' * PWM clock cycle */
void SetPWMPeriod (unsigned char chan, unsigned short per) {
   /* duty cycle is by default set to 50% */
   if (chan < 4) {
      MIOS.pwm0[chan].perr = per;
      MIOS.pwm0[chan].pulr = per / 2;
   }
   else {
      chan -= 16;
      MIOS.pwm16[chan].perr = per;
      MIOS.pwm16[chan].pulr = per / 2;
   }
}
