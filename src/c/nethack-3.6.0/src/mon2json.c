/* NetHack 3.6 monst.c $NHDT-Date: 1445556875 2015/10/22 23:34:35 $  $NHDT-Branch: master $:$NHDT-Revision: 1.53 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "config.h"
#include "permonst.h"
#include "pm.h"
#include "monsym.h"

#define NO_ATTK    \
    {              \
        0, 0, 0, 0 \
    }

#define WT_ELF 800
#define WT_DRAGON 4500

#include "color.h"
#define C(color) color
#define HI_DOMESTIC CLR_WHITE /* use for player + friendlies */
#define HI_LORD CLR_MAGENTA

/*
 * Entry Format:  (from permonst.h)
 *
 * name, symbol (S_* defines),
 * difficulty level, move rate, armor class, magic resistance,
 * alignment, creation/geno flags (G_* defines),
 * 6 * attack structs ( type , damage-type, # dice, # sides ),
 * weight (WT_* defines), nutritional value, extension length,
 * sounds made (MS_* defines), physical size (MZ_* defines),
 * resistances, resistances conferred (both MR_* defines),
 * 3 * flag bitmaps (M1_*, M2_*, and M3_* defines respectively)
 * symbol color (C(x) macro)
 */
#define MON(typeID, nam, sym, lvl, gen, atk, siz, mr1, mr2, flg1, flg2, flg3, col) \
    {                                                                      \
        typeID, nam, sym, lvl, gen, atk, siz, mr1, mr2, flg1, flg2, flg3, C(col)   \
    }
/* LVL() and SIZ() collect several fields to cut down on # of args for MON()
 */
#define LVL(lvl, mov, ac, mr, aln) lvl, mov, ac, mr, aln
#define SIZ(wt, nut, snd, siz) wt, nut, snd, siz
/* ATTK() and A() are to avoid braces and commas within args to MON() */
#define ATTK(at, ad, n, d) \
    {                      \
        at, ad, n, d       \
    }
#define A(a1, a2, a3, a4, a5, a6) \
    {                             \
        a1, a2, a3, a4, a5, a6    \
    }

/*
 * Rule #1: monsters of a given class are contiguous in the
 *   mons[] array.
 *
 * Rule #2: monsters of a given class are presented in ascending
 *   order of strength.
 *
 * Rule #3: monster frequency is included in the geno mask;
 *   the frequency can be from 0 to 7.  0's will also
 *   be skipped during generation.
 *
 * Rule #4: monster subclasses (e.g. giants) should be kept
 *   together, unless it violates Rule 2.  NOGEN monsters
 *   won't violate Rule 2.
 *
 * Guidelines for color assignment:
 *
 * * Use the same color for all `growth stages' of a monster (ex.
 *   little dog/big dog, baby naga/full-grown naga.
 *
 * * Use colors given in names wherever possible. If the class has `real'
 *   members with strong color associations, use those.
 *
 * * Favor `cool' colors for cold-resistant monsters, `warm' ones for
 *   fire-resistant ones.
 *
 * * Try to reserve purple (magenta) for powerful `ruler' monsters (queen
 *   bee, kobold lord, &c.).
 *
 * * Subject to all these constraints, try to use color to make as many
 *   distinctions as the / command (that is, within a monster letter
 *   distinct names should map to distinct colors).
 *
 * The aim in assigning colors is to be consistent enough so a player can
 * become `intuitive' about them, deducing some or all of these rules
 * unconsciously. Use your common sense.
 */

