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

/*** Troubles ***/

/*** Vision and senses ***/

#define HClairvoyant u.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant u.uprops[CLAIRVOYANT].extrinsic
#define Clairvoyant ((yourIntrinsic(CLAIRVOYANT) || yourExtrinsic(CLAIRVOYANT)) && !youAreBlockedFrom(CLAIRVOYANT))

#define HInfravision u.uprops[INFRAVISION].intrinsic
#define EInfravision u.uprops[INFRAVISION].extrinsic
#define Infravision (yourIntrinsic(INFRAVISION) || yourExtrinsic(INFRAVISION))

#define HDetect_monsters u.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters u.uprops[DETECT_MONSTERS].extrinsic
#define Detect_monsters (yourIntrinsic(DETECT_MONSTERS) || yourExtrinsic(DETECT_MONSTERS))

/*** Appearance and behavior ***/
#define Adornment u.uprops[ADORNED].extrinsic

#define HInvis u.uprops[INVIS].intrinsic
#define EInvis u.uprops[INVIS].extrinsic
#define Invis ((yourIntrinsic(INVIS) || yourExtrinsic(INVIS)) && !youAreBlockedFrom(INVIS))
#define Invisible (Invis && !youCanSeeInvisible())
/* Note: invisibility also hides inventory and steed */

#define EDisplaced u.uprops[DISPLACED].extrinsic
#define Displaced EDisplaced

#define HStealth u.uprops[STEALTH].intrinsic
#define EStealth u.uprops[STEALTH].extrinsic
#define Stealth ((yourIntrinsic(STEALTH) || yourExtrinsic(STEALTH)) && !youAreBlockedFrom(STEALTH))

#define HAggravate_monster u.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster u.uprops[AGGRAVATE_MONSTER].extrinsic
#define Aggravate_monster (yourIntrinsic(AGGRAVATE_MONSTER) || yourExtrinsic(AGGRAVATE_MONSTER))

#define HConflict u.uprops[CONFLICT].intrinsic
#define EConflict u.uprops[CONFLICT].extrinsic
#define Conflict (yourIntrinsic(CONFLICT) || yourExtrinsic(CONFLICT))

/*** Transportation ***/
#define HJumping u.uprops[JUMPING].intrinsic
#define EJumping u.uprops[JUMPING].extrinsic
#define Jumping (yourIntrinsic(JUMPING) || yourExtrinsic(JUMPING))

#define HTeleportation u.uprops[TELEPORT].intrinsic
#define ETeleportation u.uprops[TELEPORT].extrinsic
#define Teleportation (yourIntrinsic(TELEPORT) || yourExtrinsic(TELEPORT))

#define HTeleport_control u.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control u.uprops[TELEPORT_CONTROL].extrinsic
#define Teleport_control (yourIntrinsic(TELEPORT_CONTROL) || yourExtrinsic(TELEPORT_CONTROL))

#define HLevitation u.uprops[LEVITATION].intrinsic
#define ELevitation u.uprops[LEVITATION].extrinsic
#define Levitation ((yourIntrinsic(LEVITATION) || yourExtrinsic(LEVITATION)) && !youAreBlockedFrom(LEVITATION))
/* Can't touch surface, can't go under water; overrides all others */
#define Lev_at_will                                                    \
    (((yourIntrinsic(LEVITATION) & I_SPECIAL) != 0L || (yourExtrinsic(LEVITATION) & W_ARTI) != 0L) \
     && (yourIntrinsic(LEVITATION) & ~(I_SPECIAL | TIMEOUT)) == 0L                   \
     && (ELevitation & ~W_ARTI) == 0L)

#define HFlying u.uprops[FLYING].intrinsic
#define EFlying u.uprops[FLYING].extrinsic
#define Flying                                                      \
    ((yourIntrinsic(FLYING) || yourExtrinsic(FLYING) || (u.usteed && is_flyer(u.usteed->data))) \
     && !youAreBlockedFrom(FLYING))
/* May touch surface; does not override any others */

extern boolean canYouWalkOnWater();

#define HSwimming u.uprops[SWIMMING].intrinsic
#define ESwimming u.uprops[SWIMMING].extrinsic /* [Tom] */
#define Swimming \
    (yourIntrinsic(SWIMMING) || yourExtrinsic(SWIMMING) || (u.usteed && is_swimmer(u.usteed->data)))
