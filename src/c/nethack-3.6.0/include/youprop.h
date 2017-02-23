/* NetHack 3.6	youprop.h	$NHDT-Date: 1433291407 2015/06/03 00:30:07 $  $NHDT-Branch: master $:$NHDT-Revision: 1.23 $ */
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef YOUPROP_H
#define YOUPROP_H

#include "prop.h"
#include "permonst.h"
#include "mondata.h"
#include "pm.h"

/* KMH, intrinsics patch.
 * Reorganized and rewritten for >32-bit properties.
 * HXxx refers to intrinsic bitfields while in human form.
 * EXxx refers to extrinsic bitfields from worn objects.
 * Xxx refers to any source, including polymorph forms.
 * [Post-3.4.3: HXxx now includes a FROMFORM bit to handle
 * intrinsic conferred by being polymorphed.]
 */

/*** Physical attributes ***/
#define Slow_digestion (yourIntrinsic(SLOW_DIGESTION) || yourExtrinsic(SLOW_DIGESTION)) /* KMH */

#define Half_spell_damage (yourIntrinsic(HALF_SPDAM) || yourExtrinsic(HALF_SPDAM))

/*
 * Physical damage
 *
 * Damage is NOT physical damage if (in order of priority):
 * 1. it already qualifies for some other special category
 *    for which a special resistance already exists in the game
 *    including: cold, fire, shock, acid, and magic.
 *    Note that fire is extended to include all non-acid forms of
 *    burning, even boiling water since that is already dealt with
 *    by fire resistance, and in most or all cases is caused by fire.
 * 2. it doesn't leave a mark. Marks include destruction of, or
 *    damage to, an internal organ (including the brain),
 *    lacerations, bruises, crushed body parts, bleeding.
 *
 * The following were evaluated and determined _NOT_ to be
 * susceptible to Half_physical_damage protection:
 *   Being caught in a fireball                      [fire damage]
 *   Sitting in lava                                 [lava damage]
 *   Thrown potion (acid)                            [acid damage]
 *   Splattered burning oil from thrown potion       [fire damage]
 *   Mixing water and acid                           [acid damage]
 *   Molten lava (entering or being splashed)        [lava damage]
 *   boiling water from a sink                       [fire damage]
 *   Fire traps                                      [fire damage]
 *   Scrolls of fire (confused and otherwise)        [fire damage]
 *   Alchemical explosion                            [not physical]
 *   System shock                                    [shock damage]
 *   Bag of holding explosion                        [magical]
 *   Being undead-turned by your god                 [magical]
 *   Level-drain                                     [magical]
 *   Magical explosion of a magic trap               [magical]
 *   Sitting on a throne with a bad effect           [magical]
 *   Contaminated water from a sink                  [poison/sickness]
 *   Contact-poisoned spellbooks                     [poison/sickness]
 *   Eating acidic/poisonous/mildly-old corpses      [poison/sickness]
 *   Eating a poisoned weapon while polyselfed       [poison/sickness]
 *   Engulfing a zombie or mummy (AT_ENGL in hmonas) [poison/sickness]
 *   Quaffed potions of sickness, lit oil, acid      [poison/sickness]
 *   Pyrolisks' fiery gaze                           [fire damage]
 *   Any passive attack                              [most don't qualify]
 */

#define Half_physical_damage (yourIntrinsic(HALF_PHDAM) || yourExtrinsic(HALF_PHDAM))

#define Regeneration (yourIntrinsic(REGENERATION) || yourExtrinsic(REGENERATION))

#define Energy_regeneration (yourIntrinsic(ENERGY_REGENERATION) || yourExtrinsic(ENERGY_REGENERATION))

#define Protection (yourIntrinsic(PROTECTION) || yourExtrinsic(PROTECTION))

#define Protection_from_shape_changers \
    (yourIntrinsic(PROT_FROM_SHAPE_CHANGERS) || yourExtrinsic(PROT_FROM_SHAPE_CHANGERS))

#define Polymorph (yourIntrinsic(POLYMORPH) || yourExtrinsic(POLYMORPH))

#define Polymorph_control (yourIntrinsic(POLYMORPH_CONTROL) || yourExtrinsic(POLYMORPH_CONTROL))

#define Unchanging (yourIntrinsic(UNCHANGING) || yourExtrinsic(UNCHANGING)) /* KMH */

#define Fast (yourIntrinsic(FAST) || yourExtrinsic(FAST))
#define Very_fast ((yourIntrinsic(FAST) & ~INTRINSIC)) || yourExtrinsic(FAST)

#define Reflecting (yourIntrinsic(REFLECTING) || yourExtrinsic(REFLECTING))

#define Free_action yourExtrinsic(FREE_ACTION) /* [Tom] */

#define Fixed_abil yourExtrinsic(FIXED_ABIL) /* KMH */

#define Lifesaved yourExtrinsic(LIFESAVED)

#endif /* YOUPROP_H */
