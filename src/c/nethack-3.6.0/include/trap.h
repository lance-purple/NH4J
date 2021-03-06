/* NetHack 3.6	trap.h	$NHDT-Date: 1432512776 2015/05/25 00:12:56 $  $NHDT-Branch: master $:$NHDT-Revision: 1.12 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* note for 3.1.0 and later: no longer manipulated by 'makedefs' */

#ifndef TRAP_H
#define TRAP_H

#define TT_BEARTRAP 0
#define TT_PIT 1
#define TT_WEB 2
#define TT_LAVA 3
#define TT_INFLOOR 4
#define TT_BURIEDBALL 5

union vlaunchinfo {
    short v_launch_otyp; /* type of object to be triggered */
    coord v_launch2;     /* secondary launch point (for boulders) */
    uchar v_conjoined;   /* conjoined pit locations */
    short v_tnote;       /* boards: 12 notes        */
};

struct trap {
    struct trap *ntrap;
    xchar tx, ty;
    d_level dst; /* destination for portals */
    coord launch;
    Bitfield(ttyp, 5);
    Bitfield(tseen, 1);
    Bitfield(once, 1);
    Bitfield(madeby_u, 1); /* So monsters may take offence when you trap
                              them.	Recognizing who made the trap isn't
                              completely unreasonable, everybody has
                              their own style.  This flag is also needed
                              when you untrap a monster.  It would be too
                              easy to make a monster peaceful if you could
                              set a trap for it and then untrap it. */
    union vlaunchinfo vl;
#define launch_otyp vl.v_launch_otyp
#define launch2 vl.v_launch2
#define conjoined vl.v_conjoined
#define tnote vl.v_tnote
};

extern struct trap *ftrap;
#define newtrap() (struct trap *) alloc(sizeof(struct trap))
#define dealloc_trap(trap) free((genericptr_t)(trap))

/* reasons for statue animation */
#define ANIMATE_NORMAL 0
#define ANIMATE_SHATTER 1
#define ANIMATE_SPELL 2

/* reasons for animate_statue's failure */
#define AS_OK 0            /* didn't fail */
#define AS_NO_MON 1        /* makeMonsterOfType failed */
#define AS_MON_IS_UNIQUE 2 /* statue monster is unique */

/* Note: if adding/removing a trap, adjust trap_engravings[] in mklev.c */

/* unconditional traps */
#define NO_TRAP 0
#define ARROW_TRAP 1
#define DART_TRAP 2
#define ROCKTRAP 3
#define SQKY_BOARD 4
#define BEAR_TRAP 5
#define LANDMINE 6
#define ROLLING_BOULDER_TRAP 7
#define SLP_GAS_TRAP 8
#define RUST_TRAP 9
#define FIRE_TRAP 10
#define PIT 11
#define SPIKED_PIT 12
#define HOLE 13
#define TRAPDOOR 14
#define TELEP_TRAP 15
#define LEVEL_TELEP 16
#define MAGIC_PORTAL 17
#define WEB 18
#define STATUE_TRAP 19
#define MAGIC_TRAP 20
#define ANTI_MAGIC 21
#define POLY_TRAP 22
#define VIBRATING_SQUARE 23
#define TRAPNUM 24

#endif /* TRAP_H */
