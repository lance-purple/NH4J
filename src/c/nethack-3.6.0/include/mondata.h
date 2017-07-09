/* NetHack 3.6	mondata.h	$NHDT-Date: 1432512776 2015/05/25 00:12:56 $  $NHDT-Branch: master $:$NHDT-Revision: 1.26 $ */
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONDATA_H
#define MONDATA_H

#define resists_fire(mon) (((mon)->mintrinsics & MR_FIRE) != 0)
#define resists_cold(mon) (((mon)->mintrinsics & MR_COLD) != 0)
#define resists_sleep(mon) (((mon)->mintrinsics & MR_SLEEP) != 0)
#define resists_disint(mon) (((mon)->mintrinsics & MR_DISINT) != 0)
#define resists_elec(mon) (((mon)->mintrinsics & MR_ELEC) != 0)
#define resists_poison(mon) (((mon)->mintrinsics & MR_POISON) != 0)
#define resists_acid(mon) (((mon)->mintrinsics & MR_ACID) != 0)
#define resists_ston(mon) (((mon)->mintrinsics & MR_STONE) != 0)

#endif /* MONDATA_H */
