/* NetHack 3.6	permonst.h	$NHDT-Date: 1432512778 2015/05/25 00:12:58 $  $NHDT-Branch: master $:$NHDT-Revision: 1.9 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PERMONST_H
#define PERMONST_H

/*	This structure covers all attack forms.
 *	aatyp is the gross attack type (eg. claw, bite, breath, ...)
 *	adtyp is the damage type (eg. physical, fire, cold, spell, ...)
 *	damn is the number of hit dice of damage from the attack.
 *	damd is the number of sides on each die.
 *
 *	Some attacks can do no points of damage.  Additionally, some can
 *	have special effects *and* do damage as well.  If damn and damd
 *	are set, they may have a special meaning.  For example, if set
 *	for a blinding attack, they determine the amount of time blinded.
 */

struct attack {
    int type;
    int damageType;
    int dice;
    int diceSides;
};

struct Attack {
    int type;
    int damageType;
    int dice;
    int diceSides;
};

extern struct Attack NO_ATTACK;

extern boolean validAttack(const struct Attack); /* not NO_ATTACK */


/*	Max # of attacks for any given monster.
 */

#define NATTK 6

/*	Weight of a human body
 */

#define WT_HUMAN 1450

#ifndef ALIGN_H
#include "align.h"
#endif
#include "monattk.h"
#include "monflag.h"

struct permonst {
	/* can't eliminate these until mons[] gone, so rename to XX */
    const int monsterTypeID;    /* type ID */
    const char *XXmname;        /* full name */
    char XXmlet;                /* symbol */
    schar XXmlevel;             /* base monster level */
    schar XXmmove;              /* move speed */
    schar XXac;                 /* (base) armor class */
    schar XXmr;                 /* (base) magic resistance */
    aligntyp XXmaligntyp;       /* basic monster alignment */
    unsigned short XXgeno;      /* creation/geno mask value */

    struct attack XXmattk[NATTK]; /* attacks matrix */

    unsigned short XXcwt;       /* weight of corpse */
    unsigned short XXcnutrit;   /* its nutritional value */
    uchar XXmsound;             /* noise it makes (6 bits) */
    uchar XXmsize;              /* physical size (3 bits) */
    uchar XXmresists;           /* resistances */
    uchar XXmconveys;           /* conveyed by eating */

    unsigned long XXmflags1;    /* boolean bitflags */
    unsigned long XXmflags2;    /* more boolean bitflags */
    unsigned short XXmflags3;     /* yet more boolean bitflags */
#ifdef TEXTCOLOR
    uchar XXmcolor; /* color to use */
#endif
};

extern NEARDATA struct permonst mons[]; /* the master list of monster types */

#define VERY_SLOW 3
#define SLOW_SPEED 9
#define NORMAL_SPEED 12 /* movement rates */
#define FAST_SPEED 15
#define VERY_FAST 24

#define NON_PM PM_PLAYERMON          /* "not a monster" */
#define LOW_PM (NON_PM + 1)          /* first monster in mons[] */
#define SPECIAL_PM PM_LONG_WORM_TAIL /* [normal] < ~ < [special] */
/* mons[SPECIAL_PM] through mons[NUMMONS-1], inclusive, are
   never generated randomly and cannot be polymorphed into */

#endif /* PERMONST_H */
