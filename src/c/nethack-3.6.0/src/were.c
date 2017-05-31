/* NetHack 3.6	were.c	$NHDT-Date: 1432512763 2015/05/25 00:12:43 $  $NHDT-Branch: master $:$NHDT-Revision: 1.18 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

void
were_change(mon)
register struct monst *mon;
{
    if (!isWere(mon->data->monsterTypeID))
        return;

    if (isHuman(mon->data->monsterTypeID)) {
        if (!youHaveProtectionFromShapeChangers()
            && !rn2(night() ? (flags.moonphase == FULL_MOON ? 3 : 30)
                            : (flags.moonphase == FULL_MOON ? 10 : 50))) {
            new_were(mon); /* change into animal form */
            if (!youAreDeaf() && !canseemon(mon)) {
                const char *howler;

                switch (mon->data->monsterTypeID) {
                case PM_WEREWOLF:
                    howler = "wolf";
                    break;
                case PM_WEREJACKAL:
                    howler = "jackal";
                    break;
                default:
                    howler = (char *) 0;
                    break;
                }
                if (howler)
                    You_hear("a %s howling at the moon.", howler);
            }
        }
    } else if (!rn2(30) || youHaveProtectionFromShapeChangers()) {
        new_were(mon); /* change back into human form */
    }
}

int
counter_were(pm)
int pm;
{
    switch (pm) {
    case PM_WEREWOLF:
        return (PM_HUMAN_WEREWOLF);
    case PM_HUMAN_WEREWOLF:
        return (PM_WEREWOLF);
    case PM_WEREJACKAL:
        return (PM_HUMAN_WEREJACKAL);
    case PM_HUMAN_WEREJACKAL:
        return (PM_WEREJACKAL);
    case PM_WERERAT:
        return (PM_HUMAN_WERERAT);
    case PM_HUMAN_WERERAT:
        return (PM_WERERAT);
    default:
        return NON_PM;
    }
}

/* convert monsters similar to werecritters into appropriate werebeast */
int
were_beastie(pm)
int pm;
{
    switch (pm) {
    case PM_WERERAT:
    case PM_SEWER_RAT:
    case PM_GIANT_RAT:
    case PM_RABID_RAT:
        return PM_WERERAT;
    case PM_WEREJACKAL:
    case PM_JACKAL:
    case PM_FOX:
    case PM_COYOTE:
        return PM_WEREJACKAL;
    case PM_WEREWOLF:
    case PM_WOLF:
    case PM_WARG:
    case PM_WINTER_WOLF:
        return PM_WEREWOLF;
    default:
        break;
    }
    return NON_PM;
}

void
new_were(mon)
register struct monst *mon;
{
    register int pm;

    pm = counter_were(mon->data->monsterTypeID);
    if (pm < LOW_PM) {
	javaString wereName = monsterTypeName(mon->data->monsterTypeID);
        impossible("unknown lycanthrope %s.", wereName.c_str);
	releaseJavaString(wereName);
        return;
    }

    if (canseemon(mon) && !youAreHallucinating()) {
	javaString monsterName = monsterTypeName(mons[pm].monsterTypeID);
        pline("%s changes into a %s.", Monnam(mon),
              isHuman(mons[pm].monsterTypeID) ? "human" : monsterName.c_str + 4);
	releaseJavaString(monsterName);
    }

    set_mon_data(mon, &mons[pm], 0);
    if (mon->msleeping || !mon->mcanmove) {
        /* transformation wakens and/or revitalizes */
        mon->msleeping = 0;
        mon->mfrozen = 0; /* not asleep or paralyzed */
        mon->mcanmove = 1;
    }
    /* regenerate by 1/4 of the lost hit points */
    mon->mhp += (mon->mhpmax - mon->mhp) / 4;
    newsym(mon->mx, mon->my);
    mon_break_armor(mon, FALSE);
    possibly_unwield(mon, FALSE);
}

int were_summon(ptr, yours, visible,
                genbuf) /* were-creature (even you) summons a horde */
register struct permonst *ptr;
register boolean yours;
int *visible; /* number of visible helpers created */
char *genbuf;
{
    register int i, typ;
    int pm = ptr->monsterTypeID;
    register struct monst *mtmp;
    int total = 0;

    *visible = 0;
    if (youHaveProtectionFromShapeChangers() && !yours)
        return 0;
    for (i = rnd(5); i > 0; i--) {
        switch (pm) {
        case PM_WERERAT:
        case PM_HUMAN_WERERAT:
            typ =
                rn2(3) ? PM_SEWER_RAT : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT;
            if (genbuf)
                Strcpy(genbuf, "rat");
            break;
        case PM_WEREJACKAL:
        case PM_HUMAN_WEREJACKAL:
            typ = PM_JACKAL;
            if (genbuf)
                Strcpy(genbuf, "jackal");
            break;
        case PM_WEREWOLF:
        case PM_HUMAN_WEREWOLF:
            typ = rn2(5) ? PM_WOLF : PM_WINTER_WOLF;
            if (genbuf)
                Strcpy(genbuf, "wolf");
            break;
        default:
            continue;
        }
        mtmp = makemon(&mons[typ], currentX(), currentY(), NO_MM_FLAGS);
        if (mtmp) {
            total++;
            if (canseemon(mtmp))
                *visible += 1;
        }
        if (yours && mtmp)
            (void) tamedog(mtmp, (struct obj *) 0);
    }
    return total;
}

void
you_were()
{
    char qbuf[QBUFSZ];
    boolean controllable_poly = youHavePolymorphControl() && !(youAreStunned() || youAreUnaware());

    if (youAreUnchanging() || (currentMonsterNumber() == lycanthropeType()))
        return;
    if (controllable_poly) {
        /* `+4' => skip "were" prefix to get name of beast */
	javaString wereName = monsterTypeName(mons[lycanthropeType()].monsterTypeID);
        Sprintf(qbuf, "Do you want to change into %s?",
                an(wereName.c_str + 4));
	releaseJavaString(wereName);
        if (yn(qbuf) == 'n')
            return;
    }
    (void) polymon(lycanthropeType());
}

void
you_unwere(purify)
boolean purify;
{
    boolean controllable_poly = youHavePolymorphControl() && !(youAreStunned() || youAreUnaware());

    if (purify) {
        You_feel("purified.");
        setLycanthropeType(NON_PM); /* cure lycanthropy */
    }
    if (!youAreUnchanging() && isWere(youmonst.data->monsterTypeID)
        && (!controllable_poly || yn("Remain in beast form?") == 'n'))
        rehumanize();
}

boolean youHateSilver() {
    return (lycanthropeType() >= LOW_PM || hates_silver(youmonst.data));
}

/*were.c*/
