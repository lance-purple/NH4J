/* NetHack 3.6	you.h	$NHDT-Date: 1432512782 2015/05/25 00:13:02 $  $NHDT-Branch: master $:$NHDT-Revision: 1.29 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef YOU_H
#define YOU_H

#include "attrib.h"
#include "monst.h"
#ifndef PROP_H
#include "prop.h" /* (needed here for util/makedefs.c) */
#endif
#include "skills.h"

/*** Substructures ***/

struct RoleName {
    const char *m; /* name when character is male */
    const char *f; /* when female; null if same as male */
};

struct RoleAdvance {
    /* "fix" is the fixed amount, "rnd" is the random amount */
    xchar infix, inrnd; /* at character initialization */
    xchar lofix, lornd; /* gained per level <  urole.xlev */
    xchar hifix, hirnd; /* gained per level >= urole.xlev */
};

#define ACHIEVEMENT_GOT_AMULET 0
#define ACHIEVEMENT_GOT_BELL 1
#define ACHIEVEMENT_GOT_BOOK 2
#define ACHIEVEMENT_GOT_CANDELABRUM 3
#define ACHIEVEMENT_ENTERED_GEHENNOM 4
#define ACHIEVEMENT_ASCENDED 5
#define ACHIEVEMENT_FOUND_LUCKSTONE_IN_MINES 6
#define ACHIEVEMENT_FINISHED_SOKOBAN_LEVEL 7
#define ACHIEVEMENT_KILLED_MEDUSA 8

#define SPECIAL_ITEM_AMULET 0
#define SPECIAL_ITEM_BELL 1
#define SPECIAL_ITEM_BOOK 2
#define SPECIAL_ITEM_CANDELABRUM 3
#define SPECIAL_ITEM_QUEST_ARTIFACT 4


struct u_realtime {
    long
        realtime; /* actual playing time up until the last restore, seconds */
    time_t restored; /* time the game was started or restored */
    time_t endtime;
};

/*** Unified structure containing role information ***/
struct Role {
    int id;
    /*** Strings that name various things ***/
    struct RoleName XXname;    /* the role's name (from u_init.c) */

    /*** Bitmasks ***/
    short allow;             /* bit mask of allowed variations */
#define ROLE_RACEMASK 0x0ff8 /* allowable races */
#define ROLE_GENDMASK 0xf000 /* allowable genders */
#define ROLE_MALE 0x1000
#define ROLE_FEMALE 0x2000
#define ROLE_NEUTER 0x4000
#define ROLE_ALIGNMASK AM_MASK /* allowable alignments */
#define ROLE_LAWFUL AM_LAWFUL
#define ROLE_NEUTRAL AM_NEUTRAL
#define ROLE_CHAOTIC AM_CHAOTIC

    /*** Attributes (from attrib.c and exper.c) ***/
    struct RoleAdvance enadv; /* energy advancement */
    xchar initrecord;         /* initial alignment record */

    /*** Spell statistics (from spell.c) ***/
    int spelbase; /* base spellcasting penalty */
    int spelheal; /* penalty (-bonus) for healing spells */
    int spelshld; /* penalty for wearing any shield */
    int spelarmr; /* penalty for wearing metal armour */
    int spelstat; /* which stat (A_) is used */
    int spelspec; /* spell (SPE_) the class excels at */
    int spelsbon; /* penalty (-bonus) for that spell */

    /*** Properties in variable-length arrays ***/
    /* intrinsics (see attrib.c) */
    /* initial inventory (see u_init.c) */
    /* skills (see u_init.c) */

    /*** Don't forget to add... ***/
    /* quest leader, guardians, nemesis (monst.c) */
    /* quest artifact (artilist.h) */
    /* quest dungeon definition (dat/Xyz.dat) */
    /* quest text (dat/quest.txt) */
    /* dictionary entries (dat/data.bas) */
};

extern const struct Role roles[]; /* table of available roles */
extern struct Role urole;

/* used during initialization for race, gender, and alignment
   as well as for character class */
#define ROLE_NONE (-1)
#define ROLE_RANDOM (-2)

/*** Unified structure specifying race information ***/

struct Race {
    /*** Strings that name various things ***/
    const char *XXnoun;         /* noun ("human", "elf") */
    const char *XXadj;          /* adjective ("human", "elven") */
    const char *XXcoll;           /* collective ("humanity", "elvenkind") */
    const char *XXfilecode;       /* code for filenames */
    struct RoleName XXindividual; /* individual as a noun ("man", "elf") */

    /*** Indices of important monsters and objects ***/
    short malenum, /* PM_ as a male monster */
        femalenum, /* ...or as a female (NON_PM == same) */
        mummynum,  /* PM_ as a mummy */
        zombienum; /* PM_ as a zombie */

    /*** Bitmasks ***/
    short allow;    /* bit mask of allowed variations */
    short selfmask, /* your own race's bit mask */
        lovemask,   /* bit mask of always peaceful */
        hatemask;   /* bit mask of always hostile */

    /*** Attributes ***/
    xchar attrmin[A_MAX];     /* minimum allowable attribute */
    xchar attrmax[A_MAX];     /* maximum allowable attribute */
    struct RoleAdvance hpadv; /* hit point advancement */
    struct RoleAdvance enadv; /* energy advancement */
#if 0                         /* DEFERRED */
	int   nv_range;		/* night vision range */
	int   xray_range;	/* X-ray vision range */
#endif

    /*** Properties in variable-length arrays ***/
    /* intrinsics (see attrib.c) */

    /*** Don't forget to add... ***/
    /* quest leader, guardians, nemesis (monst.c) */
    /* quest dungeon definition (dat/Xyz.dat) */
    /* quest text (dat/quest.txt) */
    /* dictionary entries (dat/data.bas) */
};

extern const struct Race races[]; /* Table of available races */
extern struct Race urace;
#define Race_if(X) (urace.malenum == (X))
#define Race_switch (urace.malenum)

/*** Unified structure specifying alignment information ***/
struct Align {
    const char *noun;     /* law/balance/chaos */
    const char *adj;      /* lawful/neutral/chaotic */
    const char *filecode; /* file code */
    short allow;          /* equivalent ROLE_ mask */
    aligntyp value;       /* equivalent A_ value */
};
#define ROLE_ALIGNS 3 /* number of permitted player alignments */

extern const struct Align aligns[]; /* table of available alignments */

#define SICK_VOMITABLE 0x01
#define SICK_NONVOMITABLE 0x02
#define SICK_ALL 0x03

/*** Information about the player ***/
struct you {

    struct monst *ustuck;
    struct monst *usteed;

}; /* end of `struct you' */

#endif /* YOU_H */
