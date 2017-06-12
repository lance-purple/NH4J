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

#define is_rider(ptr)                                      \
    ((ptr) == &mons[PM_DEATH] || (ptr) == &mons[PM_FAMINE] \
     || (ptr) == &mons[PM_PESTILENCE])
#define is_placeholder(ptr)                             \
    ((ptr) == &mons[PM_ORC] || (ptr) == &mons[PM_GIANT] \
     || (ptr) == &mons[PM_ELF] || (ptr) == &mons[PM_HUMAN])

#define likes_lava(ptr) \
    (ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_SALAMANDER])
#define pm_invisible(ptr) \
    ((ptr) == &mons[PM_STALKER] || (ptr) == &mons[PM_BLACK_LIGHT])

/* could probably add more */
#define likes_fire(ptr)                                                  \
    ((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FLAMING_SPHERE] \
     || likes_lava(ptr))

#define touch_petrifies(ptr) \
    ((ptr) == &mons[PM_COCKATRICE] || (ptr) == &mons[PM_CHICKATRICE])

#define is_mind_flayer(ptr) \
    ((ptr) == &mons[PM_MIND_FLAYER] || (ptr) == &mons[PM_MASTER_MIND_FLAYER])

#define befriend_with_obj(ptr, obj) \
    ((obj)->oclass == FOOD_CLASS && isDomestic(ptr->monsterTypeID))

#endif /* MONDATA_H */
