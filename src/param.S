/*
 * $Id: param.S,v 2.5 2001/02/21 16:22:22 dulimart Exp $
 *
 * Filename: param.S
 * Author: Hans Dulimarta [dulimart@computer.org]
 * Description:
 *    Table of parameter settings specific for each robot.
 *
 * $Log: param.S,v $
 * Revision 2.5  2001/02/21 16:22:22  dulimart
 * Removed L_CORR and R_CORR
 *
 * Revision 2.4  2001/02/14 15:27:49  dulimart
 * Corrected comment for max velocity
 *
 * Revision 2.3  2000/10/04 19:26:25  dulimart
 * Modified PID parameters for Joint-1
 *
 * Revision 2.2  2000/10/02 21:03:31  dulimart
 * Added parameters for PID, reenabled LOW PRESSURE threshold
 *
 * Revision 2.1  2000/05/23 17:00:52  dulimart
 * Removed low pressure threshold
 *
 * Revision 2.0  2000/04/27 12:04:21  dulimart
 * initial revision
 *
 *
 $ Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */

#define PARAM(name) \
     .global   name; \
name:
          .section  .param
PARAM(GEAR_RATIO)
          .int      166     
          .int      604
          .int      604
PARAM(PID_LIMIT)
          .int      500
PARAM(PID_Kp)
          .int      700
          .int      400
          .int      400
PARAM(PID_Ki)
          .int      4
          .int      4
          .int      4
PARAM(PID_Kd)
          .int      30
          .int      28
          .int      28
# max recommended velocity is 7900rpm  = 8.427 counts/msec
PARAM(PID_Vel)
          .int      1600
          .int      1600
          .int      1600

# max recommended accel is 131000 rad/s^2 
# = 131000 * 64 / (2*pi) count/s^2
# = 1.334 count/ms^2
PARAM(PID_Acc)
          .int      240
          .int      240
          .int      240
PARAM(LOW_P)
          .int       140
          .int       140
PARAM(HIGH_P)
          .int       450 /* for new flipping robot */
          .int       450

PARAM(PIT_INTERVAL)
          .int       250
PARAM(ZEROPOS)
          .int       256