/* Get wet, don't go under water unless if amphibious */

#define HMagical_breathing u.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing u.uprops[MAGICAL_BREATHING].extrinsic
#define Amphibious \
    (yourIntrinsic(MAGICAL_BREATHING) || yourExtrinsic(MAGICAL_BREATHING) || amphibious(youmonst.data))
/* Get wet, may go under surface */

#define Breathless \
    (yourIntrinsic(MAGICAL_BREATHING) || yourExtrinsic(MAGICAL_BREATHING) || breathless(youmonst.data))

#define HPasses_walls u.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls u.uprops[PASSES_WALLS].extrinsic
#define Passes_walls (yourIntrinsic(PASSES_WALLS) || yourExtrinsic(PASSES_WALLS))

/*** Physical attributes ***/
#define HSlow_digestion u.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion u.uprops[SLOW_DIGESTION].extrinsic
#define Slow_digestion (yourIntrinsic(SLOW_DIGESTION) || yourExtrinsic(SLOW_DIGESTION)) /* KMH */

#define HHalf_spell_damage u.uprops[HALF_SPDAM].intrinsic
#define EHalf_spell_damage u.uprops[HALF_SPDAM].extrinsic
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

#define HHalf_physical_damage u.uprops[HALF_PHDAM].intrinsic
#define EHalf_physical_damage u.uprops[HALF_PHDAM].extrinsic
#define Half_physical_damage (yourIntrinsic(HALF_PHDAM) || yourExtrinsic(HALF_PHDAM))

#define HRegeneration u.uprops[REGENERATION].intrinsic
#define ERegeneration u.uprops[REGENERATION].extrinsic
#define Regeneration (yourIntrinsic(REGENERATION) || yourExtrinsic(REGENERATION))

#define HEnergy_regeneration u.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration u.uprops[ENERGY_REGENERATION].extrinsic
#define Energy_regeneration (yourIntrinsic(ENERGY_REGENERATION) || yourExtrinsic(ENERGY_REGENERATION))

#define HProtection u.uprops[PROTECTION].intrinsic
#define EProtection u.uprops[PROTECTION].extrinsic
#define Protection (yourIntrinsic(PROTECTION) || yourExtrinsic(PROTECTION))

#define HProtection_from_shape_changers \
    u.uprops[PROT_FROM_SHAPE_CHANGERS].intrinsic
#define EProtection_from_shape_changers \
    u.uprops[PROT_FROM_SHAPE_CHANGERS].extrinsic
#define Protection_from_shape_changers \
    (yourIntrinsic(PROT_FROM_SHAPE_CHANGERS) || yourExtrinsic(PROT_FROM_SHAPE_CHANGERS))

#define HPolymorph u.uprops[POLYMORPH].intrinsic
#define EPolymorph u.uprops[POLYMORPH].extrinsic
#define Polymorph (yourIntrinsic(POLYMORPH) || yourExtrinsic(POLYMORPH))

#define HPolymorph_control u.uprops[POLYMORPH_CONTROL].intrinsic
#define EPolymorph_control u.uprops[POLYMORPH_CONTROL].extrinsic
#define Polymorph_control (yourIntrinsic(POLYMORPH_CONTROL) || yourExtrinsic(POLYMORPH_CONTROL))

#define HUnchanging u.uprops[UNCHANGING].intrinsic
#define EUnchanging u.uprops[UNCHANGING].extrinsic
#define Unchanging (yourIntrinsic(UNCHANGING) || yourExtrinsic(UNCHANGING)) /* KMH */

#define HFast u.uprops[FAST].intrinsic
#define EFast u.uprops[FAST].extrinsic
#define Fast (yourIntrinsic(FAST) || yourExtrinsic(FAST))
#define Very_fast ((yourIntrinsic(FAST) & ~INTRINSIC)) || yourExtrinsic(FAST)

#define HReflecting u.uprops[REFLECTING].intrinsic
#define EReflecting u.uprops[REFLECTING].extrinsic
#define Reflecting (yourIntrinsic(REFLECTING) || yourExtrinsic(REFLECTING))

#define Free_action yourExtrinsic(FREE_ACTION) /* [Tom] */

#define Fixed_abil yourExtrinsic(FIXED_ABIL) /* KMH */

#define Lifesaved yourExtrinsic(LIFESAVED)

#endif /* YOUPROP_H */
