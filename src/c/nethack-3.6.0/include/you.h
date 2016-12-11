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

struct u_have {
    Bitfield(amulet, 1);   /* carrying Amulet	*/
    Bitfield(bell, 1);     /* carrying Bell	*/
    Bitfield(book, 1);     /* carrying Book	*/
    Bitfield(menorah, 1);  /* carrying Candelabrum */
    Bitfield(questart, 1); /* carrying the Quest Artifact */
    Bitfield(unused, 3);
};

struct u_event {
    Bitfield(minor_oracle, 1); /* received at least 1 cheap oracle */
    Bitfield(major_oracle, 1); /*  "  expensive oracle */
    Bitfield(read_tribute, 1); /* read a passage from a novel */
    Bitfield(qcalled, 1);      /* called by Quest leader to do task */
    Bitfield(qexpelled, 1);    /* expelled from the Quest dungeon */
    Bitfield(qcompleted, 1);   /* successfully completed Quest task */
    Bitfield(uheard_tune, 2);  /* 1=know about, 2=heard passtune */

    Bitfield(uopened_dbridge, 1);   /* opened the drawbridge */
    Bitfield(invoked, 1);           /* invoked Gate to the Sanctum level */
    Bitfield(gehennom_entered, 1);  /* entered Gehennom via Valley */
    Bitfield(uhand_of_elbereth, 2); /* became Hand of Elbereth */
    Bitfield(udemigod, 1);          /* killed the wiz */
    Bitfield(uvibrated, 1);         /* stepped on "vibrating square" */
    Bitfield(ascended, 1);          /* has offered the Amulet */
};

struct u_achieve {
    Bitfield(amulet, 1);  /* touched Amulet */
    Bitfield(bell, 1);    /* touched Bell */
    Bitfield(book, 1);    /* touched Book */
    Bitfield(menorah, 1); /* touched Candelabrum */
    Bitfield(enter_gehennom,
             1);           /* entered Gehennom (or Valley) by any means */
    Bitfield(ascended, 1); /* not quite the same as u.uevent.ascended */
    Bitfield(mines_luckstone, 1); /* got a luckstone at end of mines */
    Bitfield(finish_sokoban, 1);  /* obtained the sokoban prize */

    Bitfield(killed_medusa, 1);
};

struct u_realtime {
    long
        realtime; /* actual playing time up until the last restore, seconds */
    time_t restored; /* time the game was started or restored */
    time_t endtime;
};

/* KMH, conduct --
 * These are voluntary challenges.  Each field denotes the number of
 * times a challenge has been violated.
 */
struct u_conduct {     /* number of times... */
    long unvegetarian; /* eaten any animal */
    long unvegan;      /* ... or any animal byproduct */
    long food;         /* ... or any comestible */
    long gnostic;      /* used prayer, priest, or altar */
    long weaphit;      /* hit a monster with a weapon */
    long killer;       /* killed a monster yourself */
    long literate;     /* read something (other than BotD) */
    long polypiles;    /* polymorphed an object */
    long polyselfs;    /* transformed yourself */
    long wishes;       /* used a wish */
    long wisharti;     /* wished for an artifact */
    /* genocides already listed at end of game */
};

struct u_roleplay {
    boolean blind;  /* permanently blind */
    boolean nudist; /* has not worn any armor, ever */
    long numbones;  /* # of bones files loaded  */
};

/*** Unified structure containing role information ***/
struct Role {
    /*** Strings that name various things ***/
    struct RoleName name;    /* the role's name (from u_init.c) */
    struct RoleName rank[9]; /* names for experience levels (from botl.c) */
    const char *lgod, *ngod, *cgod; /* god names (from pray.c) */
    const char *filecode;           /* abbreviation for use in file names */
    const char *homebase; /* quest leader's location (from questpgr.c) */
    const char *intermed; /* quest intermediate goal (from questpgr.c) */