#ifndef SPLITMON_2
NEARDATA struct permonst mons[] = {
    /*
     * ants
     */
    MON(PM_GIANT_ANT, "giant ant", S_ANT, LVL(2, 18, 3, 0, 0), (G_GENO | G_SGROUP | 3),
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_CARNIVORE, M2_HOSTILE, 0,
        CLR_BROWN),
    MON(PM_KILLER_BEE, "killer bee", S_ANT, LVL(1, 18, -1, 0, 0), (G_GENO | G_LGROUP | 2),
        A(ATTK(AT_STNG, AD_DRST, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1, 5, MS_BUZZ, MZ_TINY), MR_POISON, MR_POISON,
        M1_ANIMAL | M1_FLY | M1_NOHANDS | M1_POIS, M2_HOSTILE | M2_FEMALE, 0,
        CLR_YELLOW),
    MON(PM_SOLDIER_ANT, "soldier ant", S_ANT, LVL(3, 18, 3, 0, 0), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), ATTK(AT_STNG, AD_DRST, 3, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(20, 5, MS_SILENT, MZ_TINY), MR_POISON, MR_POISON,
        M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_POIS | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_BLUE),
    MON(PM_FIRE_ANT, "fire ant", S_ANT, LVL(3, 18, 3, 10, 0), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), ATTK(AT_BITE, AD_FIRE, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(30, 10, MS_SILENT, MZ_TINY), MR_FIRE, MR_FIRE,
        M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_CARNIVORE, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_GIANT_BEETLE, "giant beetle", S_ANT, LVL(5, 6, 4, 0, 0), (G_GENO | 3),
        A(ATTK(AT_BITE, AD_PHYS, 3, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_LARGE), MR_POISON, MR_POISON,
        M1_ANIMAL | M1_NOHANDS | M1_POIS | M1_CARNIVORE, M2_HOSTILE, 0,
        CLR_BLACK),
    MON(PM_QUEEN_BEE, "queen bee", S_ANT, LVL(9, 24, -4, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_STNG, AD_DRST, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1, 5, MS_BUZZ, MZ_TINY), MR_POISON, MR_POISON,
        M1_ANIMAL | M1_FLY | M1_NOHANDS | M1_OVIPAROUS | M1_POIS,
        M2_HOSTILE | M2_FEMALE | M2_PRINCE, 0, HI_LORD),
    /*
     * blobs
     */
    MON(PM_ACID_BLOB, "acid blob", S_BLOB, LVL(1, 3, 8, 0, 0), (G_GENO | 2),
        A(ATTK(AT_NONE, AD_ACID, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 10, MS_SILENT, MZ_TINY),
        MR_SLEEP | MR_POISON | MR_ACID | MR_STONE, MR_STONE,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_ACID,
        M2_WANDER | M2_NEUTER, 0, CLR_GREEN),
    MON(PM_QUIVERING_BLOB, "quivering blob", S_BLOB, LVL(5, 1, 8, 0, 0), (G_GENO | 2),
        A(ATTK(AT_TUCH, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(200, 100, MS_SILENT, MZ_SMALL), MR_SLEEP | MR_POISON, MR_POISON,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS,
        M2_WANDER | M2_HOSTILE | M2_NEUTER, 0, CLR_WHITE),
    MON(PM_GELATINOUS_CUBE, "gelatinous cube", S_BLOB, LVL(6, 6, 8, 0, 0), (G_GENO | 2),
        A(ATTK(AT_TUCH, AD_PLYS, 2, 4), ATTK(AT_NONE, AD_PLYS, 1, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 150, MS_SILENT, MZ_LARGE),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON | MR_ACID
            | MR_STONE,
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS | M1_OMNIVORE
            | M1_ACID,
        M2_WANDER | M2_HOSTILE | M2_NEUTER, 0, CLR_CYAN),
    /*
     * cockatrice
     */
    MON(PM_CHICKATRICE, "chickatrice", S_COCKATRICE, LVL(4, 4, 8, 30, 0),
        (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 2), ATTK(AT_TUCH, AD_STON, 0, 0),
          ATTK(AT_NONE, AD_STON, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(10, 10, MS_HISS, MZ_TINY), MR_POISON | MR_STONE,
        MR_POISON | MR_STONE, M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_COCKATRICE, "cockatrice", S_COCKATRICE, LVL(5, 6, 6, 30, 0), (G_GENO | 5),
        A(ATTK(AT_BITE, AD_PHYS, 1, 3), ATTK(AT_TUCH, AD_STON, 0, 0),
          ATTK(AT_NONE, AD_STON, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(30, 30, MS_HISS, MZ_SMALL), MR_POISON | MR_STONE,
        MR_POISON | MR_STONE,
        M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE | M1_OVIPAROUS, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_YELLOW),
    MON(PM_PYROLISK, "pyrolisk", S_COCKATRICE, LVL(6, 6, 6, 30, 0), (G_GENO | 1),
        A(ATTK(AT_GAZE, AD_FIRE, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_HISS, MZ_SMALL), MR_POISON | MR_FIRE,
        MR_POISON | MR_FIRE,
        M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE | M1_OVIPAROUS, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_RED),
    /*
     * dogs & other canines
     */
    MON(PM_JACKAL, "jackal", S_DOG, LVL(0, 12, 7, 0, 0), (G_GENO | G_SGROUP | 3),
        A(ATTK(AT_BITE, AD_PHYS, 1, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 250, MS_BARK, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_FOX, "fox", S_DOG, LVL(0, 15, 7, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 250, MS_BARK, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_RED),
    MON(PM_COYOTE, "coyote", S_DOG, LVL(1, 12, 7, 0, 0), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 250, MS_BARK, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_WEREJACKAL, "werejackal", S_DOG, LVL(2, 12, 7, 10, -7), (G_NOGEN | G_NOCORPSE),
        A(ATTK(AT_BITE, AD_WERE, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 250, MS_BARK, MZ_SMALL), MR_POISON, 0,
        M1_NOHANDS | M1_POIS | M1_REGEN | M1_CARNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_LITTLE_DOG, "little dog", S_DOG, LVL(2, 18, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(150, 150, MS_BARK, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_DOMESTIC, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_DINGO, "dingo", S_DOG, LVL(4, 16, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 200, MS_BARK, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_YELLOW),
    MON(PM_DOG, "dog", S_DOG, LVL(4, 16, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 200, MS_BARK, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_DOMESTIC, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_LARGE_DOG, "large dog", S_DOG, LVL(6, 15, 4, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(800, 250, MS_BARK, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_STRONG | M2_DOMESTIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_WOLF, "wolf", S_DOG, LVL(5, 12, 4, 0, 0), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 250, MS_BARK, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_WEREWOLF, "werewolf", S_DOG, LVL(5, 12, 4, 20, -7), (G_NOGEN | G_NOCORPSE),
        A(ATTK(AT_BITE, AD_WERE, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 250, MS_BARK, MZ_MEDIUM), MR_POISON, 0,
        M1_NOHANDS | M1_POIS | M1_REGEN | M1_CARNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_WINTER_WOLF_CUB, "winter wolf cub", S_DOG, LVL(5, 12, 4, 0, -5),
        (G_NOHELL | G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 8), ATTK(AT_BREA, AD_COLD, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(250, 200, MS_BARK, MZ_SMALL), MR_COLD, MR_COLD,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, 0, CLR_CYAN),
    MON(PM_WARG, "warg", S_DOG, LVL(7, 12, 4, 0, -5), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(850, 350, MS_BARK, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_WINTER_WOLF, "winter wolf", S_DOG, LVL(7, 12, 4, 20, 0), (G_NOHELL | G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), ATTK(AT_BREA, AD_COLD, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(700, 300, MS_BARK, MZ_LARGE), MR_COLD, MR_COLD,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE | M2_STRONG, 0,
        CLR_CYAN),
    MON(PM_HELL_HOUND_PUP, "hell hound pup", S_DOG, LVL(7, 12, 4, 20, -5),
        (G_HELL | G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), ATTK(AT_BREA, AD_FIRE, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(200, 200, MS_BARK, MZ_SMALL), MR_FIRE, MR_FIRE,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_RED),
    MON(PM_HELL_HOUND, "hell hound", S_DOG, LVL(12, 14, 2, 20, 0), (G_HELL | G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 3, 6), ATTK(AT_BREA, AD_FIRE, 3, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_BARK, MZ_MEDIUM), MR_FIRE, MR_FIRE,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE | M2_STRONG,
        M3_INFRAVISIBLE, CLR_RED),

    /*
     * eyes
     */
    MON(PM_GAS_SPORE, "gas spore", S_EYE, LVL(1, 3, 10, 0, 0), (G_NOCORPSE | G_GENO | 1),
        A(ATTK(AT_BOOM, AD_PHYS, 4, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_SMALL), 0, 0,
        M1_FLY | M1_BREATHLESS | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GRAY),
    MON(PM_FLOATING_EYE, "floating eye", S_EYE, LVL(2, 1, 9, 10, 0), (G_GENO | 5),
        A(ATTK(AT_NONE, AD_PLYS, 0, 70), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_SMALL), 0, 0,
        M1_FLY | M1_AMPHIBIOUS | M1_NOLIMBS | M1_NOHEAD | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_FREEZING_SPHERE, "freezing sphere", S_EYE, LVL(6, 13, 4, 0, 0),
        (G_NOCORPSE | G_NOHELL | G_GENO | 2),
        A(ATTK(AT_EXPL, AD_COLD, 4, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_SMALL), MR_COLD, MR_COLD,
        M1_FLY | M1_BREATHLESS | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_WHITE),
    MON(PM_FLAMING_SPHERE, "flaming sphere", S_EYE, LVL(6, 13, 4, 0, 0),
        (G_NOCORPSE | G_GENO | 2), A(ATTK(AT_EXPL, AD_FIRE, 4, 6), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_SMALL), MR_FIRE, MR_FIRE,
        M1_FLY | M1_BREATHLESS | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_RED),
    MON(PM_SHOCKING_SPHERE, "shocking sphere", S_EYE, LVL(6, 13, 4, 0, 0),
        (G_NOCORPSE | G_GENO | 2), A(ATTK(AT_EXPL, AD_ELEC, 4, 6), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(10, 10, MS_SILENT, MZ_SMALL), MR_ELEC, MR_ELEC,
        M1_FLY | M1_BREATHLESS | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, HI_ZAP),
#if 0 /* not yet implemented */
    MON(PM_BEHOLDER, "beholder", S_EYE,
 LVL(6, 3, 4, 0, -10), (G_GENO | 2),
 A(ATTK(AT_GAZE, AD_SLOW, 0, 0), ATTK(AT_GAZE, AD_SLEE, 2,25),
   ATTK(AT_GAZE, AD_DISN, 0, 0), ATTK(AT_GAZE, AD_STON, 0, 0),
   ATTK(AT_GAZE, AD_CNCL, 2, 4), ATTK(AT_BITE, AD_PHYS, 2, 4)),
 SIZ(10, 10, MS_SILENT, MZ_SMALL), MR_COLD, 0,
 M1_FLY | M1_BREATHLESS | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS,
 M2_NOPOLY | M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_BROWN),
#endif
    /*
     * felines
     */
    MON(PM_KITTEN, "kitten", S_FELINE, LVL(2, 18, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(150, 150, MS_MEW, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_WANDER | M2_DOMESTIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_HOUSECAT, "housecat", S_FELINE, LVL(4, 16, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(200, 200, MS_MEW, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_DOMESTIC, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_JAGUAR, "jaguar", S_FELINE, LVL(4, 15, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_BITE, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_LYNX, "lynx", S_FELINE, LVL(5, 15, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_BITE, AD_PHYS, 1, 10), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_GROWL, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_CYAN),
    MON(PM_PANTHER, "panther", S_FELINE, LVL(5, 15, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6),
          ATTK(AT_BITE, AD_PHYS, 1, 10), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BLACK),
    MON(PM_LARGE_CAT, "large cat", S_FELINE, LVL(6, 15, 4, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(250, 250, MS_MEW, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_STRONG | M2_DOMESTIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_TIGER, "tiger", S_FELINE, LVL(6, 12, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4),
          ATTK(AT_BITE, AD_PHYS, 1, 10), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_YELLOW),
    /*
     * gremlins and gargoyles
     */
    MON(PM_GREMLIN, "gremlin", S_GREMLIN, LVL(5, 12, 2, 25, -9), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6),
          ATTK(AT_BITE, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_CURS, 0, 0), NO_ATTK,
          NO_ATTK),
        SIZ(100, 20, MS_LAUGH, MZ_SMALL), MR_POISON, MR_POISON,
        M1_SWIM | M1_HUMANOID | M1_POIS, M2_STALK, M3_INFRAVISIBLE,
        CLR_GREEN),
    MON(PM_GARGOYLE, "gargoyle", S_GREMLIN, LVL(6, 10, -4, 0, -9), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 6), ATTK(AT_CLAW, AD_PHYS, 2, 6),
          ATTK(AT_BITE, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1000, 200, MS_GRUNT, MZ_HUMAN), MR_STONE, MR_STONE,
        M1_HUMANOID | M1_THICK_HIDE | M1_BREATHLESS, M2_HOSTILE | M2_STRONG,
        0, CLR_BROWN),
    MON(PM_WINGED_GARGOYLE, "winged gargoyle", S_GREMLIN, LVL(9, 15, -2, 0, -12), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 6), ATTK(AT_CLAW, AD_PHYS, 3, 6),
          ATTK(AT_BITE, AD_PHYS, 3, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 300, MS_GRUNT, MZ_HUMAN), MR_STONE, MR_STONE,
        M1_FLY | M1_HUMANOID | M1_THICK_HIDE | M1_BREATHLESS | M1_OVIPAROUS,
        M2_LORD | M2_HOSTILE | M2_STRONG | M2_MAGIC, 0, HI_LORD),
    /*
     * humanoids
     */
    MON(PM_HOBBIT, "hobbit", S_HUMANOID, LVL(1, 9, 10, 0, 6), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 200, MS_HUMANOID, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_COLLECT, M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GREEN),
    MON(PM_DWARF, "dwarf", S_HUMANOID, LVL(2, 6, 10, 10, 4), (G_GENO | 3),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(900, 300, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_TUNNEL | M1_NEEDPICK | M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_DWARF | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_BUGBEAR, "bugbear", S_HUMANOID, LVL(3, 9, 5, 0, -6), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1250, 250, MS_GROWL, MZ_LARGE), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_DWARF_LORD, "dwarf lord", S_HUMANOID, LVL(4, 6, 10, 10, 5), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(900, 300, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_TUNNEL | M1_NEEDPICK | M1_HUMANOID | M1_OMNIVORE,
        M2_DWARF | M2_STRONG | M2_LORD | M2_MALE | M2_GREEDY | M2_JEWELS
            | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
    MON(PM_DWARF_KING, "dwarf king", S_HUMANOID, LVL(6, 6, 10, 20, 6), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(900, 300, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_TUNNEL | M1_NEEDPICK | M1_HUMANOID | M1_OMNIVORE,
        M2_DWARF | M2_STRONG | M2_PRINCE | M2_MALE | M2_GREEDY | M2_JEWELS
            | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_MIND_FLAYER, "mind flayer", S_HUMANOID, LVL(9, 12, 5, 90, -8), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), ATTK(AT_TENT, AD_DRIN, 2, 1),
          ATTK(AT_TENT, AD_DRIN, 2, 1), ATTK(AT_TENT, AD_DRIN, 2, 1), NO_ATTK,
          NO_ATTK),
        SIZ(1450, 400, MS_HISS, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_FLY | M1_SEE_INVIS | M1_OMNIVORE,
        M2_HOSTILE | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_MAGENTA),
    MON(PM_MASTER_MIND_FLAYER, "master mind flayer", S_HUMANOID, LVL(13, 12, 0, 90, -8),
        (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_TENT, AD_DRIN, 2, 1),
          ATTK(AT_TENT, AD_DRIN, 2, 1), ATTK(AT_TENT, AD_DRIN, 2, 1),
          ATTK(AT_TENT, AD_DRIN, 2, 1), ATTK(AT_TENT, AD_DRIN, 2, 1)),
        SIZ(1450, 400, MS_HISS, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_FLY | M1_SEE_INVIS | M1_OMNIVORE,
        M2_HOSTILE | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_MAGENTA),
    /*
     * imps & other minor demons/devils
     */
    MON(PM_MANES, "manes", S_IMP, LVL(1, 3, 7, 0, -7),
        (G_GENO | G_LGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3),
          ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(100, 100, MS_SILENT, MZ_SMALL), MR_SLEEP | MR_POISON, 0, M1_POIS,
        M2_HOSTILE | M2_STALK, M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_HOMUNCULUS, "homunculus", S_IMP, LVL(2, 12, 6, 10, -7), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_SLEE, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(60, 100, MS_SILENT, MZ_TINY), MR_SLEEP | MR_POISON,
        MR_SLEEP | MR_POISON, M1_FLY | M1_POIS, M2_STALK,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GREEN),
    MON(PM_IMP, "imp", S_IMP, LVL(3, 12, 2, 20, -7), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(20, 10, MS_CUSS, MZ_TINY), 0, 0, M1_REGEN, M2_WANDER | M2_STALK,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_LEMURE, "lemure", S_IMP, LVL(3, 3, 7, 0, -7),
        (G_HELL | G_GENO | G_LGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(150, 100, MS_SILENT, MZ_MEDIUM), MR_SLEEP | MR_POISON, MR_SLEEP,
        M1_POIS | M1_REGEN, M2_HOSTILE | M2_WANDER | M2_STALK | M2_NEUTER,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_QUASIT, "quasit", S_IMP, LVL(3, 15, 2, 20, -7), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_DRDX, 1, 2), ATTK(AT_CLAW, AD_DRDX, 1, 2),
          ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(200, 200, MS_SILENT, MZ_SMALL), MR_POISON, MR_POISON, M1_REGEN,
        M2_STALK, M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
    MON(PM_TENGU, "tengu", S_IMP, LVL(6, 13, 5, 30, 7), (G_GENO | 3),
        A(ATTK(AT_BITE, AD_PHYS, 1, 7), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 200, MS_SQAWK, MZ_SMALL), MR_POISON, MR_POISON,
        M1_TPORT | M1_TPORT_CNTRL, M2_STALK, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_CYAN),
    /*
     * jellies
     */
    MON(PM_BLUE_JELLY, "blue jelly", S_JELLY, LVL(4, 0, 8, 10, 0), (G_GENO | 2),
        A(ATTK(AT_NONE, AD_COLD, 0, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 20, MS_SILENT, MZ_MEDIUM), MR_COLD | MR_POISON,
        MR_COLD | MR_POISON,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BLUE),
    MON(PM_SPOTTED_JELLY, "spotted jelly", S_JELLY, LVL(5, 0, 8, 10, 0), (G_GENO | 1),
        A(ATTK(AT_NONE, AD_ACID, 0, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 20, MS_SILENT, MZ_MEDIUM), MR_ACID | MR_STONE, 0,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_ACID | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GREEN),
    MON(PM_OCHRE_JELLY, "ochre jelly", S_JELLY, LVL(6, 3, 8, 20, 0), (G_GENO | 2),
        A(ATTK(AT_ENGL, AD_ACID, 3, 6), ATTK(AT_NONE, AD_ACID, 3, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(50, 20, MS_SILENT, MZ_MEDIUM), MR_ACID | MR_STONE, 0,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_ACID | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BROWN),
    /*
     * kobolds
     */
    MON(PM_KOBOLD, "kobold", S_KOBOLD, LVL(0, 6, 10, 0, -2), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 100, MS_ORC, MZ_SMALL), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_OMNIVORE, M2_HOSTILE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_LARGE_KOBOLD, "large kobold", S_KOBOLD, LVL(1, 6, 10, 0, -3), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(450, 150, MS_ORC, MZ_SMALL), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_OMNIVORE, M2_HOSTILE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_KOBOLD_LORD, "kobold lord", S_KOBOLD, LVL(2, 6, 10, 0, -4), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 200, MS_ORC, MZ_SMALL), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_OMNIVORE,
        M2_HOSTILE | M2_LORD | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_KOBOLD_SHAMAN, "kobold shaman", S_KOBOLD, LVL(2, 6, 6, 10, -4), (G_GENO | 1),
        A(ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(450, 150, MS_ORC, MZ_SMALL), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_OMNIVORE, M2_HOSTILE | M2_MAGIC,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_ZAP),
    /*
     * leprechauns
     */
    MON(PM_LEPRECHAUN, "leprechaun", S_LEPRECHAUN, LVL(5, 15, 8, 20, 0), (G_GENO | 4),
        A(ATTK(AT_CLAW, AD_SGLD, 1, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(60, 30, MS_LAUGH, MZ_TINY), 0, 0, M1_HUMANOID | M1_TPORT,
        M2_HOSTILE | M2_GREEDY, M3_INFRAVISIBLE, CLR_GREEN),
    /*
     * mimics
     */
    MON(PM_SMALL_MIMIC, "small mimic", S_MIMIC, LVL(7, 3, 7, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 200, MS_SILENT, MZ_MEDIUM), MR_ACID, 0,
        M1_BREATHLESS | M1_AMORPHOUS | M1_HIDE | M1_ANIMAL | M1_NOEYES
            | M1_NOHEAD | M1_NOLIMBS | M1_THICK_HIDE | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_LARGE_MIMIC, "large mimic", S_MIMIC, LVL(8, 3, 7, 10, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_STCK, 3, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(600, 400, MS_SILENT, MZ_LARGE), MR_ACID, 0,
        M1_CLING | M1_BREATHLESS | M1_AMORPHOUS | M1_HIDE | M1_ANIMAL
            | M1_NOEYES | M1_NOHEAD | M1_NOLIMBS | M1_THICK_HIDE
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG, 0, CLR_RED),
    MON(PM_GIANT_MIMIC, "giant mimic", S_MIMIC, LVL(9, 3, 7, 20, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_STCK, 3, 6), ATTK(AT_CLAW, AD_STCK, 3, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(800, 500, MS_SILENT, MZ_LARGE), MR_ACID, 0,
        M1_CLING | M1_BREATHLESS | M1_AMORPHOUS | M1_HIDE | M1_ANIMAL
            | M1_NOEYES | M1_NOHEAD | M1_NOLIMBS | M1_THICK_HIDE
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG, 0, HI_LORD),
    /*
     * nymphs
     */
    MON(PM_WOOD_NYMPH, "wood nymph", S_NYMPH, LVL(3, 12, 9, 20, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_SITM, 0, 0), ATTK(AT_CLAW, AD_SEDU, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_SEDUCE, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_TPORT,
        M2_HOSTILE | M2_FEMALE | M2_COLLECT, M3_INFRAVISIBLE, CLR_GREEN),
    MON(PM_WATER_NYMPH, "water nymph", S_NYMPH, LVL(3, 12, 9, 20, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_SITM, 0, 0), ATTK(AT_CLAW, AD_SEDU, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_SEDUCE, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_TPORT | M1_SWIM, M2_HOSTILE | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_MOUNTAIN_NYMPH, "mountain nymph", S_NYMPH, LVL(3, 12, 9, 20, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_SITM, 0, 0), ATTK(AT_CLAW, AD_SEDU, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(600, 300, MS_SEDUCE, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_TPORT,
        M2_HOSTILE | M2_FEMALE | M2_COLLECT, M3_INFRAVISIBLE, CLR_BROWN),
    /*
     * orcs
     */
    MON(PM_GOBLIN, "goblin", S_ORC, LVL(0, 6, 10, 0, -3), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 100, MS_ORC, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_COLLECT, M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_HOBGOBLIN, "hobgoblin", S_ORC, LVL(1, 9, 10, 0, -4), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1000, 200, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_BROWN),
    /* plain "orc" for zombie corpses only; not created at random
     */
    MON(PM_ORC, "orc", S_ORC, LVL(1, 9, 10, 0, -3), (G_GENO | G_NOGEN | G_LGROUP),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(850, 150, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_HILL_ORC, "hill orc", S_ORC, LVL(2, 9, 10, 0, -4), (G_GENO | G_LGROUP | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1000, 200, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_YELLOW),
    MON(PM_MORDOR_ORC, "Mordor orc", S_ORC, LVL(3, 5, 10, 0, -5), (G_GENO | G_LGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1200, 200, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
    MON(PM_URUK_HAI, "Uruk-hai", S_ORC, LVL(3, 7, 10, 0, -4), (G_GENO | G_LGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1300, 300, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLACK),
    MON(PM_ORC_SHAMAN, "orc shaman", S_ORC, LVL(3, 9, 5, 10, -5), (G_GENO | 1),
        A(ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1000, 300, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_ZAP),
    MON(PM_ORC_CAPTAIN, "orc-captain", S_ORC, LVL(5, 5, 10, 0, -5), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1350, 350, MS_ORC, MZ_HUMAN), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_ORC | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    /*
     * piercers
     */
    MON(PM_ROCK_PIERCER, "rock piercer", S_PIERCER, LVL(3, 1, 3, 0, 0), (G_GENO | 4),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(200, 200, MS_SILENT, MZ_SMALL), 0, 0,
        M1_CLING | M1_HIDE | M1_ANIMAL | M1_NOEYES | M1_NOLIMBS | M1_CARNIVORE
            | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_GRAY),
    MON(PM_IRON_PIERCER, "iron piercer", S_PIERCER, LVL(5, 1, 0, 0, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 3, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 300, MS_SILENT, MZ_MEDIUM), 0, 0,
        M1_CLING | M1_HIDE | M1_ANIMAL | M1_NOEYES | M1_NOLIMBS | M1_CARNIVORE
            | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_CYAN),
    MON(PM_GLASS_PIERCER, "glass piercer", S_PIERCER, LVL(7, 1, 0, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 4, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 300, MS_SILENT, MZ_MEDIUM), MR_ACID, 0,
        M1_CLING | M1_HIDE | M1_ANIMAL | M1_NOEYES | M1_NOLIMBS | M1_CARNIVORE
            | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_WHITE),
    /*
     * quadrupeds
     */
    MON(PM_ROTHE, "rothe", S_QUADRUPED, LVL(2, 9, 7, 0, 0), (G_GENO | G_SGROUP | 4),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_BITE, AD_PHYS, 1, 3),
          ATTK(AT_BITE, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 100, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_MUMAK, "mumak", S_QUADRUPED, LVL(5, 9, 0, 0, -2), (G_GENO | 1),
        A(ATTK(AT_BUTT, AD_PHYS, 4, 12), ATTK(AT_BITE, AD_PHYS, 2, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2500, 500, MS_ROAR, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_THICK_HIDE | M1_NOHANDS | M1_HERBIVORE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_LEOCROTTA, "leocrotta", S_QUADRUPED, LVL(6, 18, 4, 10, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 6), ATTK(AT_BITE, AD_PHYS, 2, 6),
          ATTK(AT_CLAW, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 500, MS_IMITATE, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE, M2_HOSTILE | M2_STRONG,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_WUMPUS, "wumpus", S_QUADRUPED, LVL(8, 3, 2, 10, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 3, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2500, 500, MS_BURBLE, MZ_LARGE), 0, 0,
        M1_CLING | M1_ANIMAL | M1_NOHANDS | M1_OMNIVORE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_CYAN),
    MON(PM_TITANOTHERE, "titanothere", S_QUADRUPED, LVL(12, 12, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2650, 650, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_THICK_HIDE | M1_NOHANDS | M1_HERBIVORE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_BALUCHITHERIUM, "baluchitherium", S_QUADRUPED, LVL(14, 12, 5, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 5, 4), ATTK(AT_CLAW, AD_PHYS, 5, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(3800, 800, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_THICK_HIDE | M1_NOHANDS | M1_HERBIVORE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_MASTODON, "mastodon", S_QUADRUPED, LVL(20, 12, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BUTT, AD_PHYS, 4, 8), ATTK(AT_BUTT, AD_PHYS, 4, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(3800, 800, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_THICK_HIDE | M1_NOHANDS | M1_HERBIVORE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_BLACK),
    /*
     * rodents
     */
    MON(PM_SEWER_RAT, "sewer rat", S_RODENT, LVL(0, 12, 7, 0, 0), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(20, 12, MS_SQEEK, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_GIANT_RAT, "giant rat", S_RODENT, LVL(1, 10, 7, 0, 0), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_SQEEK, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_RABID_RAT, "rabid rat", S_RODENT, LVL(2, 12, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_DRCO, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 5, MS_SQEEK, MZ_TINY), MR_POISON, 0,
        M1_ANIMAL | M1_NOHANDS | M1_POIS | M1_CARNIVORE, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_WERERAT, "wererat", S_RODENT, LVL(2, 12, 6, 10, -7), (G_NOGEN | G_NOCORPSE),
        A(ATTK(AT_BITE, AD_WERE, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(40, 30, MS_SQEEK, MZ_TINY), MR_POISON, 0,
        M1_NOHANDS | M1_POIS | M1_REGEN | M1_CARNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_ROCK_MOLE, "rock mole", S_RODENT, LVL(3, 3, 0, 20, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_SILENT, MZ_SMALL), 0, 0,
        M1_TUNNEL | M1_ANIMAL | M1_NOHANDS | M1_METALLIVORE,
        M2_HOSTILE | M2_GREEDY | M2_JEWELS | M2_COLLECT, M3_INFRAVISIBLE,
        CLR_GRAY),
    MON(PM_WOODCHUCK, "woodchuck", S_RODENT, LVL(3, 3, 0, 20, 0), (G_NOGEN | G_GENO),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_SILENT, MZ_SMALL), 0, 0,
        M1_TUNNEL /*LOGGING*/ | M1_ANIMAL | M1_NOHANDS | M1_SWIM
            | M1_HERBIVORE,
        /* In reality, they tunnel instead of cutting lumber.  Oh, well. */
        M2_WANDER | M2_HOSTILE, M3_INFRAVISIBLE, CLR_BROWN),
    /*
     * spiders & scorpions (keep webmaker() in sync if new critters are added)
     */
    MON(PM_CAVE_SPIDER, "cave spider", S_SPIDER, LVL(1, 12, 3, 0, 0), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 50, MS_SILENT, MZ_TINY), MR_POISON, MR_POISON,
        M1_CONCEAL | M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_GRAY),
    MON(PM_CENTIPEDE, "centipede", S_SPIDER, LVL(2, 4, 3, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_DRST, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 50, MS_SILENT, MZ_TINY), MR_POISON, MR_POISON,
        M1_CONCEAL | M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_YELLOW),
    MON(PM_GIANT_SPIDER, "giant spider", S_SPIDER, LVL(5, 15, 4, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_DRST, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(100, 100, MS_SILENT, MZ_LARGE), MR_POISON, MR_POISON,
        M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_POIS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG, 0, CLR_MAGENTA),
    MON(PM_SCORPION, "scorpion", S_SPIDER, LVL(5, 15, 3, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 2), ATTK(AT_CLAW, AD_PHYS, 1, 2),
          ATTK(AT_STNG, AD_DRST, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(50, 100, MS_SILENT, MZ_SMALL), MR_POISON, MR_POISON,
        M1_CONCEAL | M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_POIS
            | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_RED),
    /*
     * trappers, lurkers, &c
     */
    MON(PM_LURKER_ABOVE, "lurker above", S_TRAPPER, LVL(10, 3, 3, 0, 0), (G_GENO | 2),
        A(ATTK(AT_ENGL, AD_DGST, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(800, 350, MS_SILENT, MZ_HUGE), 0, 0,
        M1_HIDE | M1_FLY | M1_ANIMAL | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STALK | M2_STRONG, 0, CLR_GRAY),
    MON(PM_TRAPPER, "trapper", S_TRAPPER, LVL(12, 3, 3, 0, 0), (G_GENO | 2),
        A(ATTK(AT_ENGL, AD_DGST, 1, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(800, 350, MS_SILENT, MZ_HUGE), 0, 0,
        M1_HIDE | M1_ANIMAL | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STALK | M2_STRONG, 0, CLR_GREEN),
    /*
     * unicorns and horses
     */
    MON(PM_PONY, "pony", S_UNICORN, LVL(3, 16, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_KICK, AD_PHYS, 1, 6), ATTK(AT_BITE, AD_PHYS, 1, 2), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1300, 250, MS_NEIGH, MZ_MEDIUM), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_HERBIVORE,
        M2_WANDER | M2_STRONG | M2_DOMESTIC, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_WHITE_UNICORN, "white unicorn", S_UNICORN, LVL(4, 24, 2, 70, 7), (G_GENO | 2),
        A(ATTK(AT_BUTT, AD_PHYS, 1, 12), ATTK(AT_KICK, AD_PHYS, 1, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1300, 300, MS_NEIGH, MZ_LARGE), MR_POISON, MR_POISON,
        M1_NOHANDS | M1_HERBIVORE, M2_WANDER | M2_STRONG | M2_JEWELS,
        M3_INFRAVISIBLE, CLR_WHITE),
    MON(PM_GRAY_UNICORN, "gray unicorn", S_UNICORN, LVL(4, 24, 2, 70, 0), (G_GENO | 1),
        A(ATTK(AT_BUTT, AD_PHYS, 1, 12), ATTK(AT_KICK, AD_PHYS, 1, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1300, 300, MS_NEIGH, MZ_LARGE), MR_POISON, MR_POISON,
        M1_NOHANDS | M1_HERBIVORE, M2_WANDER | M2_STRONG | M2_JEWELS,
        M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_BLACK_UNICORN, "black unicorn", S_UNICORN, LVL(4, 24, 2, 70, -7), (G_GENO | 1),
        A(ATTK(AT_BUTT, AD_PHYS, 1, 12), ATTK(AT_KICK, AD_PHYS, 1, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1300, 300, MS_NEIGH, MZ_LARGE), MR_POISON, MR_POISON,
        M1_NOHANDS | M1_HERBIVORE, M2_WANDER | M2_STRONG | M2_JEWELS,
        M3_INFRAVISIBLE, CLR_BLACK),
    MON(PM_HORSE, "horse", S_UNICORN, LVL(5, 20, 5, 0, 0), (G_GENO | 2),
        A(ATTK(AT_KICK, AD_PHYS, 1, 8), ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 300, MS_NEIGH, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_HERBIVORE,
        M2_WANDER | M2_STRONG | M2_DOMESTIC, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_WARHORSE, "warhorse", S_UNICORN, LVL(7, 24, 4, 0, 0), (G_GENO | 2),
        A(ATTK(AT_KICK, AD_PHYS, 1, 10), ATTK(AT_BITE, AD_PHYS, 1, 4),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1800, 350, MS_NEIGH, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_HERBIVORE,
        M2_WANDER | M2_STRONG | M2_DOMESTIC, M3_INFRAVISIBLE, CLR_BROWN),
    /*
     * vortices
     */
    MON(PM_FOG_CLOUD, "fog cloud", S_VORTEX, LVL(3, 1, 0, 0, 0), (G_GENO | G_NOCORPSE | 2),
        A(ATTK(AT_ENGL, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE), MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_AMORPHOUS | M1_UNSOLID,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GRAY),
    MON(PM_DUST_VORTEX, "dust vortex", S_VORTEX, LVL(4, 20, 2, 30, 0),
        (G_GENO | G_NOCORPSE | 2), A(ATTK(AT_ENGL, AD_BLND, 2, 8), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE), MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BROWN),
    MON(PM_ICE_VORTEX, "ice vortex", S_VORTEX, LVL(5, 20, 2, 30, 0),
        (G_NOHELL | G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_ENGL, AD_COLD, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE),
        MR_COLD | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_CYAN),
    MON(PM_ENERGY_VORTEX, "energy vortex", S_VORTEX, LVL(6, 20, 2, 30, 0),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_ENGL, AD_ELEC, 1, 6), ATTK(AT_ENGL, AD_DREN, 4, 6),
          ATTK(AT_NONE, AD_ELEC, 0, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE),
        MR_ELEC | MR_SLEEP | MR_DISINT | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_UNSOLID,
        M2_HOSTILE | M2_NEUTER, 0, HI_ZAP),
    MON(PM_STEAM_VORTEX, "steam vortex", S_VORTEX, LVL(7, 22, 2, 30, 0),
        (G_HELL | G_GENO | G_NOCORPSE | 2),
        A(ATTK(AT_ENGL, AD_FIRE, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE),
        MR_FIRE | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_UNSOLID,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_FIRE_VORTEX, "fire vortex", S_VORTEX, LVL(8, 22, 2, 30, 0),
        (G_HELL | G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_ENGL, AD_FIRE, 1, 10), ATTK(AT_NONE, AD_FIRE, 0, 4),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE),
        MR_FIRE | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_UNSOLID,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_YELLOW),
    /*
     * worms
     */
    MON(PM_BABY_LONG_WORM, "baby long worm", S_WORM, LVL(5, 3, 5, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(600, 250, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_SLITHY | M1_NOLIMBS | M1_CARNIVORE | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_BABY_PURPLE_WORM, "baby purple worm", S_WORM, LVL(8, 3, 5, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(600, 250, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_SLITHY | M1_NOLIMBS | M1_CARNIVORE, M2_HOSTILE, 0,
        CLR_MAGENTA),
    MON(PM_LONG_WORM, "long worm", S_WORM, LVL(9, 3, 5, 10, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_SILENT, MZ_GIGANTIC), 0, 0,
        M1_ANIMAL | M1_SLITHY | M1_NOLIMBS | M1_OVIPAROUS | M1_CARNIVORE
            | M1_NOTAKE,
        M2_HOSTILE | M2_STRONG | M2_NASTY, 0, CLR_BROWN),
    MON(PM_PURPLE_WORM, "purple worm", S_WORM, LVL(15, 9, 6, 20, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 2, 8), ATTK(AT_ENGL, AD_DGST, 1, 10),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2700, 700, MS_SILENT, MZ_GIGANTIC), 0, 0,
        M1_ANIMAL | M1_SLITHY | M1_NOLIMBS | M1_OVIPAROUS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY, 0, CLR_MAGENTA),
    /*
     * xan, &c
     */
    MON(PM_GRID_BUG, "grid bug", S_XAN, LVL(0, 12, 9, 0, 0),
        (G_GENO | G_SGROUP | G_NOCORPSE | 3),
        A(ATTK(AT_BITE, AD_ELEC, 1, 1), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(15, 10, MS_BUZZ, MZ_TINY), MR_ELEC | MR_POISON, 0, M1_ANIMAL,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_MAGENTA),
    MON(PM_XAN, "xan", S_XAN, LVL(7, 18, -4, 0, 0), (G_GENO | 3),
        A(ATTK(AT_STNG, AD_LEGS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(300, 300, MS_BUZZ, MZ_TINY), MR_POISON, MR_POISON,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_POIS, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_RED),
    /*
     * lights
     */
    MON(PM_YELLOW_LIGHT, "yellow light", S_LIGHT, LVL(3, 15, 0, 0, 0),
        (G_NOCORPSE | G_GENO | 4), A(ATTK(AT_EXPL, AD_BLND, 10, 20), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_SMALL),
        MR_FIRE | MR_COLD | MR_ELEC | MR_DISINT | MR_SLEEP | MR_POISON
            | MR_ACID | MR_STONE,
        0, M1_FLY | M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS
               | M1_NOHEAD | M1_MINDLESS | M1_UNSOLID | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_YELLOW),
    MON(PM_BLACK_LIGHT, "black light", S_LIGHT, LVL(5, 15, 0, 0, 0),
        (G_NOCORPSE | G_GENO | 2), A(ATTK(AT_EXPL, AD_HALU, 10, 12), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_SMALL),
        MR_FIRE | MR_COLD | MR_ELEC | MR_DISINT | MR_SLEEP | MR_POISON
            | MR_ACID | MR_STONE,
        0,
        M1_FLY | M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS
            | M1_NOHEAD | M1_MINDLESS | M1_UNSOLID | M1_SEE_INVIS | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BLACK),
    /*
     * zruty
     */
    MON(PM_ZRUTY, "zruty", S_ZRUTY, LVL(9, 8, 3, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 4), ATTK(AT_CLAW, AD_PHYS, 3, 4),
          ATTK(AT_BITE, AD_PHYS, 3, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 600, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE, M2_HOSTILE | M2_STRONG,
        M3_INFRAVISIBLE, CLR_BROWN),
    /*
     * Angels and other lawful minions
     */
    MON(PM_COUATL, "couatl", S_ANGEL, LVL(8, 10, 5, 30, 7),
        (G_NOHELL | G_SGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_BITE, AD_DRST, 2, 4), ATTK(AT_BITE, AD_PHYS, 1, 3),
          ATTK(AT_HUGS, AD_WRAP, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(900, 400, MS_HISS, MZ_LARGE), MR_POISON, 0,
        M1_FLY | M1_NOHANDS | M1_SLITHY | M1_POIS,
        M2_MINION | M2_STALK | M2_STRONG | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GREEN),
    MON(PM_ALEAX, "Aleax", S_ANGEL, LVL(10, 8, 0, 30, 7), (G_NOHELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6),
          ATTK(AT_KICK, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_IMITATE, MZ_HUMAN),
        MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_HUMANOID | M1_SEE_INVIS,
        M2_MINION | M2_STALK | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_YELLOW),
    /* Angels start with the emin extension attached, and usually have
       the isminion flag set; however, non-minion Angels can be tamed
       and will switch to edog (guardian Angel is handled specially and
       always sticks with emin) */
    MON(PM_ANGEL, "Angel", S_ANGEL, LVL(14, 10, -4, 55, 12),
        (G_NOHELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_MAGC, AD_MAGM, 2, 6), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_CUSS, MZ_HUMAN),
        MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_HUMANOID | M1_SEE_INVIS,
        M2_NOPOLY | M2_MINION | M2_STALK | M2_STRONG | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_WHITE),
    MON(PM_KI_RIN, "ki-rin", S_ANGEL, LVL(16, 18, -5, 90, 15),
        (G_NOHELL | G_NOCORPSE | 1),
        A(ATTK(AT_KICK, AD_PHYS, 2, 4), ATTK(AT_KICK, AD_PHYS, 2, 4),
          ATTK(AT_BUTT, AD_PHYS, 3, 6), ATTK(AT_MAGC, AD_SPEL, 2, 6), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEIGH, MZ_LARGE), 0, 0,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_SEE_INVIS,
        M2_NOPOLY | M2_MINION | M2_STALK | M2_STRONG | M2_NASTY | M2_LORD,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_GOLD),
    MON(PM_ARCHON, "Archon", S_ANGEL, LVL(19, 16, -6, 80, 15),
        (G_NOHELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4),
          ATTK(AT_GAZE, AD_BLND, 2, 6), ATTK(AT_CLAW, AD_PHYS, 1, 8),
          ATTK(AT_MAGC, AD_SPEL, 4, 6), NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_CUSS, MZ_LARGE),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_HUMANOID | M1_SEE_INVIS | M1_REGEN,
        M2_NOPOLY | M2_MINION | M2_STALK | M2_STRONG | M2_NASTY | M2_LORD
            | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    /*
     * Bats
     */
    MON(PM_BAT, "bat", S_BAT, LVL(0, 22, 8, 0, 0), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(20, 20, MS_SQEEK, MZ_TINY), 0, 0,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_WANDER,
        M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_GIANT_BAT, "giant bat", S_BAT, LVL(2, 22, 7, 0, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_SQEEK, MZ_SMALL), 0, 0,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_WANDER | M2_HOSTILE, M3_INFRAVISIBLE, CLR_RED),
    MON(PM_RAVEN, "raven", S_BAT, LVL(4, 20, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_BLND, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(40, 20, MS_SQAWK, MZ_SMALL), 0, 0,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_WANDER | M2_HOSTILE, M3_INFRAVISIBLE, CLR_BLACK),
    MON(PM_VAMPIRE_BAT, "vampire bat", S_BAT, LVL(5, 20, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), ATTK(AT_BITE, AD_DRST, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(30, 20, MS_SQEEK, MZ_SMALL), MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_ANIMAL | M1_NOHANDS | M1_POIS | M1_REGEN | M1_OMNIVORE,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_BLACK),
    /*
     * Centaurs
     */
    MON(PM_PLAINS_CENTAUR, "plains centaur", S_CENTAUR, LVL(4, 18, 4, 0, 0), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_KICK, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2500, 500, MS_HUMANOID, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_STRONG | M2_GREEDY | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_FOREST_CENTAUR, "forest centaur", S_CENTAUR, LVL(5, 18, 3, 10, -1), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_KICK, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2550, 600, MS_HUMANOID, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_STRONG | M2_GREEDY | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_GREEN),
    MON(PM_MOUNTAIN_CENTAUR, "mountain centaur", S_CENTAUR, LVL(6, 20, 2, 10, -3), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 10), ATTK(AT_KICK, AD_PHYS, 1, 6),
          ATTK(AT_KICK, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2550, 500, MS_HUMANOID, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_STRONG | M2_GREEDY | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_CYAN),
    /*
     * Dragons
     */
    /* The order of the dragons is VERY IMPORTANT.  Quite a few
     * pieces of code depend on gray being first and yellow being last.
     * The code also depends on the *order* being the same as that for
     * dragon scale mail and dragon scales in objects.c.  Baby dragons
     * cannot confer intrinsics, to avoid polyself/egg abuse.
     *
     * As reptiles, dragons are cold-blooded and thus aren't seen
     * with infravision.  Red dragons are the exception.
     */
    MON(PM_BABY_GRAY_DRAGON, "baby gray dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), 0, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_GRAY),
    MON(PM_BABY_SILVER_DRAGON, "baby silver dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), 0, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, DRAGON_SILVER),
#if 0 /* DEFERRED */
    MON(PM_BABY_SHIMMERING_DRAGON, "baby shimmering dragon", S_DRAGON,
 LVL(12, 9, 2, 10, 0), G_GENO,
 A(ATTK(AT_BITE, AD_PHYS, 2, 6),
   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(1500, 500, MS_ROAR, MZ_HUGE), 0, 0,
 M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
 M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_CYAN),
#endif
    MON(PM_BABY_RED_DRAGON, "baby red dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_FIRE, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, M3_INFRAVISIBLE,
        CLR_RED),
    MON(PM_BABY_WHITE_DRAGON, "baby white dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_COLD, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_WHITE),
    MON(PM_BABY_ORANGE_DRAGON, "baby orange dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_SLEEP, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_ORANGE),
    MON(PM_BABY_BLACK_DRAGON, "baby black dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_DISINT, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_BLACK),
    MON(PM_BABY_BLUE_DRAGON, "baby blue dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_ELEC, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_BLUE),
    MON(PM_BABY_GREEN_DRAGON, "baby green dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_POISON, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE | M1_POIS,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_GREEN),
    MON(PM_BABY_YELLOW_DRAGON, "baby yellow dragon", S_DRAGON, LVL(12, 9, 2, 10, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_ROAR, MZ_HUGE), MR_ACID | MR_STONE, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE | M1_ACID,
        M2_HOSTILE | M2_STRONG | M2_GREEDY | M2_JEWELS, 0, CLR_YELLOW),
    MON(PM_GRAY_DRAGON, "gray dragon", S_DRAGON, LVL(15, 9, -1, 20, 4), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_MAGM, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), 0, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_GRAY),
    MON(PM_SILVER_DRAGON, "silver dragon", S_DRAGON, LVL(15, 9, -1, 20, 4), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_COLD, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_COLD, 0,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, DRAGON_SILVER),
#if 0 /* DEFERRED */
    MON(PM_SHIMMERING_DRAGON, "shimmering dragon", S_DRAGON,
 LVL(15, 9, -1, 20, 4), (G_GENO | 1),
 A(ATTK(AT_BREA, AD_MAGM, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
   ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
   NO_ATTK, NO_ATTK),
 SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), 0, 0,
 M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
   | M1_CARNIVORE,
 M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
 0, CLR_CYAN),
#endif
    MON(PM_RED_DRAGON, "red dragon", S_DRAGON, LVL(15, 9, -1, 20, -4), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_FIRE, 6, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_FIRE, MR_FIRE,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_WHITE_DRAGON, "white dragon", S_DRAGON, LVL(15, 9, -1, 20, -5), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_COLD, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_COLD, MR_COLD,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_WHITE),
    MON(PM_ORANGE_DRAGON, "orange dragon", S_DRAGON, LVL(15, 9, -1, 20, 5), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_SLEE, 4, 25), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_SLEEP, MR_SLEEP,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_ORANGE),
    MON(PM_BLACK_DRAGON, "black dragon", S_DRAGON, LVL(15, 9, -1, 20, -6), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_DISN, 4, 10), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_DISINT, MR_DISINT,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_BLACK),
    MON(PM_BLUE_DRAGON, "blue dragon", S_DRAGON, LVL(15, 9, -1, 20, -7), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_ELEC, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_ELEC, MR_ELEC,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_BLUE),
    MON(PM_GREEN_DRAGON, "green dragon", S_DRAGON, LVL(15, 9, -1, 20, 6), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_DRST, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_POISON, MR_POISON,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS | M1_OVIPAROUS
            | M1_CARNIVORE | M1_POIS,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_GREEN),
    MON(PM_YELLOW_DRAGON, "yellow dragon", S_DRAGON, LVL(15, 9, -1, 20, 7), (G_GENO | 1),
        A(ATTK(AT_BREA, AD_ACID, 4, 6), ATTK(AT_BITE, AD_PHYS, 3, 8),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_DRAGON, 1500, MS_ROAR, MZ_GIGANTIC), MR_ACID | MR_STONE,
        MR_STONE, M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_SEE_INVIS
                      | M1_OVIPAROUS | M1_CARNIVORE | M1_ACID,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        0, CLR_YELLOW),
    /*
     * Elementals
     */
    MON(PM_STALKER, "stalker", S_ELEMENTAL, LVL(8, 12, 3, 0, 0), (G_GENO | 3),
        A(ATTK(AT_CLAW, AD_PHYS, 4, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(900, 400, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_FLY | M1_SEE_INVIS,
        M2_WANDER | M2_STALK | M2_HOSTILE | M2_STRONG, M3_INFRAVISION,
        CLR_WHITE),
    MON(PM_AIR_ELEMENTAL, "air elemental", S_ELEMENTAL, LVL(8, 36, 2, 30, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_ENGL, AD_PHYS, 1, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE), MR_POISON | MR_STONE, 0,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS | M1_BREATHLESS
            | M1_UNSOLID | M1_FLY,
        M2_STRONG | M2_NEUTER, 0, CLR_CYAN),
    MON(PM_FIRE_ELEMENTAL, "fire elemental", S_ELEMENTAL, LVL(8, 12, 2, 30, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_FIRE, 3, 6), ATTK(AT_NONE, AD_FIRE, 0, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUGE), MR_FIRE | MR_POISON | MR_STONE, 0,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS | M1_BREATHLESS
            | M1_UNSOLID | M1_FLY | M1_NOTAKE,
        M2_STRONG | M2_NEUTER, M3_INFRAVISIBLE, CLR_YELLOW),
    MON(PM_EARTH_ELEMENTAL, "earth elemental", S_ELEMENTAL, LVL(8, 6, 2, 30, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 4, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2500, 0, MS_SILENT, MZ_HUGE),
        MR_FIRE | MR_COLD | MR_POISON | MR_STONE, 0,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS | M1_BREATHLESS
            | M1_WALLWALK | M1_THICK_HIDE,
        M2_STRONG | M2_NEUTER, 0, CLR_BROWN),
    MON(PM_WATER_ELEMENTAL, "water elemental", S_ELEMENTAL, LVL(8, 6, 2, 30, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 5, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2500, 0, MS_SILENT, MZ_HUGE), MR_POISON | MR_STONE, 0,
        M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS | M1_BREATHLESS
            | M1_UNSOLID | M1_AMPHIBIOUS | M1_SWIM,
        M2_STRONG | M2_NEUTER, 0, CLR_BLUE),
    /*
     * Fungi
     */
    MON(PM_LICHEN, "lichen", S_FUNGUS, LVL(0, 1, 9, 0, 0), (G_GENO | 4),
        A(ATTK(AT_TUCH, AD_STCK, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(20, 200, MS_SILENT, MZ_SMALL), 0, 0,
        M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BRIGHT_GREEN),
    MON(PM_BROWN_MOLD, "brown mold", S_FUNGUS, LVL(1, 0, 9, 0, 0), (G_GENO | 1),
        A(ATTK(AT_NONE, AD_COLD, 0, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 30, MS_SILENT, MZ_SMALL), MR_COLD | MR_POISON,
        MR_COLD | MR_POISON, M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS
                                 | M1_NOHEAD | M1_MINDLESS | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BROWN),
    MON(PM_YELLOW_MOLD, "yellow mold", S_FUNGUS, LVL(1, 0, 9, 0, 0), (G_GENO | 2),
        A(ATTK(AT_NONE, AD_STUN, 0, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 30, MS_SILENT, MZ_SMALL), MR_POISON, MR_POISON,
        M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_POIS | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_YELLOW),
    MON(PM_GREEN_MOLD, "green mold", S_FUNGUS, LVL(1, 0, 9, 0, 0), (G_GENO | 1),
        A(ATTK(AT_NONE, AD_ACID, 0, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 30, MS_SILENT, MZ_SMALL), MR_ACID | MR_STONE, MR_STONE,
        M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_ACID | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GREEN),
    MON(PM_RED_MOLD, "red mold", S_FUNGUS, LVL(1, 0, 9, 0, 0), (G_GENO | 1),
        A(ATTK(AT_NONE, AD_FIRE, 0, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 30, MS_SILENT, MZ_SMALL), MR_FIRE | MR_POISON,
        MR_FIRE | MR_POISON, M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS
                                 | M1_NOHEAD | M1_MINDLESS | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, M3_INFRAVISIBLE, CLR_RED),
    MON(PM_SHRIEKER, "shrieker", S_FUNGUS, LVL(3, 1, 7, 0, 0), (G_GENO | 1),
        A(NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(100, 100, MS_SHRIEK, MZ_SMALL), MR_POISON, MR_POISON,
        M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_MAGENTA),
    MON(PM_VIOLET_FUNGUS, "violet fungus", S_FUNGUS, LVL(3, 1, 7, 0, 0), (G_GENO | 2),
        A(ATTK(AT_TUCH, AD_PHYS, 1, 4), ATTK(AT_TUCH, AD_STCK, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(100, 100, MS_SILENT, MZ_SMALL), MR_POISON, MR_POISON,
        M1_BREATHLESS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD | M1_MINDLESS
            | M1_NOTAKE,
        M2_HOSTILE | M2_NEUTER, 0, CLR_MAGENTA),
    /*
     * Gnomes
     */
    MON(PM_GNOME, "gnome", S_GNOME, LVL(1, 6, 10, 4, 0), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(650, 100, MS_ORC, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_GNOME | M2_COLLECT, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_BROWN),
    MON(PM_GNOME_LORD, "gnome lord", S_GNOME, LVL(3, 8, 10, 4, 0), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(700, 120, MS_ORC, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_GNOME | M2_LORD | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
    MON(PM_GNOMISH_WIZARD, "gnomish wizard", S_GNOME, LVL(3, 10, 4, 10, 0), (G_GENO | 1),
        A(ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(700, 120, MS_ORC, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_GNOME | M2_MAGIC, M3_INFRAVISIBLE | M3_INFRAVISION, HI_ZAP),
    MON(PM_GNOME_KING, "gnome king", S_GNOME, LVL(5, 10, 10, 20, 0), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(750, 150, MS_ORC, MZ_SMALL), 0, 0, M1_HUMANOID | M1_OMNIVORE,
        M2_GNOME | M2_PRINCE | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),

    /*
     * giant Humanoids
     */
    MON(PM_GIANT, "giant", S_GIANT, LVL(6, 6, 0, 0, 2), (G_GENO | G_NOGEN | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2250, 750, MS_BOAST, MZ_HUGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_GIANT | M2_STRONG | M2_ROCKTHROW | M2_NASTY | M2_COLLECT
            | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_STONE_GIANT, "stone giant", S_GIANT, LVL(6, 6, 0, 0, 2), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2250, 750, MS_BOAST, MZ_HUGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_GIANT | M2_STRONG | M2_ROCKTHROW | M2_NASTY | M2_COLLECT
            | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_HILL_GIANT, "hill giant", S_GIANT, LVL(8, 10, 6, 0, -2), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2200, 700, MS_BOAST, MZ_HUGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_GIANT | M2_STRONG | M2_ROCKTHROW | M2_NASTY | M2_COLLECT
            | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_CYAN),
    MON(PM_FIRE_GIANT, "fire giant", S_GIANT, LVL(9, 12, 4, 5, 2), (G_GENO | G_SGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2250, 750, MS_BOAST, MZ_HUGE), MR_FIRE, MR_FIRE,
        M1_HUMANOID | M1_CARNIVORE, M2_GIANT | M2_STRONG | M2_ROCKTHROW
                                        | M2_NASTY | M2_COLLECT | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_YELLOW),
    MON(PM_FROST_GIANT, "frost giant", S_GIANT, LVL(10, 12, 3, 10, -3),
        (G_NOHELL | G_GENO | G_SGROUP | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 12), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2250, 750, MS_BOAST, MZ_HUGE), MR_COLD, MR_COLD,
        M1_HUMANOID | M1_CARNIVORE, M2_GIANT | M2_STRONG | M2_ROCKTHROW
                                        | M2_NASTY | M2_COLLECT | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_WHITE),
    MON(PM_ETTIN, "ettin", S_GIANT, LVL(10, 12, 3, 0, 0), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), ATTK(AT_WEAP, AD_PHYS, 3, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1700, 500, MS_GRUNT, MZ_HUGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_STORM_GIANT, "storm giant", S_GIANT, LVL(16, 12, 3, 10, -3),
        (G_GENO | G_SGROUP | 1), A(ATTK(AT_WEAP, AD_PHYS, 2, 12), NO_ATTK,
                                   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2250, 750, MS_BOAST, MZ_HUGE), MR_ELEC, MR_ELEC,
        M1_HUMANOID | M1_CARNIVORE, M2_GIANT | M2_STRONG | M2_ROCKTHROW
                                        | M2_NASTY | M2_COLLECT | M2_JEWELS,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
    MON(PM_TITAN, "titan", S_GIANT, LVL(16, 18, -3, 70, 9), (1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2300, 900, MS_SPELL, MZ_HUGE), 0, 0,
        M1_FLY | M1_HUMANOID | M1_OMNIVORE,
        M2_STRONG | M2_ROCKTHROW | M2_NASTY | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_MAGENTA),
    MON(PM_MINOTAUR, "minotaur", S_GIANT, LVL(15, 15, 6, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 10), ATTK(AT_CLAW, AD_PHYS, 3, 10),
          ATTK(AT_BUTT, AD_PHYS, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 700, MS_SILENT, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_BROWN),
    /* 'I' is a visual marker for all invisible monsters and must be unused */
    /*
     * Jabberwock
     */
    /* the illustration from _Through_the_Looking_Glass_
       depicts hands as well as wings */
    MON(PM_JABBERWOCK, "jabberwock", S_JABBERWOCK, LVL(15, 12, -2, 50, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 10), ATTK(AT_BITE, AD_PHYS, 2, 10),
          ATTK(AT_CLAW, AD_PHYS, 2, 10), ATTK(AT_CLAW, AD_PHYS, 2, 10),
          NO_ATTK, NO_ATTK),
        SIZ(1300, 600, MS_BURBLE, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_FLY | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY | M2_COLLECT, M3_INFRAVISIBLE,
        CLR_ORANGE),
#if 0 /* DEFERRED */
    MON(PM_VORPAL_JABBERWOCK, "vorpal jabberwock", S_JABBERWOCK,
 LVL(20, 12, -2, 50, 0), (G_GENO | 1),
 A(ATTK(AT_BITE, AD_PHYS, 3, 10), ATTK(AT_BITE, AD_PHYS, 3, 10),
   ATTK(AT_CLAW, AD_PHYS, 3, 10), ATTK(AT_CLAW, AD_PHYS, 3, 10),
   NO_ATTK, NO_ATTK),
 SIZ(1300, 600, MS_BURBLE, MZ_LARGE), 0, 0,
 M1_ANIMAL | M1_FLY | M1_CARNIVORE,
 M2_HOSTILE | M2_STRONG | M2_NASTY | M2_COLLECT, M3_INFRAVISIBLE,
        HI_LORD),
#endif
    /*
     * Kops
     */
    MON(PM_KEYSTONE_KOP, "Keystone Kop", S_KOP, LVL(1, 6, 10, 10, 9),
        (G_GENO | G_LGROUP | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_ARREST, MZ_HUMAN), 0, 0, M1_HUMANOID,
        M2_HUMAN | M2_WANDER | M2_HOSTILE | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_KOP_SERGEANT, "Kop Sergeant", S_KOP, LVL(2, 8, 10, 10, 10),
        (G_GENO | G_SGROUP | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_ARREST, MZ_HUMAN), 0, 0, M1_HUMANOID,
        M2_HUMAN | M2_WANDER | M2_HOSTILE | M2_STRONG | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_KOP_LIEUTENANT, "Kop Lieutenant", S_KOP, LVL(3, 10, 10, 20, 11), (G_GENO | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_ARREST, MZ_HUMAN), 0, 0, M1_HUMANOID,
        M2_HUMAN | M2_WANDER | M2_HOSTILE | M2_STRONG | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_CYAN),
    MON(PM_KOP_KAPTAIN, "Kop Kaptain", S_KOP, LVL(4, 12, 10, 20, 12), (G_GENO | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_ARREST, MZ_HUMAN), 0, 0, M1_HUMANOID,
        M2_HUMAN | M2_WANDER | M2_HOSTILE | M2_STRONG | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_LORD),
    /*
     * Liches
     */
    MON(PM_LICH, "lich", S_LICH, LVL(11, 6, 0, 30, -9), (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_TUCH, AD_COLD, 1, 10), ATTK(AT_MAGC, AD_SPEL, 0, 0),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 100, MS_MUMBLE, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON,
        MR_COLD, M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_HOSTILE | M2_MAGIC, M3_INFRAVISION, CLR_BROWN),
    MON(PM_DEMILICH, "demilich", S_LICH, LVL(14, 9, -2, 60, -12),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_TUCH, AD_COLD, 3, 4), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 100, MS_MUMBLE, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON,
        MR_COLD, M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_HOSTILE | M2_MAGIC, M3_INFRAVISION, CLR_RED),
    MON(PM_MASTER_LICH, "master lich", S_LICH, LVL(17, 9, -4, 90, -15),
        (G_HELL | G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_TUCH, AD_COLD, 3, 6), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 100, MS_MUMBLE, MZ_HUMAN),
        MR_FIRE | MR_COLD | MR_SLEEP | MR_POISON, MR_FIRE | MR_COLD,
        M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_HOSTILE | M2_MAGIC, M3_WANTSBOOK | M3_INFRAVISION,
        HI_LORD),
    MON(PM_ARCH_LICH, "arch-lich", S_LICH, LVL(25, 9, -6, 90, -15),
        (G_HELL | G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_TUCH, AD_COLD, 5, 6), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 100, MS_MUMBLE, MZ_HUMAN),
        MR_FIRE | MR_COLD | MR_SLEEP | MR_ELEC | MR_POISON, MR_FIRE | MR_COLD,
        M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_HOSTILE | M2_MAGIC, M3_WANTSBOOK | M3_INFRAVISION,
        HI_LORD),
    /*
     * Mummies
     */
    MON(PM_KOBOLD_MUMMY, "kobold mummy", S_MUMMY, LVL(3, 8, 6, 20, -2),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 50, MS_SILENT, MZ_SMALL), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE, M3_INFRAVISION, CLR_BROWN),
    MON(PM_GNOME_MUMMY, "gnome mummy", S_MUMMY, LVL(4, 10, 6, 20, -3),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(650, 50, MS_SILENT, MZ_SMALL), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_GNOME, M3_INFRAVISION, CLR_RED),
    MON(PM_ORC_MUMMY, "orc mummy", S_MUMMY, LVL(5, 10, 5, 20, -4),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(850, 75, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_ORC | M2_GREEDY | M2_JEWELS,
        M3_INFRAVISION, CLR_GRAY),
    MON(PM_DWARF_MUMMY, "dwarf mummy", S_MUMMY, LVL(5, 10, 5, 20, -4),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(900, 150, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_DWARF | M2_GREEDY | M2_JEWELS,
        M3_INFRAVISION, CLR_RED),
    MON(PM_ELF_MUMMY, "elf mummy", S_MUMMY, LVL(6, 12, 4, 30, -5),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 2, 4), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_ELF, 175, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON,
        0, M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_ELF, M3_INFRAVISION, CLR_GREEN),
    MON(PM_HUMAN_MUMMY, "human mummy", S_MUMMY, LVL(6, 12, 4, 30, -5),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_SILENT, MZ_HUMAN),
        MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE, M3_INFRAVISION, CLR_GRAY),
    MON(PM_ETTIN_MUMMY, "ettin mummy", S_MUMMY, LVL(7, 12, 4, 30, -6),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 6), ATTK(AT_CLAW, AD_PHYS, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1700, 250, MS_SILENT, MZ_HUGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_STRONG, M3_INFRAVISION, CLR_BLUE),
    MON(PM_GIANT_MUMMY, "giant mummy", S_MUMMY, LVL(8, 14, 3, 30, -7),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 4), ATTK(AT_CLAW, AD_PHYS, 3, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2050, 375, MS_SILENT, MZ_HUGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_HOSTILE | M2_GIANT | M2_STRONG | M2_JEWELS,
        M3_INFRAVISION, CLR_CYAN),
    /*
     * Nagas
     */
    MON(PM_RED_NAGA_HATCHLING, "red naga hatchling", S_NAGA, LVL(3, 10, 6, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 100, MS_MUMBLE, MZ_LARGE), MR_FIRE | MR_POISON,
        MR_FIRE | MR_POISON,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_NOTAKE | M1_OMNIVORE,
        M2_STRONG, M3_INFRAVISIBLE, CLR_RED),
    MON(PM_BLACK_NAGA_HATCHLING, "black naga hatchling", S_NAGA, LVL(3, 10, 6, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 100, MS_MUMBLE, MZ_LARGE), MR_POISON | MR_ACID | MR_STONE,
        MR_POISON | MR_STONE, M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_ACID
                                  | M1_NOTAKE | M1_CARNIVORE,
        M2_STRONG, 0, CLR_BLACK),
    MON(PM_GOLDEN_NAGA_HATCHLING, "golden naga hatchling", S_NAGA, LVL(3, 10, 6, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 100, MS_MUMBLE, MZ_LARGE), MR_POISON, MR_POISON,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_NOTAKE | M1_OMNIVORE,
        M2_STRONG, 0, HI_GOLD),
    MON(PM_GUARDIAN_NAGA_HATCHLING, "guardian naga hatchling", S_NAGA, LVL(3, 10, 6, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 100, MS_MUMBLE, MZ_LARGE), MR_POISON, MR_POISON,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_NOTAKE | M1_OMNIVORE,
        M2_STRONG, 0, CLR_GREEN),
    MON(PM_RED_NAGA, "red naga", S_NAGA, LVL(6, 12, 4, 0, -4), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 4), ATTK(AT_BREA, AD_FIRE, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2600, 400, MS_MUMBLE, MZ_HUGE), MR_FIRE | MR_POISON,
        MR_FIRE | MR_POISON, M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE
                                 | M1_OVIPAROUS | M1_NOTAKE | M1_OMNIVORE,
        M2_STRONG, M3_INFRAVISIBLE, CLR_RED),
    MON(PM_BLACK_NAGA, "black naga", S_NAGA, LVL(8, 14, 2, 10, 4), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), ATTK(AT_SPIT, AD_ACID, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2600, 400, MS_MUMBLE, MZ_HUGE), MR_POISON | MR_ACID | MR_STONE,
        MR_POISON | MR_STONE,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_OVIPAROUS | M1_ACID
            | M1_NOTAKE | M1_CARNIVORE,
        M2_STRONG, 0, CLR_BLACK),
    MON(PM_GOLDEN_NAGA, "golden naga", S_NAGA, LVL(10, 14, 2, 70, 5), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), ATTK(AT_MAGC, AD_SPEL, 4, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2600, 400, MS_MUMBLE, MZ_HUGE), MR_POISON, MR_POISON,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_OVIPAROUS | M1_NOTAKE
            | M1_OMNIVORE,
        M2_STRONG, 0, HI_GOLD),
    MON(PM_GUARDIAN_NAGA, "guardian naga", S_NAGA, LVL(12, 16, 0, 50, 7), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PLYS, 1, 6), ATTK(AT_SPIT, AD_DRST, 1, 6),
          ATTK(AT_HUGS, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2600, 400, MS_MUMBLE, MZ_HUGE), MR_POISON, MR_POISON,
        M1_NOLIMBS | M1_SLITHY | M1_THICK_HIDE | M1_OVIPAROUS | M1_POIS
            | M1_NOTAKE | M1_OMNIVORE,
        M2_STRONG, 0, CLR_GREEN),
    /*
     * Ogres
     */
    MON(PM_OGRE, "ogre", S_OGRE, LVL(5, 10, 5, 0, -3), (G_SGROUP | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 5), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1600, 500, MS_GRUNT, MZ_LARGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_OGRE_LORD, "ogre lord", S_OGRE, LVL(7, 12, 3, 30, -5), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1700, 700, MS_GRUNT, MZ_LARGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_STRONG | M2_LORD | M2_MALE | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_OGRE_KING, "ogre king", S_OGRE, LVL(9, 14, 4, 60, -7), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 5), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1700, 750, MS_GRUNT, MZ_LARGE), 0, 0, M1_HUMANOID | M1_CARNIVORE,
        M2_STRONG | M2_PRINCE | M2_MALE | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    /*
     * Puddings
     *
     * must be in the same order as the pudding globs in objects.c
     */
    MON(PM_GRAY_OOZE, "gray ooze", S_PUDDING, LVL(3, 1, 8, 0, 0), (G_GENO | G_NOCORPSE | 2),
        A(ATTK(AT_BITE, AD_RUST, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 250, MS_SILENT, MZ_MEDIUM),
        MR_FIRE | MR_COLD | MR_POISON | MR_ACID | MR_STONE,
        MR_FIRE | MR_COLD | MR_POISON,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_OMNIVORE | M1_ACID,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GRAY),
    MON(PM_BROWN_PUDDING, "brown pudding", S_PUDDING, LVL(5, 3, 8, 0, 0),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_BITE, AD_DCAY, 0, 0), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(500, 250, MS_SILENT, MZ_MEDIUM),
        MR_COLD | MR_ELEC | MR_POISON | MR_ACID | MR_STONE,
        MR_COLD | MR_ELEC | MR_POISON,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_OMNIVORE | M1_ACID,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BROWN),
    MON(PM_GREEN_SLIME, "green slime", S_PUDDING, LVL(6, 6, 6, 0, 0),
        (G_HELL | G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_TUCH, AD_SLIM, 1, 4), ATTK(AT_NONE, AD_SLIM, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 150, MS_SILENT, MZ_LARGE),
        MR_COLD | MR_ELEC | MR_POISON | MR_ACID | MR_STONE, 0,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_OMNIVORE | M1_ACID | M1_POIS,
        M2_HOSTILE | M2_NEUTER, 0, CLR_GREEN),
    MON(PM_BLACK_PUDDING, "black pudding", S_PUDDING, LVL(10, 6, 6, 0, 0),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_BITE, AD_CORR, 3, 8), ATTK(AT_NONE, AD_CORR, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(900, 250, MS_SILENT, MZ_LARGE),
        MR_COLD | MR_ELEC | MR_POISON | MR_ACID | MR_STONE,
        MR_COLD | MR_ELEC | MR_POISON,
        M1_BREATHLESS | M1_AMORPHOUS | M1_NOEYES | M1_NOLIMBS | M1_NOHEAD
            | M1_MINDLESS | M1_OMNIVORE | M1_ACID,
        M2_HOSTILE | M2_NEUTER, 0, CLR_BLACK),
    /*
     * Quantum mechanics
     */
    MON(PM_QUANTUM_MECHANIC, "quantum mechanic", S_QUANTMECH, LVL(7, 12, 3, 10, 0), (G_GENO | 3),
        A(ATTK(AT_CLAW, AD_TLPT, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 20, MS_HUMANOID, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE | M1_POIS | M1_TPORT, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_CYAN),
    /*
     * Rust monster or disenchanter
     */
    MON(PM_RUST_MONSTER, "rust monster", S_RUSTMONST, LVL(5, 18, 2, 0, 0), (G_GENO | 2),
        A(ATTK(AT_TUCH, AD_RUST, 0, 0), ATTK(AT_TUCH, AD_RUST, 0, 0),
          ATTK(AT_NONE, AD_RUST, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1000, 250, MS_SILENT, MZ_MEDIUM), 0, 0,
        M1_SWIM | M1_ANIMAL | M1_NOHANDS | M1_METALLIVORE, M2_HOSTILE,
        M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_DISENCHANTER, "disenchanter", S_RUSTMONST, LVL(12, 12, -10, 0, -3),
        (G_HELL | G_GENO | 2),
        A(ATTK(AT_CLAW, AD_ENCH, 4, 4), ATTK(AT_NONE, AD_ENCH, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(750, 200, MS_GROWL, MZ_LARGE), 0, 0, M1_ANIMAL | M1_CARNIVORE,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_BLUE),
    /*
     * Snakes
     */
    MON(PM_GARTER_SNAKE, "garter snake", S_SNAKE, LVL(1, 8, 8, 0, 0), (G_LGROUP | G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(50, 60, MS_HISS, MZ_TINY), 0, 0,
        M1_SWIM | M1_CONCEAL | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY
            | M1_OVIPAROUS | M1_CARNIVORE | M1_NOTAKE,
        0, 0, CLR_GREEN),
    MON(PM_SNAKE, "snake", S_SNAKE, LVL(4, 15, 3, 0, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_DRST, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(100, 80, MS_HISS, MZ_SMALL), MR_POISON, MR_POISON,
        M1_SWIM | M1_CONCEAL | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY | M1_POIS
            | M1_OVIPAROUS | M1_CARNIVORE | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_WATER_MOCCASIN, "water moccasin", S_SNAKE, LVL(4, 15, 3, 0, 0),
        (G_GENO | G_NOGEN | G_LGROUP),
        A(ATTK(AT_BITE, AD_DRST, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(150, 80, MS_HISS, MZ_SMALL), MR_POISON, MR_POISON,
        M1_SWIM | M1_CONCEAL | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY | M1_POIS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_RED),
    MON(PM_PYTHON, "python", S_SNAKE, LVL(6, 3, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), ATTK(AT_TUCH, AD_PHYS, 0, 0),
          ATTK(AT_HUGS, AD_WRAP, 1, 4), ATTK(AT_HUGS, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK),
        SIZ(250, 100, MS_HISS, MZ_LARGE), 0, 0,
        M1_SWIM | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY | M1_CARNIVORE
            | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE | M2_STRONG, M3_INFRAVISION, CLR_MAGENTA),
    MON(PM_PIT_VIPER, "pit viper", S_SNAKE, LVL(6, 15, 2, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_DRST, 1, 4), ATTK(AT_BITE, AD_DRST, 1, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(100, 60, MS_HISS, MZ_MEDIUM), MR_POISON, MR_POISON,
        M1_SWIM | M1_CONCEAL | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY | M1_POIS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, M3_INFRAVISION, CLR_BLUE),
    MON(PM_COBRA, "cobra", S_SNAKE, LVL(6, 18, 2, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_DRST, 2, 4), ATTK(AT_SPIT, AD_BLND, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(250, 100, MS_HISS, MZ_MEDIUM), MR_POISON, MR_POISON,
        M1_SWIM | M1_CONCEAL | M1_NOLIMBS | M1_ANIMAL | M1_SLITHY | M1_POIS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_BLUE),
    /*
     * Trolls
     */
    MON(PM_TROLL, "troll", S_TROLL, LVL(7, 12, 4, 0, -3), (G_GENO | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 2), ATTK(AT_CLAW, AD_PHYS, 4, 2),
          ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(800, 350, MS_GRUNT, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_REGEN | M1_CARNIVORE,
        M2_STRONG | M2_STALK | M2_HOSTILE, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_BROWN),
    MON(PM_ICE_TROLL, "ice troll", S_TROLL, LVL(9, 10, 2, 20, -3), (G_NOHELL | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_CLAW, AD_COLD, 2, 6),
          ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1000, 300, MS_GRUNT, MZ_LARGE), MR_COLD, MR_COLD,
        M1_HUMANOID | M1_REGEN | M1_CARNIVORE,
        M2_STRONG | M2_STALK | M2_HOSTILE, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_WHITE),
    MON(PM_ROCK_TROLL, "rock troll", S_TROLL, LVL(9, 12, 0, 0, -3), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 6), ATTK(AT_CLAW, AD_PHYS, 2, 8),
          ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 300, MS_GRUNT, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_REGEN | M1_CARNIVORE,
        M2_STRONG | M2_STALK | M2_HOSTILE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_CYAN),
    MON(PM_WATER_TROLL, "water troll", S_TROLL, LVL(11, 14, 4, 40, -3), (G_NOGEN | G_GENO),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), ATTK(AT_CLAW, AD_PHYS, 2, 8),
          ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 350, MS_GRUNT, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_REGEN | M1_CARNIVORE | M1_SWIM,
        M2_STRONG | M2_STALK | M2_HOSTILE, M3_INFRAVISIBLE | M3_INFRAVISION,
        CLR_BLUE),
    MON(PM_OLOG_HAI, "Olog-hai", S_TROLL, LVL(13, 12, -4, 0, -7), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 6), ATTK(AT_CLAW, AD_PHYS, 2, 8),
          ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 400, MS_GRUNT, MZ_LARGE), 0, 0,
        M1_HUMANOID | M1_REGEN | M1_CARNIVORE,
        M2_STRONG | M2_STALK | M2_HOSTILE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    /*
     * Umber hulk
     */
    MON(PM_UMBER_HULK, "umber hulk", S_UMBER, LVL(9, 6, 2, 25, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 4), ATTK(AT_CLAW, AD_PHYS, 3, 4),
          ATTK(AT_BITE, AD_PHYS, 2, 5), ATTK(AT_GAZE, AD_CONF, 0, 0), NO_ATTK,
          NO_ATTK),
        SIZ(1200, 500, MS_SILENT, MZ_LARGE), 0, 0, M1_TUNNEL | M1_CARNIVORE,
        M2_STRONG, M3_INFRAVISIBLE, CLR_BROWN),
    /*
     * Vampires
     */
    MON(PM_VAMPIRE, "vampire", S_VAMPIRE, LVL(10, 12, 1, 25, -8),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_BITE, AD_DRLI, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_VAMPIRE, MZ_HUMAN), MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY
            | M2_SHAPESHIFTER,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_VAMPIRE_LORD, "vampire lord", S_VAMPIRE, LVL(12, 14, 0, 50, -9),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 8), ATTK(AT_BITE, AD_DRLI, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_VAMPIRE, MZ_HUMAN), MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY | M2_LORD
            | M2_MALE | M2_SHAPESHIFTER,
        M3_INFRAVISIBLE, CLR_BLUE),
#if 0 /* DEFERRED */
    MON(PM_VAMPIRE_MAGE, "vampire mage", S_VAMPIRE,
 LVL(20, 14, -4, 50, -9), (G_GENO | G_NOCORPSE | 1),
 A(ATTK(AT_CLAW, AD_DRLI, 2, 8), ATTK(AT_BITE, AD_DRLI, 1, 8),
   ATTK(AT_MAGC, AD_SPEL, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(WT_HUMAN, 400, MS_VAMPIRE, MZ_HUMAN), MR_SLEEP | MR_POISON, 0,
 M1_FLY | M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
 M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY | M2_LORD
          | M2_MALE | M2_MAGIC | M2_SHAPESHIFTER,
        M3_INFRAVISIBLE, HI_ZAP),
#endif
    MON(PM_VLAD_THE_IMPALER, "Vlad the Impaler", S_VAMPIRE, LVL(14, 18, -3, 80, -10),
        (G_NOGEN | G_NOCORPSE | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 10), ATTK(AT_BITE, AD_DRLI, 1, 10),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_VAMPIRE, MZ_HUMAN), MR_SLEEP | MR_POISON, 0,
        M1_FLY | M1_BREATHLESS | M1_HUMANOID | M1_POIS | M1_REGEN,
        M2_NOPOLY | M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_PNAME | M2_STRONG
            | M2_NASTY | M2_PRINCE | M2_MALE | M2_SHAPESHIFTER,
        M3_WAITFORU | M3_WANTSCAND | M3_INFRAVISIBLE, HI_LORD),
    /*
     * Wraiths
     */
    MON(PM_BARROW_WIGHT, "barrow wight", S_WRAITH, LVL(3, 12, 5, 5, -3),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_DRLI, 0, 0), ATTK(AT_MAGC, AD_SPEL, 0, 0),
          ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1200, 0, MS_SPELL, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_HUMANOID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_COLLECT, 0, CLR_GRAY),
    MON(PM_WRAITH, "wraith", S_WRAITH, LVL(6, 12, 4, 15, -6), (G_GENO | 2),
        A(ATTK(AT_TUCH, AD_DRLI, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(0, 0, MS_SILENT, MZ_HUMAN),
        MR_COLD | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_BREATHLESS | M1_FLY | M1_HUMANOID | M1_UNSOLID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE, 0, CLR_BLACK),
    MON(PM_NAZGUL, "Nazgul", S_WRAITH, LVL(13, 12, 0, 25, -17),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_DRLI, 1, 4), ATTK(AT_BREA, AD_SLEE, 2, 25),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 0, MS_SPELL, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON,
        0, M1_BREATHLESS | M1_HUMANOID,
        M2_NOPOLY | M2_UNDEAD | M2_STALK | M2_STRONG | M2_HOSTILE | M2_MALE
            | M2_COLLECT,
        0, HI_LORD),
    /*
     * Xorn
     */
    MON(PM_XORN, "xorn", S_XORN, LVL(8, 9, -2, 20, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3),
          ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_BITE, AD_PHYS, 4, 6), NO_ATTK,
          NO_ATTK),
        SIZ(1200, 700, MS_ROAR, MZ_MEDIUM), MR_FIRE | MR_COLD | MR_STONE,
        MR_STONE,
        M1_BREATHLESS | M1_WALLWALK | M1_THICK_HIDE | M1_METALLIVORE,
        M2_HOSTILE | M2_STRONG, 0, CLR_BROWN),
    /*
     * Apelike beasts
     */
    MON(PM_MONKEY, "monkey", S_YETI, LVL(2, 12, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_SITM, 0, 0), ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(100, 50, MS_GROWL, MZ_SMALL), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE, 0, M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_APE, "ape", S_YETI, LVL(4, 12, 6, 0, 0), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3),
          ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1100, 500, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE, M2_STRONG, M3_INFRAVISIBLE,
        CLR_BROWN),
    MON(PM_OWLBEAR, "owlbear", S_YETI, LVL(5, 12, 5, 0, 0), (G_GENO | 3),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6),
          ATTK(AT_HUGS, AD_PHYS, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1700, 700, MS_ROAR, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE,
        M2_HOSTILE | M2_STRONG | M2_NASTY, M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_YETI, "yeti", S_YETI, LVL(5, 15, 6, 0, 0), (G_GENO | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6),
          ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1600, 700, MS_GROWL, MZ_LARGE), MR_COLD, MR_COLD,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE, M2_HOSTILE | M2_STRONG,
        M3_INFRAVISIBLE, CLR_WHITE),
    MON(PM_CARNIVOROUS_APE, "carnivorous ape", S_YETI, LVL(6, 12, 6, 0, 0), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_HUGS, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1250, 550, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_CARNIVORE, M2_HOSTILE | M2_STRONG,
        M3_INFRAVISIBLE, CLR_BLACK),
    MON(PM_SASQUATCH, "sasquatch", S_YETI, LVL(7, 15, 6, 0, 2), (G_GENO | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6),
          ATTK(AT_KICK, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1550, 750, MS_GROWL, MZ_LARGE), 0, 0,
        M1_ANIMAL | M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE, M2_STRONG,
        M3_INFRAVISIBLE, CLR_GRAY),
    /*
     * Zombies
     */
    MON(PM_KOBOLD_ZOMBIE, "kobold zombie", S_ZOMBIE, LVL(0, 6, 10, 0, -2),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 4), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 50, MS_SILENT, MZ_SMALL), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_STALK | M2_HOSTILE, M3_INFRAVISION, CLR_BROWN),
    MON(PM_GNOME_ZOMBIE, "gnome zombie", S_ZOMBIE, LVL(1, 6, 10, 0, -2),
        (G_GENO | G_NOCORPSE | 1), A(ATTK(AT_CLAW, AD_PHYS, 1, 5), NO_ATTK,
                                     NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(650, 50, MS_SILENT, MZ_SMALL), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_GNOME, M3_INFRAVISION,
        CLR_BROWN),
    MON(PM_ORC_ZOMBIE, "orc zombie", S_ZOMBIE, LVL(2, 6, 9, 0, -3),
        (G_GENO | G_SGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(850, 75, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_ORC, M3_INFRAVISION, CLR_GRAY),
    MON(PM_DWARF_ZOMBIE, "dwarf zombie", S_ZOMBIE, LVL(2, 6, 9, 0, -3),
        (G_GENO | G_SGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(900, 150, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_DWARF, M3_INFRAVISION,
        CLR_RED),
    MON(PM_ELF_ZOMBIE, "elf zombie", S_ZOMBIE, LVL(3, 6, 9, 0, -3),
        (G_GENO | G_SGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 7), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_ELF, 175, MS_SILENT, MZ_HUMAN), MR_COLD | MR_SLEEP | MR_POISON,
        0, M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_ELF, M3_INFRAVISION,
        CLR_GREEN),
    MON(PM_HUMAN_ZOMBIE, "human zombie", S_ZOMBIE, LVL(4, 6, 8, 0, -3),
        (G_GENO | G_SGROUP | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 200, MS_SILENT, MZ_HUMAN),
        MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE, M3_INFRAVISION, HI_DOMESTIC),
    MON(PM_ETTIN_ZOMBIE, "ettin zombie", S_ZOMBIE, LVL(6, 8, 6, 0, -4),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 10), ATTK(AT_CLAW, AD_PHYS, 1, 10),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1700, 250, MS_SILENT, MZ_HUGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_STRONG, M3_INFRAVISION,
        CLR_BLUE),
    MON(PM_GHOUL, "ghoul", S_ZOMBIE, LVL(3, 6, 10, 0, -2), (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PLYS, 1, 2), ATTK(AT_CLAW, AD_PHYS, 1, 3), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 50, MS_SILENT, MZ_SMALL), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_POIS | M1_OMNIVORE,
        M2_UNDEAD | M2_WANDER | M2_HOSTILE, M3_INFRAVISION, CLR_BLACK),
    MON(PM_GIANT_ZOMBIE, "giant zombie", S_ZOMBIE, LVL(8, 8, 6, 0, -4),
        (G_GENO | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 8), ATTK(AT_CLAW, AD_PHYS, 2, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2050, 375, MS_SILENT, MZ_HUGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID,
        M2_UNDEAD | M2_STALK | M2_HOSTILE | M2_GIANT | M2_STRONG,
        M3_INFRAVISION, CLR_CYAN),
    MON(PM_SKELETON, "skeleton", S_ZOMBIE, LVL(12, 8, 4, 0, 0), (G_NOCORPSE | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_TUCH, AD_SLOW, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(300, 5, MS_BONES, MZ_HUMAN),
        MR_COLD | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_UNDEAD | M2_WANDER | M2_HOSTILE | M2_STRONG | M2_COLLECT
            | M2_NASTY,
        M3_INFRAVISION, CLR_WHITE),
    /*
     * golems
     */
    MON(PM_STRAW_GOLEM, "straw golem", S_GOLEM, LVL(3, 12, 10, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 2), ATTK(AT_CLAW, AD_PHYS, 1, 2), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(400, 0, MS_SILENT, MZ_LARGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID, M2_HOSTILE | M2_NEUTER, 0,
        CLR_YELLOW),
    MON(PM_PAPER_GOLEM, "paper golem", S_GOLEM, LVL(3, 12, 10, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(400, 0, MS_SILENT, MZ_LARGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID, M2_HOSTILE | M2_NEUTER, 0,
        HI_PAPER),
    MON(PM_ROPE_GOLEM, "rope golem", S_GOLEM, LVL(4, 9, 8, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_HUGS, AD_PHYS, 6, 1), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(450, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID, M2_HOSTILE | M2_NEUTER, 0,
        CLR_BROWN),
    MON(PM_GOLD_GOLEM, "gold golem", S_GOLEM, LVL(5, 9, 6, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 3), ATTK(AT_CLAW, AD_PHYS, 2, 3), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(450, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON | MR_ACID, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_HOSTILE | M2_NEUTER, 0, HI_GOLD),
    MON(PM_LEATHER_GOLEM, "leather golem", S_GOLEM, LVL(6, 6, 6, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 6), ATTK(AT_CLAW, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(800, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID, M2_HOSTILE | M2_NEUTER, 0,
        HI_LEATHER),
    MON(PM_WOOD_GOLEM, "wood golem", S_GOLEM, LVL(7, 3, 4, 0, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(900, 0, MS_SILENT, MZ_LARGE), MR_COLD | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_HOSTILE | M2_NEUTER, 0, HI_WOOD),
    MON(PM_FLESH_GOLEM, "flesh golem", S_GOLEM, LVL(9, 8, 9, 30, 0), (1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 8), ATTK(AT_CLAW, AD_PHYS, 2, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1400, 600, MS_SILENT, MZ_LARGE),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON,
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID, M2_HOSTILE | M2_STRONG, 0,
        CLR_RED),
    MON(PM_CLAY_GOLEM, "clay golem", S_GOLEM, LVL(11, 7, 7, 40, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1550, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_HOSTILE | M2_STRONG, 0, CLR_BROWN),
    MON(PM_STONE_GOLEM, "stone golem", S_GOLEM, LVL(14, 6, 5, 50, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1900, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_HOSTILE | M2_STRONG, 0, CLR_GRAY),
    MON(PM_GLASS_GOLEM, "glass golem", S_GOLEM, LVL(16, 6, 1, 50, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 8), ATTK(AT_CLAW, AD_PHYS, 2, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1800, 0, MS_SILENT, MZ_LARGE), MR_SLEEP | MR_POISON | MR_ACID, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE,
        M2_HOSTILE | M2_STRONG, 0, CLR_CYAN),
    MON(PM_IRON_GOLEM, "iron golem", S_GOLEM, LVL(18, 6, 3, 60, 0), (G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), ATTK(AT_BREA, AD_DRST, 4, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2000, 0, MS_SILENT, MZ_LARGE),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_BREATHLESS | M1_MINDLESS | M1_HUMANOID | M1_THICK_HIDE | M1_POIS,
        M2_HOSTILE | M2_STRONG | M2_COLLECT, 0, HI_METAL),
    /*
     * humans, including elves and were-critters
     */
    MON(PM_HUMAN, "human", S_HUMAN, LVL(0, 12, 10, 0, 0), G_NOGEN, /* for corpses */
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_WERERAT, "wererat", S_HUMAN, LVL(2, 12, 10, 10, -7), (1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_WERE, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_REGEN | M1_OMNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE | M2_HUMAN | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BROWN),
    MON(PM_WEREJACKAL, "werejackal", S_HUMAN, LVL(2, 12, 10, 10, -7), (1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_WERE, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_REGEN | M1_OMNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE | M2_HUMAN | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_WEREWOLF, "werewolf", S_HUMAN, LVL(5, 12, 10, 20, -7), (1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_WERE, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_REGEN | M1_OMNIVORE,
        M2_NOPOLY | M2_WERE | M2_HOSTILE | M2_HUMAN | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_ORANGE),
    MON(PM_ELF, "elf", S_HUMAN, LVL(10, 12, 10, 2, -3), G_NOGEN, /* for corpses */
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS,
        M2_NOPOLY | M2_ELF | M2_STRONG | M2_COLLECT,
        M3_INFRAVISION | M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_WOODLAND_ELF, "Woodland-elf", S_HUMAN, LVL(4, 12, 10, 10, -5),
        (G_GENO | G_SGROUP | 2), A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK,
                                   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS, M2_ELF | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GREEN),
    MON(PM_GREEN_ELF, "Green-elf", S_HUMAN, LVL(5, 12, 10, 10, -6), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS, M2_ELF | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BRIGHT_GREEN),
    MON(PM_GREY_ELF, "Grey-elf", S_HUMAN, LVL(6, 12, 10, 10, -7), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS, M2_ELF | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_ELF_LORD, "elf-lord", S_HUMAN, LVL(8, 12, 10, 20, -9), (G_GENO | G_SGROUP | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS,
        M2_ELF | M2_STRONG | M2_LORD | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BRIGHT_BLUE),
    MON(PM_ELVENKING, "Elvenking", S_HUMAN, LVL(9, 12, 10, 25, -10), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_ELF, 350, MS_HUMANOID, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS,
        M2_ELF | M2_STRONG | M2_PRINCE | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_DOPPELGANGER, "doppelganger", S_HUMAN, LVL(9, 12, 5, 20, 0), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 12), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_IMITATE, MZ_HUMAN), MR_SLEEP, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_HOSTILE | M2_STRONG | M2_COLLECT
            | M2_SHAPESHIFTER,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_SHOPKEEPER, "shopkeeper", S_HUMAN, LVL(12, 18, 0, 50, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 4, 4), ATTK(AT_WEAP, AD_PHYS, 4, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SELL, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_PEACEFUL
                                       | M2_STRONG | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_GUARD, "guard", S_HUMAN, LVL(12, 12, 10, 40, 10), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARD, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_MERC | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BLUE),
    MON(PM_PRISONER, "prisoner", S_HUMAN, LVL(12, 12, 10, 0, 0),
        G_NOGEN, /* for special levels */
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_DJINNI, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE | M3_CLOSE, HI_DOMESTIC),
    MON(PM_ORACLE, "Oracle", S_HUMAN, LVL(12, 0, 0, 50, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_NONE, AD_MAGM, 0, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_ORACLE, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_FEMALE, M3_INFRAVISIBLE,
        HI_ZAP),
    /* aligned priests always have the epri extension attached;
       individual instantiations should always have either ispriest
       or isminion set */
    MON(PM_ALIGNED_PRIEST, "aligned priest", S_HUMAN, LVL(12, 12, 10, 50, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), ATTK(AT_KICK, AD_PHYS, 1, 4),
          ATTK(AT_MAGC, AD_CLRC, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_PRIEST, MZ_HUMAN), MR_ELEC, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_LORD | M2_PEACEFUL | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_WHITE),
    /* high priests always have epri and always have ispriest set */
    MON(PM_HIGH_PRIEST, "high priest", S_HUMAN, LVL(25, 15, 7, 70, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), ATTK(AT_KICK, AD_PHYS, 2, 8),
          ATTK(AT_MAGC, AD_CLRC, 2, 8), ATTK(AT_MAGC, AD_CLRC, 2, 8), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_PRIEST, MZ_HUMAN),
        MR_FIRE | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_MINION | M2_PRINCE | M2_NASTY | M2_COLLECT
            | M2_MAGIC,
        M3_INFRAVISIBLE, CLR_WHITE),
    MON(PM_SOLDIER, "soldier", S_HUMAN, LVL(6, 10, 10, 0, -2), (G_SGROUP | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_HOSTILE | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_SERGEANT, "sergeant", S_HUMAN, LVL(8, 10, 10, 5, -3), (G_SGROUP | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_HOSTILE | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_RED),
    MON(PM_NURSE, "nurse", S_HUMAN, LVL(11, 6, 0, 0, 0), (G_GENO | 3),
        A(ATTK(AT_CLAW, AD_HEAL, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NURSE, MZ_HUMAN), MR_POISON, MR_POISON,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_HOSTILE,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_LIEUTENANT, "lieutenant", S_HUMAN, LVL(10, 10, 10, 15, -4), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 4), ATTK(AT_WEAP, AD_PHYS, 3, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_HOSTILE | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_GREEN),
    MON(PM_CAPTAIN, "captain", S_HUMAN, LVL(12, 10, 10, 15, -5), (G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 4), ATTK(AT_WEAP, AD_PHYS, 4, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_HOSTILE | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_BLUE),
    /* Keep these separate - some of the mkroom code assumes that
     * all the soldiers are contiguous.
     */
    MON(PM_WATCHMAN, "watchman", S_HUMAN, LVL(6, 10, 10, 0, -2),
        (G_SGROUP | G_NOGEN | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_GRAY),
    MON(PM_WATCH_CAPTAIN, "watch captain", S_HUMAN, LVL(10, 10, 10, 15, -4),
        (G_NOGEN | G_GENO | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 4), ATTK(AT_WEAP, AD_PHYS, 3, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SOLDIER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_MERC | M2_STALK
                                       | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_GREEN),
    /* Unique humans not tied to quests.
     */
    MON(PM_MEDUSA, "Medusa", S_HUMAN, LVL(20, 12, 2, 50, -15), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 1, 8),
          ATTK(AT_GAZE, AD_STON, 0, 0), ATTK(AT_BITE, AD_DRST, 1, 6), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HISS, MZ_LARGE), MR_POISON | MR_STONE,
        MR_POISON | MR_STONE, M1_FLY | M1_SWIM | M1_AMPHIBIOUS | M1_HUMANOID
                                  | M1_POIS | M1_OMNIVORE,
        M2_NOPOLY | M2_HOSTILE | M2_STRONG | M2_PNAME | M2_FEMALE,
        M3_WAITFORU | M3_INFRAVISIBLE, CLR_BRIGHT_GREEN),
    MON(PM_WIZARD_OF_YENDOR, "Wizard of Yendor", S_HUMAN, LVL(30, 12, -8, 100, A_NONE),
        (G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_SAMU, 2, 12), ATTK(AT_MAGC, AD_SPEL, 0, 0),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_CUSS, MZ_HUMAN), MR_FIRE | MR_POISON,
        MR_FIRE | MR_POISON,
        M1_FLY | M1_BREATHLESS | M1_HUMANOID | M1_REGEN | M1_SEE_INVIS
            | M1_TPORT | M1_TPORT_CNTRL | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_HOSTILE | M2_STRONG | M2_NASTY | M2_PRINCE
            | M2_MALE | M2_MAGIC,
        M3_COVETOUS | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_CROESUS, "Croesus", S_HUMAN, LVL(20, 15, 0, 40, 15), (G_UNIQ | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARD, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY
            | M2_PNAME | M2_PRINCE | M2_MALE | M2_GREEDY | M2_JEWELS
            | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE, HI_LORD),

    /*
     * ghosts
     */
    MON(PM_GHOST, "ghost", S_GHOST, LVL(10, 3, -5, 50, -5), (G_NOCORPSE | G_NOGEN),
        A(ATTK(AT_TUCH, AD_PHYS, 1, 1), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 0, MS_SILENT, MZ_HUMAN),
        MR_COLD | MR_DISINT | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_WALLWALK | M1_HUMANOID | M1_UNSOLID,
        M2_NOPOLY | M2_UNDEAD | M2_STALK | M2_HOSTILE, M3_INFRAVISION,
        CLR_GRAY),
    MON(PM_SHADE, "shade", S_GHOST, LVL(12, 10, 10, 0, 0), (G_NOCORPSE | G_NOGEN),
        A(ATTK(AT_TUCH, AD_PLYS, 2, 6), ATTK(AT_TUCH, AD_SLOW, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 0, MS_WAIL, MZ_HUMAN),
        MR_COLD | MR_DISINT | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_BREATHLESS | M1_WALLWALK | M1_HUMANOID | M1_UNSOLID
            | M1_SEE_INVIS,
        M2_NOPOLY | M2_UNDEAD | M2_WANDER | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISION, CLR_BLACK),
    /*
     * (major) demons
     */
    MON(PM_WATER_DEMON, "water demon", S_DEMON, LVL(8, 12, -4, 30, -7),
        (G_NOCORPSE | G_NOGEN),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3),
          ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_DJINNI, MZ_HUMAN), MR_FIRE | MR_POISON, 0,
        M1_HUMANOID | M1_POIS | M1_SWIM,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BLUE),
/* standard demons & devils
 */
#define SEDUCTION_ATTACKS_YES                                     \
    A(ATTK(AT_BITE, AD_SSEX, 0, 0), ATTK(AT_CLAW, AD_PHYS, 1, 3), \
      ATTK(AT_CLAW, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK)
#define SEDUCTION_ATTACKS_NO                                      \
    A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3), \
      ATTK(AT_BITE, AD_DRLI, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK)
    MON(PM_SUCCUBUS, "succubus", S_DEMON, LVL(6, 12, 0, 70, -9), (G_NOCORPSE | 1),
        SEDUCTION_ATTACKS_YES, SIZ(WT_HUMAN, 400, MS_SEDUCE, MZ_HUMAN),
        MR_FIRE | MR_POISON, 0, M1_HUMANOID | M1_FLY | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_FEMALE,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_HORNED_DEVIL, "horned devil", S_DEMON, LVL(6, 9, -5, 50, 11),
        (G_HELL | G_NOCORPSE | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_BITE, AD_PHYS, 2, 3), ATTK(AT_STNG, AD_PHYS, 1, 3), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_HUMAN), MR_FIRE | MR_POISON, 0,
        M1_POIS | M1_THICK_HIDE, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_BROWN),
    MON(PM_INCUBUS, "incubus", S_DEMON, LVL(6, 12, 0, 70, -9), (G_NOCORPSE | 1),
        SEDUCTION_ATTACKS_YES, SIZ(WT_HUMAN, 400, MS_SEDUCE, MZ_HUMAN),
        MR_FIRE | MR_POISON, 0, M1_HUMANOID | M1_FLY | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_MALE,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    /* Used by AD&D for a type of demon, originally one of the Furies */
    /* and spelled this way */
    MON(PM_ERINYS, "erinys", S_DEMON, LVL(7, 12, 2, 30, 10),
        (G_HELL | G_NOCORPSE | G_SGROUP | 2),
        A(ATTK(AT_WEAP, AD_DRST, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_HUMAN), MR_FIRE | MR_POISON, 0,
        M1_HUMANOID | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY
            | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_BARBED_DEVIL, "barbed devil", S_DEMON, LVL(8, 12, 0, 35, 8),
        (G_HELL | G_NOCORPSE | G_SGROUP | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4),
          ATTK(AT_STNG, AD_PHYS, 3, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_HUMAN), MR_FIRE | MR_POISON, 0,
        M1_POIS | M1_THICK_HIDE, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_MARILITH, "marilith", S_DEMON, LVL(7, 12, -6, 80, -12),
        (G_HELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_WEAP, AD_PHYS, 2, 4),
          ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4),
          ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4)),
        SIZ(WT_HUMAN, 400, MS_CUSS, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_HUMANOID | M1_SLITHY | M1_SEE_INVIS | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_VROCK, "vrock", S_DEMON, LVL(8, 12, 0, 50, -9),
        (G_HELL | G_NOCORPSE | G_SGROUP | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_CLAW, AD_PHYS, 1, 8), ATTK(AT_CLAW, AD_PHYS, 1, 8),
          ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_POIS, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_HEZROU, "hezrou", S_DEMON, LVL(9, 6, -2, 55, -10),
        (G_HELL | G_NOCORPSE | G_SGROUP | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 3), ATTK(AT_CLAW, AD_PHYS, 1, 3),
          ATTK(AT_BITE, AD_PHYS, 4, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_HUMANOID | M1_POIS, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_BONE_DEVIL, "bone devil", S_DEMON, LVL(9, 15, -1, 40, -9),
        (G_HELL | G_NOCORPSE | G_SGROUP | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 4), ATTK(AT_STNG, AD_DRST, 2, 4), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_POIS, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_ICE_DEVIL, "ice devil", S_DEMON, LVL(11, 6, -4, 55, -12),
        (G_HELL | G_NOCORPSE | 2),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_BITE, AD_PHYS, 2, 4), ATTK(AT_STNG, AD_COLD, 3, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE),
        MR_FIRE | MR_COLD | MR_POISON, 0, M1_SEE_INVIS | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_WHITE),
    MON(PM_NALFESHNEE, "nalfeshnee", S_DEMON, LVL(11, 9, -1, 65, -11),
        (G_HELL | G_NOCORPSE | 1),
        A(ATTK(AT_CLAW, AD_PHYS, 1, 4), ATTK(AT_CLAW, AD_PHYS, 1, 4),
          ATTK(AT_BITE, AD_PHYS, 2, 4), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SPELL, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_HUMANOID | M1_POIS, M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_PIT_FIEND, "pit fiend", S_DEMON, LVL(13, 6, -3, 65, -13),
        (G_HELL | G_NOCORPSE | 2),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 2), ATTK(AT_WEAP, AD_PHYS, 4, 2),
          ATTK(AT_HUGS, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GROWL, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_SEE_INVIS | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    MON(PM_SANDESTIN, "sandestin", S_DEMON, LVL(13, 12, 4, 60, -5),
        (G_HELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 400, MS_CUSS, MZ_HUMAN), MR_STONE, 0, M1_HUMANOID,
        M2_NOPOLY | M2_STALK | M2_STRONG | M2_COLLECT | M2_SHAPESHIFTER,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_GRAY),
    MON(PM_BALROG, "balrog", S_DEMON, LVL(16, 5, -2, 75, -14), (G_HELL | G_NOCORPSE | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 8, 4), ATTK(AT_WEAP, AD_PHYS, 4, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_DEMON | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY | M2_COLLECT,
        M3_INFRAVISIBLE | M3_INFRAVISION, CLR_RED),
    /* Named demon lords & princes plus Arch-Devils.
     * (their order matters; see minion.c)
     */
    MON(PM_JUIBLEX, "Juiblex", S_DEMON, LVL(50, 3, -7, 65, -15),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_ENGL, AD_DISE, 4, 10), ATTK(AT_SPIT, AD_ACID, 3, 6),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 0, MS_GURGLE, MZ_LARGE),
        MR_FIRE | MR_POISON | MR_ACID | MR_STONE, 0,
        M1_AMPHIBIOUS | M1_AMORPHOUS | M1_NOHEAD | M1_FLY | M1_SEE_INVIS
            | M1_ACID | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_LORD | M2_MALE,
        M3_WAITFORU | M3_WANTSAMUL | M3_INFRAVISION, CLR_BRIGHT_GREEN),
    MON(PM_YEENOGHU, "Yeenoghu", S_DEMON, LVL(56, 18, -5, 80, -15),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 6), ATTK(AT_WEAP, AD_CONF, 2, 8),
          ATTK(AT_CLAW, AD_PLYS, 1, 6), ATTK(AT_MAGC, AD_MAGM, 2, 6), NO_ATTK,
          NO_ATTK),
        SIZ(900, 500, MS_ORC, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_LORD | M2_MALE | M2_COLLECT,
        M3_WANTSAMUL | M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_ORCUS, "Orcus", S_DEMON, LVL(66, 9, -6, 85, -20),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 3, 6), ATTK(AT_CLAW, AD_PHYS, 3, 4),
          ATTK(AT_CLAW, AD_PHYS, 3, 4), ATTK(AT_MAGC, AD_SPEL, 8, 6),
          ATTK(AT_STNG, AD_DRST, 2, 4), NO_ATTK),
        SIZ(1500, 500, MS_ORC, MZ_HUGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_PRINCE | M2_MALE | M2_COLLECT,
        M3_WAITFORU | M3_WANTSBOOK | M3_WANTSAMUL | M3_INFRAVISIBLE
            | M3_INFRAVISION,
        HI_LORD),
    MON(PM_GERYON, "Geryon", S_DEMON, LVL(72, 3, -3, 75, 15),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_PHYS, 3, 6), ATTK(AT_CLAW, AD_PHYS, 3, 6),
          ATTK(AT_STNG, AD_DRST, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 500, MS_BRIBE, MZ_HUGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS | M1_SLITHY,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_PRINCE | M2_MALE,
        M3_WANTSAMUL | M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_DISPATER, "Dispater", S_DEMON, LVL(78, 15, -2, 80, 15),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 6), ATTK(AT_MAGC, AD_SPEL, 6, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 500, MS_BRIBE, MZ_HUMAN), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS | M1_HUMANOID,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_PRINCE | M2_MALE | M2_COLLECT,
        M3_WANTSAMUL | M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    MON(PM_BAALZEBUB, "Baalzebub", S_DEMON, LVL(89, 9, -5, 85, 20),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_BITE, AD_DRST, 2, 6), ATTK(AT_GAZE, AD_STUN, 2, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 500, MS_BRIBE, MZ_LARGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_PRINCE | M2_MALE,
        M3_WANTSAMUL | M3_WAITFORU | M3_INFRAVISIBLE | M3_INFRAVISION,
        HI_LORD),
    MON(PM_ASMODEUS, "Asmodeus", S_DEMON, LVL(105, 12, -7, 90, 20),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_PHYS, 4, 4), ATTK(AT_MAGC, AD_COLD, 6, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1500, 500, MS_BRIBE, MZ_HUGE), MR_FIRE | MR_COLD | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_HUMANOID | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_STRONG
            | M2_NASTY | M2_PRINCE | M2_MALE,
        M3_WANTSAMUL | M3_WAITFORU | M3_INFRAVISIBLE | M3_INFRAVISION,
        HI_LORD),
    MON(PM_DEMOGORGON, "Demogorgon", S_DEMON, LVL(106, 15, -8, 95, -20),
        (G_HELL | G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_MAGC, AD_SPEL, 8, 6), ATTK(AT_STNG, AD_DRLI, 1, 4),
          ATTK(AT_CLAW, AD_DISE, 1, 6), ATTK(AT_CLAW, AD_DISE, 1, 6), NO_ATTK,
          NO_ATTK),
        SIZ(1500, 500, MS_GROWL, MZ_HUGE), MR_FIRE | MR_POISON, 0,
        M1_FLY | M1_SEE_INVIS | M1_NOHANDS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_PNAME | M2_NASTY
            | M2_PRINCE | M2_MALE,
        M3_WANTSAMUL | M3_INFRAVISIBLE | M3_INFRAVISION, HI_LORD),
    /* Riders -- the Four Horsemen of the Apocalypse ("War" == player)
     */
    MON(PM_DEATH, "Death", S_DEMON, LVL(30, 12, -5, 100, 0), (G_UNIQ | G_NOGEN),
        A(ATTK(AT_TUCH, AD_DETH, 8, 8), ATTK(AT_TUCH, AD_DETH, 8, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 1, MS_RIDER, MZ_HUMAN),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_HUMANOID | M1_REGEN | M1_SEE_INVIS | M1_TPORT_CNTRL,
        M2_NOPOLY | M2_STALK | M2_HOSTILE | M2_PNAME | M2_STRONG | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION | M3_DISPLACES, HI_LORD),
    MON(PM_PESTILENCE, "Pestilence", S_DEMON, LVL(30, 12, -5, 100, 0), (G_UNIQ | G_NOGEN),
        A(ATTK(AT_TUCH, AD_PEST, 8, 8), ATTK(AT_TUCH, AD_PEST, 8, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 1, MS_RIDER, MZ_HUMAN),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_HUMANOID | M1_REGEN | M1_SEE_INVIS | M1_TPORT_CNTRL,
        M2_NOPOLY | M2_STALK | M2_HOSTILE | M2_PNAME | M2_STRONG | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION | M3_DISPLACES, HI_LORD),
    MON(PM_FAMINE, "Famine", S_DEMON, LVL(30, 12, -5, 100, 0), (G_UNIQ | G_NOGEN),
        A(ATTK(AT_TUCH, AD_FAMN, 8, 8), ATTK(AT_TUCH, AD_FAMN, 8, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 1, MS_RIDER, MZ_HUMAN),
        MR_FIRE | MR_COLD | MR_ELEC | MR_SLEEP | MR_POISON | MR_STONE, 0,
        M1_FLY | M1_HUMANOID | M1_REGEN | M1_SEE_INVIS | M1_TPORT_CNTRL,
        M2_NOPOLY | M2_STALK | M2_HOSTILE | M2_PNAME | M2_STRONG | M2_NASTY,
        M3_INFRAVISIBLE | M3_INFRAVISION | M3_DISPLACES, HI_LORD),
/* other demons
 */

    MON(PM_DJINNI, "djinni", S_DEMON, LVL(7, 12, 4, 30, 0), (G_NOGEN | G_NOCORPSE),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(1500, 400, MS_DJINNI, MZ_HUMAN), MR_POISON | MR_STONE, 0,
        M1_HUMANOID | M1_FLY | M1_POIS, M2_NOPOLY | M2_STALK | M2_COLLECT,
        M3_INFRAVISIBLE, CLR_YELLOW),
    /*
     * sea monsters
     */
    MON(PM_JELLYFISH, "jellyfish", S_EEL, LVL(3, 3, 6, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_STNG, AD_DRST, 3, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(80, 20, MS_SILENT, MZ_SMALL), MR_POISON, MR_POISON,
        M1_SWIM | M1_AMPHIBIOUS | M1_SLITHY | M1_NOLIMBS | M1_NOHEAD
            | M1_NOTAKE | M1_POIS,
        M2_HOSTILE, 0, CLR_BLUE),
    MON(PM_PIRANHA, "piranha", S_EEL, LVL(5, 12, 4, 0, 0), (G_GENO | G_NOGEN | G_SGROUP),
        A(ATTK(AT_BITE, AD_PHYS, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(60, 30, MS_SILENT, MZ_SMALL), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_RED),
    MON(PM_SHARK, "shark", S_EEL, LVL(7, 12, 2, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_BITE, AD_PHYS, 5, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(500, 350, MS_SILENT, MZ_LARGE), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_THICK_HIDE | M1_NOTAKE,
        M2_HOSTILE, 0, CLR_GRAY),
    MON(PM_GIANT_EEL, "giant eel", S_EEL, LVL(5, 9, -1, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_BITE, AD_PHYS, 3, 6), ATTK(AT_TUCH, AD_WRAP, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(200, 250, MS_SILENT, MZ_HUGE), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_CYAN),
    MON(PM_ELECTRIC_EEL, "electric eel", S_EEL, LVL(7, 10, -3, 0, 0), (G_GENO | G_NOGEN),
        A(ATTK(AT_BITE, AD_ELEC, 4, 6), ATTK(AT_TUCH, AD_WRAP, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(200, 250, MS_SILENT, MZ_HUGE), MR_ELEC, MR_ELEC,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_SLITHY | M1_NOLIMBS
            | M1_CARNIVORE | M1_OVIPAROUS | M1_NOTAKE,
        M2_HOSTILE, M3_INFRAVISIBLE, CLR_BRIGHT_BLUE),
    MON(PM_KRAKEN, "kraken", S_EEL, LVL(20, 3, 6, 0, -3), (G_GENO | G_NOGEN),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 4), ATTK(AT_CLAW, AD_PHYS, 2, 4),
          ATTK(AT_HUGS, AD_WRAP, 2, 6), ATTK(AT_BITE, AD_PHYS, 5, 4), NO_ATTK,
          NO_ATTK),
        SIZ(1800, 1000, MS_SILENT, MZ_HUGE), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_NOPOLY | M2_HOSTILE | M2_STRONG, M3_INFRAVISIBLE, CLR_RED),
    /*
     * lizards, &c
     */
    MON(PM_NEWT, "newt", S_LIZARD, LVL(0, 6, 8, 0, 0), (G_GENO | 5),
        A(ATTK(AT_BITE, AD_PHYS, 1, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 20, MS_SILENT, MZ_TINY), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_YELLOW),
    MON(PM_GECKO, "gecko", S_LIZARD, LVL(1, 6, 8, 0, 0), (G_GENO | 5),
        A(ATTK(AT_BITE, AD_PHYS, 1, 3), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 20, MS_SQEEK, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, 0, CLR_GREEN),
    MON(PM_IGUANA, "iguana", S_LIZARD, LVL(2, 6, 7, 0, 0), (G_GENO | 5),
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(30, 30, MS_SILENT, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_BABY_CROCODILE, "baby crocodile", S_LIZARD, LVL(3, 6, 7, 0, 0), G_GENO,
        A(ATTK(AT_BITE, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(200, 200, MS_SILENT, MZ_MEDIUM), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_LIZARD, "lizard", S_LIZARD, LVL(5, 6, 6, 10, 0), (G_GENO | 5),
        A(ATTK(AT_BITE, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(10, 40, MS_SILENT, MZ_TINY), MR_STONE, MR_STONE,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE, M2_HOSTILE, 0, CLR_GREEN),
    MON(PM_CHAMELEON, "chameleon", S_LIZARD, LVL(6, 5, 6, 10, 0), (G_GENO | 2),
        A(ATTK(AT_BITE, AD_PHYS, 4, 2), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(100, 100, MS_SILENT, MZ_TINY), 0, 0,
        M1_ANIMAL | M1_NOHANDS | M1_CARNIVORE,
        M2_NOPOLY | M2_HOSTILE | M2_SHAPESHIFTER, 0, CLR_BROWN),
    MON(PM_CROCODILE, "crocodile", S_LIZARD, LVL(6, 9, 5, 0, 0), (G_GENO | 1),
        A(ATTK(AT_BITE, AD_PHYS, 4, 2), ATTK(AT_CLAW, AD_PHYS, 1, 12),
          NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_SILENT, MZ_LARGE), 0, 0,
        M1_SWIM | M1_AMPHIBIOUS | M1_ANIMAL | M1_THICK_HIDE | M1_NOHANDS
            | M1_OVIPAROUS | M1_CARNIVORE,
        M2_STRONG | M2_HOSTILE, 0, CLR_BROWN),
    MON(PM_SALAMANDER, "salamander", S_LIZARD, LVL(8, 12, -1, 0, -9), (G_HELL | 1),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 8), ATTK(AT_TUCH, AD_FIRE, 1, 6),
          ATTK(AT_HUGS, AD_PHYS, 2, 6), ATTK(AT_HUGS, AD_FIRE, 3, 6), NO_ATTK,
          NO_ATTK),
        SIZ(1500, 400, MS_MUMBLE, MZ_HUMAN), MR_SLEEP | MR_FIRE, MR_FIRE,
        M1_HUMANOID | M1_SLITHY | M1_THICK_HIDE | M1_POIS,
        M2_STALK | M2_HOSTILE | M2_COLLECT | M2_MAGIC, M3_INFRAVISIBLE,
        CLR_ORANGE),

    /*
     * dummy monster needed for visual interface
     */
    /* (marking it unique prevents figurines)
     */
    MON(PM_LONG_WORM_TAIL, "long worm tail", S_WORM_TAIL, LVL(0, 0, 0, 0, 0),
        (G_NOGEN | G_NOCORPSE | G_UNIQ),
        A(NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, 0, 0), 0, 0, 0L, M2_NOPOLY, 0, CLR_BROWN),

    /* Note:
     * Worm tail must be between the normal monsters and the special
     * quest & pseudo-character ones because an optimization in the
     * random monster selection code assumes everything beyond here
     * has the G_NOGEN and M2_NOPOLY attributes.
     */

    /*
     * character classes
     */
    MON(PM_ARCHEOLOGIST, "archeologist", S_HUMAN, LVL(10, 12, 10, 1, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_TUNNEL | M1_NEEDPICK | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_BARBARIAN, "barbarian", S_HUMAN, LVL(10, 12, 10, 1, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_CAVEMAN, "caveman", S_HUMAN, LVL(10, 12, 10, 0, 1), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_CAVEWOMAN, "cavewoman", S_HUMAN, LVL(10, 12, 10, 0, 1), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_HEALER, "healer", S_HUMAN, LVL(10, 12, 10, 1, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_KNIGHT, "knight", S_HUMAN, LVL(10, 12, 10, 1, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_MONK, "monk", S_HUMAN, LVL(10, 12, 10, 2, 0), G_NOGEN,
        A(ATTK(AT_CLAW, AD_PHYS, 1, 8), ATTK(AT_KICK, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_HERBIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT | M2_MALE,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_PRIEST, "priest", S_HUMAN, LVL(10, 12, 10, 2, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_MALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_PRIESTESS, "priestess", S_HUMAN, LVL(10, 12, 10, 2, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_RANGER, "ranger", S_HUMAN, LVL(10, 12, 10, 2, -3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_ROGUE, "rogue", S_HUMAN, LVL(10, 12, 10, 1, -3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_GREEDY | M2_JEWELS | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_SAMURAI, "samurai", S_HUMAN, LVL(10, 12, 10, 1, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_TOURIST, "tourist", S_HUMAN, LVL(10, 12, 10, 1, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT, M3_INFRAVISIBLE,
        HI_DOMESTIC),
    MON(PM_VALKYRIE, "valkyrie", S_HUMAN, LVL(10, 12, 10, 1, -1), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), MR_COLD, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_FEMALE | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_WIZARD, "wizard", S_HUMAN, LVL(10, 12, 10, 3, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    /*
     * quest leaders
     */
    MON(PM_LORD_CARNARVON, "Lord Carnarvon", S_HUMAN, LVL(20, 12, 0, 30, 20), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_TUNNEL | M1_NEEDPICK | M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_PELIAS, "Pelias", S_HUMAN, LVL(20, 12, 0, 30, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_SHAMAN_KARNOV, "Shaman Karnov", S_HUMAN, LVL(20, 12, 0, 30, 20), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
#if 0 /* OBSOLETE */
 /* Two for elves - one of each sex.
  */
    MON(PM_EARENDIL, "Earendil", S_HUMAN,
 LVL(20, 12, 0, 50, -20), (G_NOGEN | G_UNIQ),
 A(ATTK(AT_WEAP, AD_PHYS, 1, 8),
   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(WT_ELF, 350, MS_LEADER, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
 M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
 M2_NOPOLY | M2_ELF | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG
          | M2_MALE | M2_COLLECT | M2_MAGIC,
 M3_CLOSE | M3_INFRAVISION | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_ELWING, "Elwing", S_HUMAN,
 LVL(20, 12, 0, 50, -20), (G_NOGEN | G_UNIQ),
 A(ATTK(AT_WEAP, AD_PHYS, 1, 8),
   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(WT_ELF, 350, MS_LEADER, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
 M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
 M2_NOPOLY | M2_ELF | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG
          | M2_FEMALE | M2_COLLECT | M2_MAGIC,
 M3_CLOSE | M3_INFRAVISION | M3_INFRAVISIBLE, HI_LORD),
#endif
    MON(PM_HIPPOCRATES, "Hippocrates", S_HUMAN, LVL(20, 12, 0, 40, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_KING_ARTHUR, "King Arthur", S_HUMAN, LVL(20, 12, 0, 40, 20), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_GRAND_MASTER, "Grand Master", S_HUMAN, LVL(25, 12, 0, 70, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_PHYS, 4, 10), ATTK(AT_KICK, AD_PHYS, 2, 8),
          ATTK(AT_MAGC, AD_CLRC, 2, 8), ATTK(AT_MAGC, AD_CLRC, 2, 8), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN),
        MR_FIRE | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_HUMANOID | M1_SEE_INVIS | M1_HERBIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_MALE | M2_NASTY
            | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, CLR_BLACK),
    MON(PM_ARCH_PRIEST, "Arch Priest", S_HUMAN, LVL(25, 12, 7, 70, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 10), ATTK(AT_KICK, AD_PHYS, 2, 8),
          ATTK(AT_MAGC, AD_CLRC, 2, 8), ATTK(AT_MAGC, AD_CLRC, 2, 8), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN),
        MR_FIRE | MR_ELEC | MR_SLEEP | MR_POISON, 0,
        M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_MALE | M2_COLLECT
            | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, CLR_WHITE),
    MON(PM_ORION, "Orion", S_HUMAN, LVL(20, 12, 0, 30, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(2200, 700, MS_LEADER, MZ_HUGE), 0, 0,
        M1_HUMANOID | M1_OMNIVORE | M1_SEE_INVIS | M1_SWIM | M1_AMPHIBIOUS,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISION | M3_INFRAVISIBLE, HI_LORD),
    /* Note: Master of Thieves is also the Tourist's nemesis.
     */
    MON(PM_MASTER_OF_THIEVES, "Master of Thieves", S_HUMAN, LVL(20, 12, 0, 30, -20),
        (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 6),
          ATTK(AT_CLAW, AD_SAMU, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_MALE | M2_GREEDY
            | M2_JEWELS | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_LORD_SATO, "Lord Sato", S_HUMAN, LVL(20, 12, 0, 30, 20), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_TWOFLOWER, "Twoflower", S_HUMAN, LVL(20, 12, 10, 20, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_PEACEFUL | M2_STRONG | M2_MALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_NORN, "Norn", S_HUMAN, LVL(20, 12, 0, 80, 0), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1800, 550, MS_LEADER, MZ_HUGE), MR_COLD, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_FEMALE
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_NEFERET_THE_GREEN, "Neferet the Green", S_HUMAN, LVL(20, 12, 0, 60, 0),
        (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_MAGC, AD_SPEL, 2, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_LEADER, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_FEMALE | M2_PNAME | M2_PEACEFUL | M2_STRONG
            | M2_COLLECT | M2_MAGIC,
        M3_CLOSE | M3_INFRAVISIBLE, CLR_GREEN),
    /*
     * quest nemeses
     */
    MON(PM_MINION_OF_HUHETOTL, "Minion of Huhetotl", S_DEMON, LVL(16, 12, -2, 75, -14),
        (G_NOCORPSE | G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 8, 4), ATTK(AT_WEAP, AD_PHYS, 4, 6),
          ATTK(AT_MAGC, AD_SPEL, 0, 0), ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_LARGE),
        MR_FIRE | MR_POISON | MR_STONE, 0, M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_STALK | M2_HOSTILE | M2_STRONG | M2_NASTY
            | M2_COLLECT,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISION | M3_INFRAVISIBLE,
        CLR_RED),
    MON(PM_THOTH_AMON, "Thoth Amon", S_HUMAN, LVL(16, 12, 0, 10, -14),
        (G_NOGEN | G_UNIQ | G_NOCORPSE),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_MAGC, AD_SPEL, 0, 0),
          ATTK(AT_MAGC, AD_SPEL, 0, 0), ATTK(AT_CLAW, AD_SAMU, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_HUMAN), MR_POISON | MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_STRONG | M2_MALE | M2_STALK
            | M2_HOSTILE | M2_NASTY | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
    /* Multi-headed, possessing the breath attacks of all the other dragons
     * (selected at random when attacking).
     */
    MON(PM_CHROMATIC_DRAGON, "Chromatic Dragon", S_DRAGON, LVL(16, 12, 0, 30, -14),
        (G_NOGEN | G_UNIQ),
        A(ATTK(AT_BREA, AD_RBRE, 6, 8), ATTK(AT_MAGC, AD_SPEL, 0, 0),
          ATTK(AT_CLAW, AD_SAMU, 2, 8), ATTK(AT_BITE, AD_PHYS, 4, 8),
          ATTK(AT_BITE, AD_PHYS, 4, 8), ATTK(AT_STNG, AD_PHYS, 1, 6)),
        SIZ(WT_DRAGON, 1700, MS_NEMESIS, MZ_GIGANTIC),
        MR_FIRE | MR_COLD | MR_SLEEP | MR_DISINT | MR_ELEC | MR_POISON
            | MR_ACID | MR_STONE,
        MR_FIRE | MR_COLD | MR_SLEEP | MR_DISINT | MR_ELEC | MR_POISON
            | MR_STONE,
        M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_HOSTILE | M2_FEMALE | M2_STALK | M2_STRONG | M2_NASTY
            | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
#if 0 /* OBSOLETE */
    MON(PM_GOBLIN_KING, "Goblin King", S_ORC,
 LVL(15, 12, 10, 0, -15), (G_NOGEN | G_UNIQ),
 A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 6),
   ATTK(AT_CLAW, AD_SAMU, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(750, 350, MS_NEMESIS, MZ_HUMAN), 0, 0,
 M1_HUMANOID | M1_OMNIVORE,
 M2_NOPOLY | M2_ORC | M2_HOSTILE | M2_STRONG | M2_STALK | M2_NASTY
          | M2_MALE | M2_GREEDY | M2_JEWELS | M2_COLLECT | M2_MAGIC,
 M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISION | M3_INFRAVISIBLE,
        HI_LORD),
#endif
    MON(PM_CYCLOPS, "Cyclops", S_GIANT, LVL(18, 12, 0, 0, -15), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 4, 8), ATTK(AT_WEAP, AD_PHYS, 4, 8),
          ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(1900, 700, MS_NEMESIS, MZ_HUGE), MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_GIANT | M2_STRONG | M2_ROCKTHROW | M2_STALK
            | M2_HOSTILE | M2_NASTY | M2_MALE | M2_JEWELS | M2_COLLECT,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISION | M3_INFRAVISIBLE,
        CLR_GRAY),
    MON(PM_IXOTH, "Ixoth", S_DRAGON, LVL(15, 12, -1, 20, -14), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_BREA, AD_FIRE, 8, 6), ATTK(AT_BITE, AD_PHYS, 4, 8),
          ATTK(AT_MAGC, AD_SPEL, 0, 0), ATTK(AT_CLAW, AD_PHYS, 2, 4),
          ATTK(AT_CLAW, AD_SAMU, 2, 4), NO_ATTK),
        SIZ(WT_DRAGON, 1600, MS_NEMESIS, MZ_GIGANTIC), MR_FIRE | MR_STONE,
        MR_FIRE,
        M1_FLY | M1_THICK_HIDE | M1_NOHANDS | M1_CARNIVORE | M1_SEE_INVIS,
        M2_NOPOLY | M2_MALE | M2_PNAME | M2_HOSTILE | M2_STRONG | M2_NASTY
            | M2_STALK | M2_GREEDY | M2_JEWELS | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, CLR_RED),
    MON(PM_MASTER_KAEN, "Master Kaen", S_HUMAN, LVL(25, 12, -10, 10, -20), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_PHYS, 16, 2), ATTK(AT_CLAW, AD_PHYS, 16, 2),
          ATTK(AT_MAGC, AD_CLRC, 0, 0), ATTK(AT_CLAW, AD_SAMU, 1, 4), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_HUMAN), MR_POISON | MR_STONE,
        MR_POISON, M1_HUMANOID | M1_HERBIVORE | M1_SEE_INVIS,
        M2_NOPOLY | M2_HUMAN | M2_MALE | M2_PNAME | M2_HOSTILE | M2_STRONG
            | M2_NASTY | M2_STALK | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
    MON(PM_NALZOK, "Nalzok", S_DEMON, LVL(16, 12, -2, 85, -127),
        (G_NOGEN | G_UNIQ | G_NOCORPSE),
        A(ATTK(AT_WEAP, AD_PHYS, 8, 4), ATTK(AT_WEAP, AD_PHYS, 4, 6),
          ATTK(AT_MAGC, AD_SPEL, 0, 0), ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_LARGE),
        MR_FIRE | MR_POISON | MR_STONE, 0, M1_FLY | M1_SEE_INVIS | M1_POIS,
        M2_NOPOLY | M2_DEMON | M2_MALE | M2_PNAME | M2_HOSTILE | M2_STRONG
            | M2_STALK | M2_NASTY | M2_COLLECT,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISION | M3_INFRAVISIBLE,
        CLR_RED),
    MON(PM_SCORPIUS, "Scorpius", S_SPIDER, LVL(15, 12, 10, 0, -15), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_CLAW, AD_PHYS, 2, 6), ATTK(AT_CLAW, AD_SAMU, 2, 6),
          ATTK(AT_STNG, AD_DISE, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(750, 350, MS_NEMESIS, MZ_HUMAN), MR_POISON | MR_STONE, MR_POISON,
        M1_ANIMAL | M1_NOHANDS | M1_OVIPAROUS | M1_POIS | M1_CARNIVORE,
        M2_NOPOLY | M2_MALE | M2_PNAME | M2_HOSTILE | M2_STRONG | M2_STALK
            | M2_NASTY | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU, HI_LORD),
    MON(PM_MASTER_ASSASSIN, "Master Assassin", S_HUMAN, LVL(15, 12, 0, 30, 18),
        (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_DRST, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 8),
          ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_HUMAN), MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_MALE | M2_HOSTILE | M2_STALK
            | M2_NASTY | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
    /* A renegade daimyo who led a 13 year civil war against the shogun
     * of his time.
     */
    MON(PM_ASHIKAGA_TAKAUJI, "Ashikaga Takauji", S_HUMAN, LVL(15, 12, 0, 40, -13),
        (G_NOGEN | G_UNIQ | G_NOCORPSE),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 6), ATTK(AT_WEAP, AD_PHYS, 2, 6),
          ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_HUMAN), MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PNAME | M2_HOSTILE | M2_STRONG | M2_STALK
            | M2_NASTY | M2_MALE | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, HI_LORD),
    /*
     * Note: the Master of Thieves was defined above.
     */
    MON(PM_LORD_SURTUR, "Lord Surtur", S_GIANT, LVL(15, 12, 2, 50, 12), (G_NOGEN | G_UNIQ),
        A(ATTK(AT_WEAP, AD_PHYS, 2, 10), ATTK(AT_WEAP, AD_PHYS, 2, 10),
          ATTK(AT_CLAW, AD_SAMU, 2, 6), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(2250, 850, MS_NEMESIS, MZ_HUGE), MR_FIRE | MR_STONE, MR_FIRE,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_GIANT | M2_MALE | M2_PNAME | M2_HOSTILE | M2_STALK
            | M2_STRONG | M2_NASTY | M2_ROCKTHROW | M2_JEWELS | M2_COLLECT,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISION | M3_INFRAVISIBLE,
        HI_LORD),
    MON(PM_DARK_ONE, "Dark One", S_HUMAN, LVL(15, 12, 0, 80, -10),
        (G_NOGEN | G_UNIQ | G_NOCORPSE),
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6),
          ATTK(AT_CLAW, AD_SAMU, 1, 4), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_NEMESIS, MZ_HUMAN), MR_STONE, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_STRONG | M2_HOSTILE | M2_STALK | M2_NASTY
            | M2_COLLECT | M2_MAGIC,
        M3_WANTSARTI | M3_WAITFORU | M3_INFRAVISIBLE, CLR_BLACK),
    /*
     * quest "guardians"
     */
    MON(PM_STUDENT, "student", S_HUMAN, LVL(5, 12, 10, 10, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_TUNNEL | M1_NEEDPICK | M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_CHIEFTAIN, "chieftain", S_HUMAN, LVL(5, 12, 10, 10, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_NEANDERTHAL, "neanderthal", S_HUMAN, LVL(5, 12, 10, 10, 1), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 2, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
#if 0 /* OBSOLETE */
    MON(PM_HIGH_ELF, "High-elf", S_HUMAN,
 LVL(5, 12, 10, 10, -7), G_NOGEN,
 A(ATTK(AT_WEAP, AD_PHYS, 2, 4), ATTK(AT_MAGC, AD_CLRC, 0, 0),
   NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
 SIZ(WT_ELF, 350, MS_GUARDIAN, MZ_HUMAN), MR_SLEEP, MR_SLEEP,
 M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
 M2_NOPOLY | M2_ELF | M2_PEACEFUL | M2_COLLECT,
 M3_INFRAVISION | M3_INFRAVISIBLE, HI_DOMESTIC),
#endif
    MON(PM_ATTENDANT, "attendant", S_HUMAN, LVL(5, 12, 10, 10, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), MR_POISON, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_PAGE, "page", S_HUMAN, LVL(5, 12, 10, 10, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_ABBOT, "abbot", S_HUMAN, LVL(5, 12, 10, 20, 0), G_NOGEN,
        A(ATTK(AT_CLAW, AD_PHYS, 8, 2), ATTK(AT_KICK, AD_STUN, 3, 2),
          ATTK(AT_MAGC, AD_CLRC, 0, 0), NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_HERBIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_ACOLYTE, "acolyte", S_HUMAN, LVL(5, 12, 10, 20, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_MAGC, AD_CLRC, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_HUNTER, "hunter", S_HUMAN, LVL(5, 12, 10, 10, -7), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 4), NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK,
          NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_SEE_INVIS | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISION | M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_THUG, "thug", S_HUMAN, LVL(5, 12, 10, 10, -3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_WEAP, AD_PHYS, 1, 6), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_PEACEFUL
                                       | M2_STRONG | M2_GREEDY | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_NINJA, "ninja", S_HUMAN, LVL(5, 12, 10, 10, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_HUMANOID, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_HOSTILE | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_ROSHI, "roshi", S_HUMAN, LVL(5, 12, 10, 10, 3), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE,
        M2_NOPOLY | M2_HUMAN | M2_PEACEFUL | M2_STRONG | M2_COLLECT,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_GUIDE, "guide", S_HUMAN, LVL(5, 12, 10, 20, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_PEACEFUL
                                       | M2_STRONG | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_WARRIOR, "warrior", S_HUMAN, LVL(5, 12, 10, 10, -1), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 8), ATTK(AT_WEAP, AD_PHYS, 1, 8), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_PEACEFUL
                                       | M2_STRONG | M2_COLLECT | M2_FEMALE,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    MON(PM_APPRENTICE, "apprentice", S_HUMAN, LVL(5, 12, 10, 30, 0), G_NOGEN,
        A(ATTK(AT_WEAP, AD_PHYS, 1, 6), ATTK(AT_MAGC, AD_SPEL, 0, 0), NO_ATTK,
          NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(WT_HUMAN, 400, MS_GUARDIAN, MZ_HUMAN), 0, 0,
        M1_HUMANOID | M1_OMNIVORE, M2_NOPOLY | M2_HUMAN | M2_PEACEFUL
                                       | M2_STRONG | M2_COLLECT | M2_MAGIC,
        M3_INFRAVISIBLE, HI_DOMESTIC),
    /*
     * array terminator
     */
    MON(999, 0, 0, LVL(0, 0, 0, 0, 0), (0),
        A(NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK, NO_ATTK),
        SIZ(0, 0, 0, 0), 0, 0, 0L, 0L, 0, 0)
};

struct attack sa_yes[NATTK] = SEDUCTION_ATTACKS_YES;
struct attack sa_no[NATTK] = SEDUCTION_ATTACKS_NO;

#endif


/* This source file is generated by 'makedefs'.  Do not edit. */
    
const char* PM_TYPES[] = {
    "PM.GIANT_ANT",
    "PM.KILLER_BEE",
    "PM.SOLDIER_ANT",
    "PM.FIRE_ANT",
    "PM.GIANT_BEETLE",
    "PM.QUEEN_BEE",
    "PM.ACID_BLOB",
    "PM.QUIVERING_BLOB",
    "PM.GELATINOUS_CUBE",
    "PM.CHICKATRICE",
    "PM.COCKATRICE",
    "PM.PYROLISK",
    "PM.JACKAL",
    "PM.FOX",
    "PM.COYOTE",
    "PM.WEREJACKAL",
    "PM.LITTLE_DOG",
    "PM.DINGO",
    "PM.DOG",
    "PM.LARGE_DOG",
    "PM.WOLF",
    "PM.WEREWOLF",
    "PM.WINTER_WOLF_CUB",
    "PM.WARG",
    "PM.WINTER_WOLF",
    "PM.HELL_HOUND_PUP",
    "PM.HELL_HOUND",
    "PM.GAS_SPORE",
    "PM.FLOATING_EYE",
    "PM.FREEZING_SPHERE",
    "PM.FLAMING_SPHERE",
    "PM.SHOCKING_SPHERE",
    "PM.KITTEN",
    "PM.HOUSECAT",
    "PM.JAGUAR",
    "PM.LYNX",
    "PM.PANTHER",
    "PM.LARGE_CAT",
    "PM.TIGER",
    "PM.GREMLIN",
    "PM.GARGOYLE",
    "PM.WINGED_GARGOYLE",
    "PM.HOBBIT",
    "PM.DWARF",
    "PM.BUGBEAR",
    "PM.DWARF_LORD",
    "PM.DWARF_KING",
    "PM.MIND_FLAYER",
    "PM.MASTER_MIND_FLAYER",
    "PM.MANES",
    "PM.HOMUNCULUS",
    "PM.IMP",
    "PM.LEMURE",
    "PM.QUASIT",
    "PM.TENGU",
    "PM.BLUE_JELLY",
    "PM.SPOTTED_JELLY",
    "PM.OCHRE_JELLY",
    "PM.KOBOLD",
    "PM.LARGE_KOBOLD",
    "PM.KOBOLD_LORD",
    "PM.KOBOLD_SHAMAN",
    "PM.LEPRECHAUN",
    "PM.SMALL_MIMIC",
    "PM.LARGE_MIMIC",
    "PM.GIANT_MIMIC",
    "PM.WOOD_NYMPH",
    "PM.WATER_NYMPH",
    "PM.MOUNTAIN_NYMPH",
    "PM.GOBLIN",
    "PM.HOBGOBLIN",
    "PM.ORC",
    "PM.HILL_ORC",
    "PM.MORDOR_ORC",
    "PM.URUK_HAI",
    "PM.ORC_SHAMAN",
    "PM.ORC_CAPTAIN",
    "PM.ROCK_PIERCER",
    "PM.IRON_PIERCER",
    "PM.GLASS_PIERCER",
    "PM.ROTHE",
    "PM.MUMAK",
    "PM.LEOCROTTA",
    "PM.WUMPUS",
    "PM.TITANOTHERE",
    "PM.BALUCHITHERIUM",
    "PM.MASTODON",
    "PM.SEWER_RAT",
    "PM.GIANT_RAT",
    "PM.RABID_RAT",
    "PM.WERERAT",
    "PM.ROCK_MOLE",
    "PM.WOODCHUCK",
    "PM.CAVE_SPIDER",
    "PM.CENTIPEDE",
    "PM.GIANT_SPIDER",
    "PM.SCORPION",
    "PM.LURKER_ABOVE",
    "PM.TRAPPER",
    "PM.PONY",
    "PM.WHITE_UNICORN",
    "PM.GRAY_UNICORN",
    "PM.BLACK_UNICORN",
    "PM.HORSE",
    "PM.WARHORSE",
    "PM.FOG_CLOUD",
    "PM.DUST_VORTEX",
    "PM.ICE_VORTEX",
    "PM.ENERGY_VORTEX",
    "PM.STEAM_VORTEX",
    "PM.FIRE_VORTEX",
    "PM.BABY_LONG_WORM",
    "PM.BABY_PURPLE_WORM",
    "PM.LONG_WORM",
    "PM.PURPLE_WORM",
    "PM.GRID_BUG",
    "PM.XAN",
    "PM.YELLOW_LIGHT",
    "PM.BLACK_LIGHT",
    "PM.ZRUTY",
    "PM.COUATL",
    "PM.ALEAX",
    "PM.ANGEL",
    "PM.KI_RIN",
    "PM.ARCHON",
    "PM.BAT",
    "PM.GIANT_BAT",
    "PM.RAVEN",
    "PM.VAMPIRE_BAT",
    "PM.PLAINS_CENTAUR",
    "PM.FOREST_CENTAUR",
    "PM.MOUNTAIN_CENTAUR",
    "PM.BABY_GRAY_DRAGON",
    "PM.BABY_SILVER_DRAGON",
    "PM.BABY_RED_DRAGON",
    "PM.BABY_WHITE_DRAGON",
    "PM.BABY_ORANGE_DRAGON",
    "PM.BABY_BLACK_DRAGON",
    "PM.BABY_BLUE_DRAGON",
    "PM.BABY_GREEN_DRAGON",
    "PM.BABY_YELLOW_DRAGON",
    "PM.GRAY_DRAGON",
    "PM.SILVER_DRAGON",
    "PM.RED_DRAGON",
    "PM.WHITE_DRAGON",
    "PM.ORANGE_DRAGON",
    "PM.BLACK_DRAGON",
    "PM.BLUE_DRAGON",
    "PM.GREEN_DRAGON",
    "PM.YELLOW_DRAGON",
    "PM.STALKER",
    "PM.AIR_ELEMENTAL",
    "PM.FIRE_ELEMENTAL",
    "PM.EARTH_ELEMENTAL",
    "PM.WATER_ELEMENTAL",
    "PM.LICHEN",
    "PM.BROWN_MOLD",
    "PM.YELLOW_MOLD",
    "PM.GREEN_MOLD",
    "PM.RED_MOLD",
    "PM.SHRIEKER",
    "PM.VIOLET_FUNGUS",
    "PM.GNOME",
    "PM.GNOME_LORD",
    "PM.GNOMISH_WIZARD",
    "PM.GNOME_KING",
    "PM.GIANT",
    "PM.STONE_GIANT",
    "PM.HILL_GIANT",
    "PM.FIRE_GIANT",
    "PM.FROST_GIANT",
    "PM.ETTIN",
    "PM.STORM_GIANT",
    "PM.TITAN",
    "PM.MINOTAUR",
    "PM.JABBERWOCK",
    "PM.KEYSTONE_KOP",
    "PM.KOP_SERGEANT",
    "PM.KOP_LIEUTENANT",
    "PM.KOP_KAPTAIN",
    "PM.LICH",
    "PM.DEMILICH",
    "PM.MASTER_LICH",
    "PM.ARCH_LICH",
    "PM.KOBOLD_MUMMY",
    "PM.GNOME_MUMMY",
    "PM.ORC_MUMMY",
    "PM.DWARF_MUMMY",
    "PM.ELF_MUMMY",
    "PM.HUMAN_MUMMY",
    "PM.ETTIN_MUMMY",
    "PM.GIANT_MUMMY",
    "PM.RED_NAGA_HATCHLING",
    "PM.BLACK_NAGA_HATCHLING",
    "PM.GOLDEN_NAGA_HATCHLING",
    "PM.GUARDIAN_NAGA_HATCHLING",
    "PM.RED_NAGA",
    "PM.BLACK_NAGA",
    "PM.GOLDEN_NAGA",
    "PM.GUARDIAN_NAGA",
    "PM.OGRE",
    "PM.OGRE_LORD",
    "PM.OGRE_KING",
    "PM.GRAY_OOZE",
    "PM.BROWN_PUDDING",
    "PM.GREEN_SLIME",
    "PM.BLACK_PUDDING",
    "PM.QUANTUM_MECHANIC",
    "PM.RUST_MONSTER",
    "PM.DISENCHANTER",
    "PM.GARTER_SNAKE",
    "PM.SNAKE",
    "PM.WATER_MOCCASIN",
    "PM.PYTHON",
    "PM.PIT_VIPER",
    "PM.COBRA",
    "PM.TROLL",
    "PM.ICE_TROLL",
    "PM.ROCK_TROLL",
    "PM.WATER_TROLL",
    "PM.OLOG_HAI",
    "PM.UMBER_HULK",
    "PM.VAMPIRE",
    "PM.VAMPIRE_LORD",
    "PM.VLAD_THE_IMPALER",
    "PM.BARROW_WIGHT",
    "PM.WRAITH",
    "PM.NAZGUL",
    "PM.XORN",
    "PM.MONKEY",
    "PM.APE",
    "PM.OWLBEAR",
    "PM.YETI",
    "PM.CARNIVOROUS_APE",
    "PM.SASQUATCH",
    "PM.KOBOLD_ZOMBIE",
    "PM.GNOME_ZOMBIE",
    "PM.ORC_ZOMBIE",
    "PM.DWARF_ZOMBIE",
    "PM.ELF_ZOMBIE",
    "PM.HUMAN_ZOMBIE",
    "PM.ETTIN_ZOMBIE",
    "PM.GHOUL",
    "PM.GIANT_ZOMBIE",
    "PM.SKELETON",
    "PM.STRAW_GOLEM",
    "PM.PAPER_GOLEM",
    "PM.ROPE_GOLEM",
    "PM.GOLD_GOLEM",
    "PM.LEATHER_GOLEM",
    "PM.WOOD_GOLEM",
    "PM.FLESH_GOLEM",
    "PM.CLAY_GOLEM",
    "PM.STONE_GOLEM",
    "PM.GLASS_GOLEM",
    "PM.IRON_GOLEM",
    "PM.HUMAN",
    "PM.HUMAN_WERERAT",
    "PM.HUMAN_WEREJACKAL",
    "PM.HUMAN_WEREWOLF",
    "PM.ELF",
    "PM.WOODLAND_ELF",
    "PM.GREEN_ELF",
    "PM.GREY_ELF",
    "PM.ELF_LORD",
    "PM.ELVENKING",
    "PM.DOPPELGANGER",
    "PM.SHOPKEEPER",
    "PM.GUARD",
    "PM.PRISONER",
    "PM.ORACLE",
    "PM.ALIGNED_PRIEST",
    "PM.HIGH_PRIEST",
    "PM.SOLDIER",
    "PM.SERGEANT",
    "PM.NURSE",
    "PM.LIEUTENANT",
    "PM.CAPTAIN",
    "PM.WATCHMAN",
    "PM.WATCH_CAPTAIN",
    "PM.MEDUSA",
    "PM.WIZARD_OF_YENDOR",
    "PM.CROESUS",
    "PM.GHOST",
    "PM.SHADE",
    "PM.WATER_DEMON",
    "PM.SUCCUBUS",
    "PM.HORNED_DEVIL",
    "PM.INCUBUS",
    "PM.ERINYS",
    "PM.BARBED_DEVIL",
    "PM.MARILITH",
    "PM.VROCK",
    "PM.HEZROU",
    "PM.BONE_DEVIL",
    "PM.ICE_DEVIL",
    "PM.NALFESHNEE",
    "PM.PIT_FIEND",
    "PM.SANDESTIN",
    "PM.BALROG",
    "PM.JUIBLEX",
    "PM.YEENOGHU",
    "PM.ORCUS",
    "PM.GERYON",
    "PM.DISPATER",
    "PM.BAALZEBUB",
    "PM.ASMODEUS",
    "PM.DEMOGORGON",
    "PM.DEATH",
    "PM.PESTILENCE",
    "PM.FAMINE",
    "PM.DJINNI",
    "PM.JELLYFISH",
    "PM.PIRANHA",
    "PM.SHARK",
    "PM.GIANT_EEL",
    "PM.ELECTRIC_EEL",
    "PM.KRAKEN",
    "PM.NEWT",
    "PM.GECKO",
    "PM.IGUANA",
    "PM.BABY_CROCODILE",
    "PM.LIZARD",
    "PM.CHAMELEON",
    "PM.CROCODILE",
    "PM.SALAMANDER",
    "PM.LONG_WORM_TAIL",
    "PM.ARCHEOLOGIST",
    "PM.BARBARIAN",
    "PM.CAVEMAN",
    "PM.CAVEWOMAN",
    "PM.HEALER",
    "PM.KNIGHT",
    "PM.MONK",
    "PM.PRIEST",
    "PM.PRIESTESS",
    "PM.RANGER",
    "PM.ROGUE",
    "PM.SAMURAI",
    "PM.TOURIST",
    "PM.VALKYRIE",
    "PM.WIZARD",
    "PM.LORD_CARNARVON",
    "PM.PELIAS",
    "PM.SHAMAN_KARNOV",
    "PM.HIPPOCRATES",
    "PM.KING_ARTHUR",
    "PM.GRAND_MASTER",
    "PM.ARCH_PRIEST",
    "PM.ORION",
    "PM.MASTER_OF_THIEVES",
    "PM.LORD_SATO",
    "PM.TWOFLOWER",
    "PM.NORN",
    "PM.NEFERET_THE_GREEN",
    "PM.MINION_OF_HUHETOTL",
    "PM.THOTH_AMON",
    "PM.CHROMATIC_DRAGON",
    "PM.CYCLOPS",
    "PM.IXOTH",
    "PM.MASTER_KAEN",
    "PM.NALZOK",
    "PM.SCORPIUS",
    "PM.MASTER_ASSASSIN",
    "PM.ASHIKAGA_TAKAUJI",
    "PM.LORD_SURTUR",
    "PM.DARK_ONE",
    "PM.STUDENT",
    "PM.CHIEFTAIN",
    "PM.NEANDERTHAL",
    "PM.ATTENDANT",
    "PM.PAGE",
    "PM.ABBOT",
    "PM.ACOLYTE",
    "PM.HUNTER",
    "PM.THUG",
    "PM.NINJA",
    "PM.ROSHI",
    "PM.GUIDE",
    "PM.WARRIOR",
    "PM.APPRENTICE"
};

const char* MC_CLASSES(int i) {
  switch (i) {
    case 1: return "MC.ANT";
    case 2: return "MC.BLOB";
    case 3: return "MC.COCKATRICE";
    case 4: return "MC.DOG";
    case 5: return "MC.EYE";
    case 6: return "MC.FELINE";
    case 7: return "MC.GREMLIN";
    case 8: return "MC.HUMANOID";
    case 9: return "MC.IMP";
    case 10: return "MC.JELLY";
    case 11: return "MC.KOBOLD";
    case 12: return "MC.LEPRECHAUN";
    case 13: return "MC.MIMIC";
    case 14: return "MC.NYMPH";
    case 15: return "MC.ORC";
    case 16: return "MC.PIERCER";
    case 17: return "MC.QUADRUPED";
    case 18: return "MC.RODENT";
    case 19: return "MC.SPIDER";
    case 20: return "MC.TRAPPER";
    case 21: return "MC.UNICORN";
    case 22: return "MC.VORTEX";
    case 23: return "MC.WORM";
    case 24: return "MC.XAN";
    case 25: return "MC.LIGHT";
    case 26: return "MC.ZRUTY";
    case 27: return "MC.ANGEL";
    case 28: return "MC.BAT";
    case 29: return "MC.CENTAUR";
    case 30: return "MC.DRAGON";
    case 31: return "MC.ELEMENTAL";
    case 32: return "MC.FUNGUS";
    case 33: return "MC.GNOME";
    case 34: return "MC.GIANT";
    case 35: return "MC.invisible";
    case 36: return "MC.JABBERWOCK";
    case 37: return "MC.KOP";
    case 38: return "MC.LICH";
    case 39: return "MC.MUMMY";
    case 40: return "MC.NAGA";
    case 41: return "MC.OGRE";
    case 42: return "MC.PUDDING";
    case 43: return "MC.QUANTMECH";
    case 44: return "MC.RUSTMONST";
    case 45: return "MC.SNAKE";
    case 46: return "MC.TROLL";
    case 47: return "MC.UMBER";
    case 48: return "MC.VAMPIRE";
    case 49: return "MC.WRAITH";
    case 50: return "MC.XORN";
    case 51: return "MC.YETI";
    case 52: return "MC.ZOMBIE";
    case 53: return "MC.HUMAN";
    case 54: return "MC.GHOST";
    case 55: return "MC.GOLEM";
    case 56: return "MC.DEMON";
    case 57: return "MC.EEL";
    case 58: return "MC.LIZARD";
    case 59: return "MC.WORM_TAIL";
    case 60: return "MC.MIMIC_DEF";
    default: return "UNKNOWN";
  }
};

const char* AT_NAME(int i) {
  switch (i) {
	case 0:   return "AT.NONE";
	case 1:   return "AT.CLAW";
	case 2:   return "AT.BITE";
        case 3:   return "AT.KICK";
        case 4:   return "AT.BUTT";
        case 5:   return "AT.TUCH";
        case 6:   return "AT.STNG";
        case 7:   return "AT.HUGS";
	case 10:  return "AT.SPIT";
	case 11:  return "AT.ENGL";
	case 12:  return "AT.BREA";
	case 13:  return "AT.EXPL";
	case 14:  return "AT.BOOM";
	case 15:  return "AT.GAZE";
	case 16:  return "AT.TENT";

	case 254: return "AT.WEAP";
	case 255: return "AT.MAGC";
	default:  return "UNKNOWN";
  }
}

const char* AD_NAME(int i) {
  switch (i) {
    case 0: return "AD.PHYS";
    case 1: return "AD.MAGM";
    case 2: return "AD.FIRE";
    case 3: return "AD.COLD";
    case 4: return "AD.SLEE";
    case 5: return "AD.DISN";
    case 6: return "AD.ELEC";
    case 7: return "AD.DRST";
    case 8: return "AD.ACID";
    case 9: return "AD.SPC1";
    case 10: return "AD.SPC2";
    case 11: return "AD.BLND";
    case 12: return "AD.STUN";
    case 13: return "AD.SLOW";
    case 14: return "AD.PLYS";
    case 15: return "AD.DRLI";
    case 16: return "AD.DREN";
    case 17: return "AD.LEGS";
    case 18: return "AD.STON";
    case 19: return "AD.STCK";
    case 20: return "AD.SGLD";
    case 21: return "AD.SITM";
    case 22: return "AD.SEDU";
    case 23: return "AD.TLPT";
    case 24: return "AD.RUST";
    case 25: return "AD.CONF";
    case 26: return "AD.DGST";
    case 27: return "AD.HEAL";
    case 28: return "AD.WRAP";
    case 29: return "AD.WERE";
    case 30: return "AD.DRDX";
    case 31: return "AD.DRCO";
    case 32: return "AD.DRIN";
    case 33: return "AD.DISE";
    case 34: return "AD.DCAY";
    case 35: return "AD.SSEX";
    case 36: return "AD.HALU";
    case 37: return "AD.DETH";
    case 38: return "AD.PEST";
    case 39: return "AD.FAMN";
    case 40: return "AD.SLIM";
    case 41: return "AD.ENCH";
    case 42: return "AD.CORR";
    case 240: return "AD.CLRC";
    case 241: return "AD.SPEL";
    case 242: return "AD.RBRE";
    case 252: return "AD.SAMU";
    case 253: return "AD.CURS";
    default: return "UNKNOWN";
  }
}

const char* MS_SOUNDS(int i) {

  switch (i) {
    case 0:	return "MS.SILENT";
    case 1:	return "MS.BARK";
    case 2:	return "MS.MEW";
    case 3:	return "MS.ROAR";
    case 4:	return "MS.GROWL";
    case 5:	return "MS.SQEEK";
    case 6:	return "MS.SQAWK";
    case 7:	return "MS.HISS";
    case 8:	return "MS.BUZZ";
    case 9:	return "MS.GRUNT";
    case 10:	return "MS.NEIGH";
    case 11:	return "MS.WAIL";
    case 12:	return "MS.GURGLE";
    case 13:	return "MS.BURBLE";
    case 15:	return "MS.SHRIEK";
    case 16:	return "MS.BONES";
    case 17:	return "MS.LAUGH";
    case 18:	return "MS.MUMBLE";
    case 19:	return "MS.IMITATE";
    case 20:	return "MS.HUMANOID";
    case 21:	return "MS.ARREST";
    case 22:	return "MS.SOLDIER";
    case 23:	return "MS.GUARD";
    case 24:	return "MS.DJINNI";
    case 25:	return "MS.NURSE";
    case 26:	return "MS.SEDUCE";
    case 27:	return "MS.VAMPIRE";
    case 28:	return "MS.BRIBE";
    case 29:	return "MS.CUSS";
    case 30:	return "MS.RIDER";
    case 31:	return "MS.LEADER";
    case 32:	return "MS.NEMESIS";
    case 33:	return "MS.GUARDIAN";
    case 34:	return "MS.SELL";
    case 35:	return "MS.ORACLE";
    case 36:	return "MS.PRIEST";
    case 37:	return "MS.SPELL";
    case 38:	return "MS.WERE";
    case 39:	return "MS.BOAST";
    default: return "UNKNOWN";
  }
}

const char* MZ_SIZES(int i, int mlet) {

  if (2 == i) {
     switch (mlet) {
        case S_DOG:
        case S_IMP:
        case S_JELLY:
        case S_LIZARD:
        case S_PIERCER:
        case S_PUDDING:
        case S_RUSTMONST:
        case S_SNAKE:
        case S_UNICORN:
        case S_XORN: return "MZ.MEDIUM";
        default: break;
     }
  }

  switch(i) {
    case 0: return "MZ.TINY";
    case 1: return "MZ.SMALL";
    case 2: return "MZ.HUMAN";
    case 3: return "MZ.LARGE";
    case 4: return "MZ.HUGE";
    case 7: return "MZ.GIGANTIC";
    default: return "UNKNOWN";
  }
}

void resistances(int resists) {
	   char* sep = "";

	   if (resists & MR_FIRE) { 
             fprintf(stdout, "%sMR.FIRE", sep); sep=", ";
	   }
	   if (resists & MR_COLD) { 
             fprintf(stdout, "%sMR.COLD", sep); sep=", ";
	   }
	   if (resists & MR_SLEEP) { 
             fprintf(stdout, "%sMR.SLEEP", sep); sep=", ";
	   }
	   if (resists & MR_DISINT) { 
             fprintf(stdout, "%sMR.DISINT", sep); sep=", ";
	   }
	   if (resists & MR_ELEC) { 
             fprintf(stdout, "%sMR.ELEC", sep); sep=", ";
	   }
	   if (resists & MR_POISON) { 
             fprintf(stdout, "%sMR.POISON", sep); sep=", ";
	   }
	   if (resists & MR_ACID) { 
             fprintf(stdout, "%sMR.ACID", sep); sep=", ";
	   }
	   if (resists & MR_STONE) { 
             fprintf(stdout, "%sMR.STONE", sep); sep=", ";
	   }
}

void mflags1(long flags1) {
    char* sep = "";

    if (0 == flags1) {
        return;
    }

    fprintf(stdout, "\n        .flags1(");

    if (flags1 & M1_ACID) {
        fprintf(stdout, "%sM1.ACID", sep); sep=", ";
    }

    if (flags1 & M1_AMORPHOUS) {
        fprintf(stdout, "%sM1.AMORPHOUS", sep); sep=", ";
    }

    if (flags1 & M1_AMPHIBIOUS) {
        fprintf(stdout, "%sM1.AMPHIBIOUS", sep); sep=", ";
    }

    if (flags1 & M1_ANIMAL) {
        fprintf(stdout, "%sM1.ANIMAL", sep); sep=", ";
    }

    if (flags1 & M1_BREATHLESS) {
        fprintf(stdout, "%sM1.BREATHLESS", sep); sep=", ";
    }

    if (flags1 & M1_CARNIVORE) {
        fprintf(stdout, "%sM1.CARNIVORE", sep); sep=", ";
    }

    if (flags1 & M1_CLING) {
        fprintf(stdout, "%sM1.CLING", sep); sep=", ";
    }

    if (flags1 & M1_CONCEAL) {
        fprintf(stdout, "%sM1.CONCEAL", sep); sep=", ";
    }

    if (flags1 & M1_FLY) {
        fprintf(stdout, "%sM1.FLY", sep); sep=", ";
    }

    if (flags1 & M1_HERBIVORE) {
        fprintf(stdout, "%sM1.HERBIVORE", sep); sep=", ";
    }

    if (flags1 & M1_HIDE) {
        fprintf(stdout, "%sM1.HIDE", sep); sep=", ";
    }

    if (flags1 & M1_HUMANOID) {
        fprintf(stdout, "%sM1.HUMANOID", sep); sep=", ";
    }

    if (flags1 & M1_METALLIVORE) {
        fprintf(stdout, "%sM1.METALLIVORE", sep); sep=", ";
    }

    if (flags1 & M1_MINDLESS) {
        fprintf(stdout, "%sM1.MINDLESS", sep); sep=", ";
    }

    if (flags1 & M1_NEEDPICK) {
        fprintf(stdout, "%sM1.NEEDPICK", sep); sep=", ";
    }

    if (flags1 & M1_NOEYES) {
        fprintf(stdout, "%sM1.NOEYES", sep); sep=", ";
    }

    if (flags1 & M1_NOHANDS) {
        fprintf(stdout, "%sM1.NOHANDS", sep); sep=", ";
    }

    if (flags1 & M1_NOHEAD) {
        fprintf(stdout, "%sM1.NOHEAD", sep); sep=", ";
    }

    if (flags1 & M1_NOLIMBS) {
        fprintf(stdout, "%sM1.NOLIMBS", sep); sep=", ";
    }

    if (flags1 & M1_NOTAKE) {
        fprintf(stdout, "%sM1.NOTAKE", sep); sep=", ";
    }

    if (flags1 & M1_OMNIVORE) {
        fprintf(stdout, "%sM1.OMNIVORE", sep); sep=", ";
    }

    if (flags1 & M1_OVIPAROUS) {
        fprintf(stdout, "%sM1.OVIPAROUS", sep); sep=", ";
    }

    if (flags1 & M1_POIS) {
        fprintf(stdout, "%sM1.POIS", sep); sep=", ";
    }

    if (flags1 & M1_REGEN) {
        fprintf(stdout, "%sM1.REGEN", sep); sep=", ";
    }

    if (flags1 & M1_SEE_INVIS) {
        fprintf(stdout, "%sM1.SEE_INVIS", sep); sep=", ";
    }

    if (flags1 & M1_SLITHY) {
        fprintf(stdout, "%sM1.SLITHY", sep); sep=", ";
    }

    if (flags1 & M1_SWIM) {
        fprintf(stdout, "%sM1.SWIM", sep); sep=", ";
    }

    if (flags1 & M1_THICK_HIDE) {
        fprintf(stdout, "%sM1.THICK_HIDE", sep); sep=", ";
    }

    if (flags1 & M1_TPORT) {
        fprintf(stdout, "%sM1.TPORT", sep); sep=", ";
    }

    if (flags1 & M1_TPORT_CNTRL) {
        fprintf(stdout, "%sM1.TPORT_CNTRL", sep); sep=", ";
    }

    if (flags1 & M1_TUNNEL) {
        fprintf(stdout, "%sM1.TUNNEL", sep); sep=", ";
    }

    if (flags1 & M1_UNSOLID) {
        fprintf(stdout, "%sM1.UNSOLID", sep); sep=", ";
    }

    if (flags1 & M1_WALLWALK) {
        fprintf(stdout, "%sM1.WALLWALK", sep); sep=", ";
    }
    fprintf(stdout, ")");

}

void mflags2(long flags2) {
    char* sep = "";

    if (0 == flags2) {
        return;
    }

    fprintf(stdout, "\n        .flags2(");

    if (flags2 & M2_COLLECT) {
        fprintf(stdout, "%sM2.COLLECT", sep); sep=", ";
    }

    if (flags2 & M2_DEMON) {
        fprintf(stdout, "%sM2.DEMON", sep); sep=", ";
    }

    if (flags2 & M2_DOMESTIC) {
        fprintf(stdout, "%sM2.DOMESTIC", sep); sep=", ";
    }

    if (flags2 & M2_DWARF) {
        fprintf(stdout, "%sM2.DWARF", sep); sep=", ";
    }

    if (flags2 & M2_ELF) {
        fprintf(stdout, "%sM2.ELF", sep); sep=", ";
    }

    if (flags2 & M2_FEMALE) {
        fprintf(stdout, "%sM2.FEMALE", sep); sep=", ";
    }

    if (flags2 & M2_GIANT) {
        fprintf(stdout, "%sM2.GIANT", sep); sep=", ";
    }

    if (flags2 & M2_GNOME) {
        fprintf(stdout, "%sM2.GNOME", sep); sep=", ";
    }

    if (flags2 & M2_GREEDY) {
        fprintf(stdout, "%sM2.GREEDY", sep); sep=", ";
    }

    if (flags2 & M2_HOSTILE) {
        fprintf(stdout, "%sM2.HOSTILE", sep); sep=", ";
    }

    if (flags2 & M2_HUMAN) {
        fprintf(stdout, "%sM2.HUMAN", sep); sep=", ";
    }

    if (flags2 & M2_JEWELS) {
        fprintf(stdout, "%sM2.JEWELS", sep); sep=", ";
    }

    if (flags2 & M2_LORD) {
        fprintf(stdout, "%sM2.LORD", sep); sep=", ";
    }

    if (flags2 & M2_MAGIC) {
        fprintf(stdout, "%sM2.MAGIC", sep); sep=", ";
    }

    if (flags2 & M2_MALE) {
        fprintf(stdout, "%sM2.MALE", sep); sep=", ";
    }

    if (flags2 & M2_MERC) {
        fprintf(stdout, "%sM2.MERC", sep); sep=", ";
    }

    if (flags2 & M2_MINION) {
        fprintf(stdout, "%sM2.MINION", sep); sep=", ";
    }

    if (flags2 & M2_NASTY) {
        fprintf(stdout, "%sM2.NASTY", sep); sep=", ";
    }

    if (flags2 & M2_NEUTER) {
        fprintf(stdout, "%sM2.NEUTER", sep); sep=", ";
    }

    if (flags2 & M2_NOPOLY) {
        fprintf(stdout, "%sM2.NOPOLY", sep); sep=", ";
    }

    if (flags2 & M2_ORC) {
        fprintf(stdout, "%sM2.ORC", sep); sep=", ";
    }

    if (flags2 & M2_PEACEFUL) {
        fprintf(stdout, "%sM2.PEACEFUL", sep); sep=", ";
    }

    if (flags2 & M2_PNAME) {
        fprintf(stdout, "%sM2.PNAME", sep); sep=", ";
    }

    if (flags2 & M2_PRINCE) {
        fprintf(stdout, "%sM2.PRINCE", sep); sep=", ";
    }

    if (flags2 & M2_ROCKTHROW) {
        fprintf(stdout, "%sM2.ROCKTHROW", sep); sep=", ";
    }

    if (flags2 & M2_SHAPESHIFTER) {
        fprintf(stdout, "%sM2.SHAPESHIFTER", sep); sep=", ";
    }

    if (flags2 & M2_STALK) {
        fprintf(stdout, "%sM2.STALK", sep); sep=", ";
    }

    if (flags2 & M2_STRONG) {
        fprintf(stdout, "%sM2.STRONG", sep); sep=", ";
    }

    if (flags2 & M2_UNDEAD) {
        fprintf(stdout, "%sM2.UNDEAD", sep); sep=", ";
    }

    if (flags2 & M2_WANDER) {
        fprintf(stdout, "%sM2.WANDER", sep); sep=", ";
    }

    if (flags2 & M2_WERE) {
        fprintf(stdout, "%sM2.WERE", sep); sep=", ";
    }

    fprintf(stdout, ")");
}


void mflags3(long flags3) {
    char* sep = "";

    if (0 == flags3) {
        return;
    }

    fprintf(stdout, "\n        .flags3(");

    if (flags3 & M3_WANTSAMUL) {
        fprintf(stdout, "%sM3.WANTSAMUL", sep); sep=", ";
    }

    if (flags3 & M3_WANTSBELL) {
        fprintf(stdout, "%sM3.WANTSBELL", sep); sep=", ";
    }

    if (flags3 & M3_WANTSBOOK) {
        fprintf(stdout, "%sM3.WANTSBOOK", sep); sep=", ";
    }

    if (flags3 & M3_WANTSCAND) {
        fprintf(stdout, "%sM3.WANTSCAND", sep); sep=", ";
    }

    if (flags3 & M3_WANTSARTI) {
        fprintf(stdout, "%sM3.WANTSARTI", sep); sep=", ";
    }

    if (flags3 & M3_WANTSALL) {
        fprintf(stdout, "%sM3.WANTSALL", sep); sep=", ";
    }

    if (flags3 & M3_WAITFORU) {
        fprintf(stdout, "%sM3.WAITFORU", sep); sep=", ";
    }

    if (flags3 & M3_CLOSE) {
        fprintf(stdout, "%sM3.CLOSE", sep); sep=", ";
    }

    if (flags3 & M3_COVETOUS) {
        fprintf(stdout, "%sM3.COVETOUS", sep); sep=", ";
    }

    if (flags3 & M3_WAITMASK) {
        fprintf(stdout, "%sM3.WAITMASK", sep); sep=", ";
    }

    if (flags3 & M3_INFRAVISION) {
        fprintf(stdout, "%sM3.INFRAVISION", sep); sep=", ";
    }

    if (flags3 & M3_INFRAVISIBLE) {
        fprintf(stdout, "%sM3.INFRAVISIBLE", sep); sep=", ";
    }

    if (flags3 & M3_DISPLACES) {
        fprintf(stdout, "%sM3.DISPLACES", sep); sep=", ";
    }

    fprintf(stdout, ")");
}


const char* CLR_NAMES(int pm, int i) {

  switch (pm) {
    case PM_IRON_GOLEM: return "CLR.METAL";
    case PM_KI_RIN:
    case PM_GOLDEN_NAGA_HATCHLING:
    case PM_GOLDEN_NAGA:
    case PM_GOLD_GOLEM: return "CLR.GOLD";
    case PM_LEATHER_GOLEM: return "CLR.LEATHER";
    case PM_WOOD_GOLEM: return "CLR.WOOD";
    case PM_PAPER_GOLEM: return "CLR.PAPER";
    case PM_BABY_SILVER_DRAGON:
    case PM_SILVER_DRAGON: return "CLR.DRAGON_SILVER";
			  
    case PM_SHOCKING_SPHERE:
    case PM_KOBOLD_SHAMAN:
    case PM_ORC_SHAMAN:
    case PM_ENERGY_VORTEX:
    case PM_GNOMISH_WIZARD:
    case PM_ORACLE: return "CLR.ZAP";

    default: break;

  }

  switch(i) {
    case 0: return "CLR.BLACK";
    case 1: return "CLR.RED";
    case 2: return "CLR.GREEN";
    case 3: return "CLR.BROWN";
    case 4: return "CLR.BLUE";
    case 5: return "CLR.MAGENTA";
    case 6: return "CLR.CYAN";
    case 7: return "CLR.GRAY";
    case 9: return "CLR.ORANGE";
    case 10: return "CLR.BRIGHT_GREEN";
    case 11: return "CLR.YELLOW";
    case 12: return "CLR.BRIGHT_BLUE";
    case 13: return "CLR.BRIGHT_MAGENTA";
    case 14: return "CLR.BRIGHT_CYAN";
    case 15: return "CLR.WHITE";
    default: return "UNKNOWN";
  }

}


int main(int argc, char* argv[])
{
   int i = 0;
   for (i = 0; mons[i].mname; i++) {
       fprintf(stdout, "    MonsterType.of(%s, %s, \"%s\")",
            MC_CLASSES(mons[i].mlet),
	    PM_TYPES[mons[i].monsterTypeID],
	    mons[i].mname);

       fprintf(stdout, "\n        .level(%d)", monsterLevel(mons[i].monsterTypeID));
       fprintf(stdout, ".move(%d)", mons[i].mmove);
       fprintf(stdout, ".ac(%d).mr(%d)", mons[i].ac, mons[i].mr);

       int align = mons[i].maligntyp;
       if (0 == align) {
           fprintf(stdout, ".neutral()");
       } else if (align > 0) {
           fprintf(stdout, ".lawful(%d)", align);
       } else {
           fprintf(stdout, ".chaotic(%d)", -align);
       }

       int gen_info = mons[i].geno;
       fprintf(stdout, "\n        .freq(%d)", gen_info & G_FREQ);
       if (gen_info & G_UNIQ) {
           fprintf(stdout, ".unique()");
       }
       if (gen_info & G_NOGEN) {
           fprintf(stdout, ".special()");
       }
       if (gen_info & G_HELL) {
           fprintf(stdout, ".onlyInHell()");
       }
       if (gen_info & G_NOHELL) {
           fprintf(stdout, ".notInHell()");
       }
       if (gen_info & G_SGROUP) {
           fprintf(stdout, ".smallGroups()");
       }
       if (gen_info & G_LGROUP) {
           fprintf(stdout, ".largeGroups()");
       }
       if (gen_info & G_GENO) {
           fprintf(stdout, ".genocidable()");
       }
       if (gen_info & G_NOCORPSE) {
           fprintf(stdout, ".noCorpse()");
       }

       int aa;
       for (aa = 0; aa < NATTK; aa++) {
           if (mons[i].mattk[aa].aatyp != 0) {
               fprintf(stdout, "\n        .attack(%s, %s, %d, Dice.D%d)",
                   AT_NAME(mons[i].mattk[aa].aatyp),
                   AD_NAME(mons[i].mattk[aa].adtyp),
                   mons[i].mattk[aa].damn, mons[i].mattk[aa].damd);
	   }
       }

       fprintf(stdout, "\n        .weight(%d).nutrition(%d)", mons[i].cwt, mons[i].cnutrit);
       fprintf(stdout, "\n        .sound(%s).size(%s)", MS_SOUNDS(mons[i].msound),
		       MZ_SIZES(mons[i].msize, mons[i].mlet));
       
       int resists = mons[i].mresists;
       if (resists != 0) {
           fprintf(stdout, "\n        .resists(");
	   resistances(resists);
           fprintf(stdout, ")");
       }

       int conveys = mons[i].mconveys;
       if (conveys != 0) {
           fprintf(stdout, "\n        .conveys(");
	   resistances(conveys);
           fprintf(stdout, ")");
       }

       mflags1(mons[i].mflags1);
       mflags2(mons[i].mflags2);
       mflags3(mons[i].mflags3);

       fprintf(stdout, "\n        .color(%s).add();\n\n", CLR_NAMES(mons[i].monsterTypeID, mons[i].mcolor));
   }
}

/*monst.c*/

   