    /*** Indices of important monsters and objects ***/
    short malenum, /* index (PM_) as a male (botl.c) */
        femalenum, /* ...or as a female (NON_PM == same) */
        petnum,    /* PM_ of preferred pet (NON_PM == random) */
        ldrnum,    /* PM_ of quest leader (questpgr.c) */
        guardnum,  /* PM_ of quest guardians (questpgr.c) */
        neminum,   /* PM_ of quest nemesis (questpgr.c) */
        enemy1num, /* specific quest enemies (NON_PM == random) */
        enemy2num;
    char enemy1sym, /* quest enemies by class (S_) */
        enemy2sym;
    short questarti; /* index (ART_) of quest artifact (questpgr.c) */

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
    xchar attrbase[A_MAX];    /* lowest initial attributes */
    xchar attrdist[A_MAX];    /* distribution of initial attributes */
    struct RoleAdvance hpadv; /* hit point advancement */
    struct RoleAdvance enadv; /* energy advancement */
    xchar xlev;               /* cutoff experience level */
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
#define Role_if(X) (urole.malenum == (X))
#define Role_switch (urole.malenum)

/* used during initialization for race, gender, and alignment
   as well as for character class */
#define ROLE_NONE (-1)
#define ROLE_RANDOM (-2)

/*** Unified structure specifying race information ***/

struct Race {
    /*** Strings that name various things ***/
    const char *noun;           /* noun ("human", "elf") */
    const char *adj;            /* adjective ("human", "elven") */
    const char *coll;           /* collective ("humanity", "elvenkind") */
    const char *filecode;       /* code for filenames */
    struct RoleName individual; /* individual as a noun ("man", "elf") */

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

/*** Unified structure specifying gender information ***/
struct Gender {
    const char *adj;      /* male/female/neuter */
    const char *he;       /* he/she/it */
    const char *him;      /* him/her/it */
    const char *his;      /* his/her/its */
    const char *filecode; /* file code */
    short allow;          /* equivalent ROLE_ mask */
};
#define ROLE_GENDERS 2 /* number of permitted player genders */
/* increment to 3 if you allow neuter roles */

extern const struct Gender genders[]; /* table of available genders */
#define uhe() (genders[flags.female ? 1 : 0].he)
#define uhim() (genders[flags.female ? 1 : 0].him)
#define uhis() (genders[flags.female ? 1 : 0].his)
#define mhe(mtmp) (genders[pronoun_gender(mtmp)].he)
#define mhim(mtmp) (genders[pronoun_gender(mtmp)].him)
#define mhis(mtmp) (genders[pronoun_gender(mtmp)].his)

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
    char urooms[5];         /* rooms (roomno + 3) occupied now */
    char urooms0[5];        /* ditto, for previous position */
    char uentered[5];       /* rooms (roomno + 3) entered this turn */
    char ushops[5];         /* shop rooms (roomno + 3) occupied now */
    char ushops0[5];        /* ditto, for previous position */
    char ushops_entered[5]; /* ditto, shops entered this turn */
    char ushops_left[5];    /* ditto, shops exited this turn */

    struct prop uprops[LAST_PROP + 1];

    Bitfield(uswallow, 1);      /* true if swallowed */
    Bitfield(uinwater, 1);      /* if you're currently in water (only
                                   underwater possible currently) */
    Bitfield(uundetected, 1);   /* if you're a hiding monster/piercer */
    Bitfield(mfemale, 1);       /* saved human value of flags.female */
    Bitfield(uinvulnerable, 1); /* you're invulnerable (praying) */
    Bitfield(uburied, 1);       /* you're buried */
    Bitfield(uedibility, 1); /* blessed food detection; sense unsafe food */
    /* 1 free bit! */

    struct u_achieve uachieve; /* achievements */
    struct u_event uevent;     /* certain events have happened */
    struct u_have uhave;       /* you're carrying special objects */
    struct u_conduct uconduct; /* KMH, conduct */
    struct u_roleplay uroleplay;

    align ualign;         /* character alignment */
#define CONVERT 2
#define A_ORIGINAL 1
#define A_CURRENT 0
    aligntyp ualignbase[CONVERT]; /* for ualign conversion record */

#define Luck (currentLuck() + luckBonus())
#define LUCKADD 3 /* added value when carrying luck stone */
#define LUCKMAX 10
#define LUCKMIN (-10)

    long ucleansed; /* to record moves when player was cleansed */
    long usleep;    /* sleeping; monstermove you last started */
    int uinvault;

    struct monst *ustuck;
    struct monst *usteed;
    long ugallop;
    int urideturns;
    int umortality;      /* how many times you died */
    int ugrave_arise;    /* you die and become something aside from a ghost */
    int weapon_slots;    /* unused skill slots */
    int skills_advanced; /* # of advances made so far */
    xchar skill_record[P_SKILL_LIMIT]; /* skill advancements */
    struct skills weapon_skills[P_NUM_SKILLS];
    boolean twoweap; /* KMH -- Using two-weapon combat */

}; /* end of `struct you' */

#endif /* YOU_H */
