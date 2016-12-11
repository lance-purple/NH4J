/* NetHack 3.6	exper.c	$NHDT-Date: 1446975467 2015/11/08 09:37:47 $  $NHDT-Branch: master $:$NHDT-Revision: 1.26 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include <limits.h>

STATIC_DCL long FDECL(xpToReachNextLevel, (int));
STATIC_DCL int FDECL(enermod, (int));

STATIC_OVL long
xpToReachNextLevel(currentLevel)
int currentLevel;
{
    if (currentLevel < 10)
        return (10L * (1L << currentLevel));
    if (currentLevel < 20)
        return (10000L * (1L << (currentLevel - 10)));
    return (10000000L * ((long) (currentLevel - 19)));
}

STATIC_OVL int
enermod(en)
int en;
{
    switch (Role_switch) {
    case PM_PRIEST:
    case PM_WIZARD:
        return (2 * en);
    case PM_HEALER:
    case PM_KNIGHT:
        return ((3 * en) / 2);
    case PM_BARBARIAN:
    case PM_VALKYRIE:
        return ((3 * en) / 4);
    default:
        return en;
    }
}

/* calculate spell power/energy points for new level */
int
newpw()
{
    int en = 0, enrnd, enfix;

    if (currentExperienceLevel() == 0) {
        en = urole.enadv.infix + urace.enadv.infix;
        if (urole.enadv.inrnd > 0)
            en += rnd(urole.enadv.inrnd);
        if (urace.enadv.inrnd > 0)
            en += rnd(urace.enadv.inrnd);
    } else {
        enrnd = (int) ACURR(A_WIS) / 2;
        if (currentExperienceLevel() < urole.xlev) {
            enrnd += urole.enadv.lornd + urace.enadv.lornd;
            enfix = urole.enadv.lofix + urace.enadv.lofix;
        } else {
            enrnd += urole.enadv.hirnd + urace.enadv.hirnd;
            enfix = urole.enadv.hifix + urace.enadv.hifix;
        }
        en = enermod(rn1(enrnd, enfix));
    }
    if (en <= 0)
        en = 1;
    if (currentExperienceLevel() < MAXULEV) {
        setMagicalEnergyIncreasePerLevel(currentExperienceLevel(), en);
    }
    return en;
}

/* return # of exp points for mtmp after nk killed */
int
experience(mtmp, nk)
register struct monst *mtmp;
register int nk;
{
    register struct permonst *ptr = mtmp->data;
    int i, tmp, tmp2;

    tmp = 1 + mtmp->m_lev * mtmp->m_lev;

    /*  For higher ac values, give extra experience */
    if ((i = find_mac(mtmp)) < 3)
        tmp += (7 - i) * ((i < 0) ? 2 : 1);

    /*  For very fast monsters, give extra experience */
    if (ptr->mmove > NORMAL_SPEED)
        tmp += (ptr->mmove > (3 * NORMAL_SPEED / 2)) ? 5 : 3;

    /*  For each "special" attack type give extra experience */
    for (i = 0; i < NATTK; i++) {
        tmp2 = ptr->mattk[i].aatyp;
        if (tmp2 > AT_BUTT) {
            if (tmp2 == AT_WEAP)
                tmp += 5;
            else if (tmp2 == AT_MAGC)
                tmp += 10;
            else
                tmp += 3;
        }
    }

    /*  For each "special" damage type give extra experience */
    for (i = 0; i < NATTK; i++) {
        tmp2 = ptr->mattk[i].adtyp;
        if (tmp2 > AD_PHYS && tmp2 < AD_BLND)
            tmp += 2 * mtmp->m_lev;
        else if ((tmp2 == AD_DRLI) || (tmp2 == AD_STON) || (tmp2 == AD_SLIM))
            tmp += 50;
        else if (tmp2 != AD_PHYS)
            tmp += mtmp->m_lev;
        /* extra heavy damage bonus */
        if ((int) (ptr->mattk[i].damd * ptr->mattk[i].damn) > 23)
            tmp += mtmp->m_lev;
        if (tmp2 == AD_WRAP && ptr->mlet == S_EEL && !Amphibious)
            tmp += 1000;
    }

    /*  For certain "extra nasty" monsters, give even more */
    if (extra_nasty(ptr))
        tmp += (7 * mtmp->m_lev);

    /*  For higher level monsters, an additional bonus is given */
    if (mtmp->m_lev > 8)
        tmp += 50;

#ifdef MAIL
    /* Mail daemons put up no fight. */
    if (mtmp->data == &mons[PM_MAIL_DAEMON])
        tmp = 1;
#endif

    if (mtmp->mrevived || mtmp->mcloned) {
        /*
         *      Reduce experience awarded for repeated killings of
         *      "the same monster".  Kill count includes all of this
         *      monster's type which have been killed--including the
         *      current monster--regardless of how they were created.
         *        1.. 20        full experience
         *       21.. 40        xp / 2
         *       41.. 80        xp / 4
         *       81..120        xp / 8
         *      121..180        xp / 16
         *      181..240        xp / 32
         *      241..255+       xp / 64
         */
        for (i = 0, tmp2 = 20; nk > tmp2 && tmp > 1; ++i) {
            tmp = (tmp + 1) / 2;
            nk -= tmp2;
            if (i & 1)
                tmp2 += 20;
        }
    }

    return (tmp);
}

void
more_experienced(addXP, addScore)
register int addXP, addScore;
{
    long newexp = currentXP() + addXP;
    long rexpincr = 4 * addXP + addScore;
    long newrexp = u.urexp + rexpincr;

    /* cap experience and score on wraparound */
    if (newexp < 0 && addXP > 0)
        newexp = LONG_MAX;
    if (newrexp < 0 && rexpincr > 0)
        newrexp = LONG_MAX;
    setCurrentXP(newexp);
    u.urexp = newrexp;

    if (addXP
#ifdef SCORE_ON_BOTL
        || flags.showscore
#endif
        )
        context.botl = 1;
    if (u.urexp >= (Role_if(PM_WIZARD) ? 1000 : 2000))
        flags.beginner = 0;
}

/* e.g., hit by drain life attack */
void
losexp(drainer)
const char *drainer; /* cause of death, if drain should be fatal */
{
    register int num;

    /* override life-drain resistance when handling an explicit
       wizard mode request to reduce level; never fatal though */
    if (drainer && !strcmp(drainer, "#levelchange"))
        drainer = 0;
    else if (resists_drli(&youmonst))
        return;

    if (currentExperienceLevel() > 1) {
        pline("%s level %d.", Goodbye(), currentExperienceLevel());
        setCurrentExperienceLevel(currentExperienceLevel() - 1);
        /* remove intrinsic abilities */
        adjabil(currentExperienceLevel() + 1, currentExperienceLevel());
        reset_rndmonst(NON_PM); /* new monster selection */
    } else {
        if (drainer) {
            killer.format = KILLED_BY;
            if (killer.name != drainer)
                Strcpy(killer.name, drainer);
            done(DIED);
        }
        /* no drainer or lifesaved */
        setCurrentXP(0);
    }
    num = hitPointIncreasePerLevel(currentExperienceLevel());

    decreaseMaximumHitPoints(num);
    if (maximumHitPoints() < 1)
        setMaximumHitPoints(1);
    decreaseCurrentHitPoints(num);
    if (currentHitPoints() < 1)
        setCurrentHitPoints(1);
    else if (currentHitPoints() > maximumHitPoints())
        setCurrentHitPoints(maximumHitPoints());

    num = magicalEnergyIncreasePerLevel(currentExperienceLevel());
    decreaseMaximumMagicalEnergy(num);
    if (maximumMagicalEnergy() < 0)
        setMaximumMagicalEnergy(0);
    decreaseCurrentMagicalEnergy(num);
    if (currentMagicalEnergy() < 0)
        setCurrentMagicalEnergy(0);
    else if (currentMagicalEnergy() > maximumMagicalEnergy())
        setCurrentMagicalEnergy(maximumMagicalEnergy());

    if (currentXP() > 0)
        setCurrentXP(xpToReachNextLevel(currentExperienceLevel()) - 1);

    if (areYouPolymorphed()) {
        num = monhp_per_lvl(&youmonst);
        decreaseMaximumHitPointsAsMonster(num);
        decreaseCurrentHitPointsAsMonster(num);
        if (currentHitPointsAsMonster() <= 0)
            rehumanize();
    }

    context.botl = 1;
}

/*
 * Make experience gaining similar to AD&D(tm), whereby you can at most go
 * up by one level at a time, extra expr possibly helping you along.
 * After all, how much real experience does one get shooting a wand of death
 * at a dragon created with a wand of polymorph??
 */
void
newexplevel()
{
    if (currentExperienceLevel() < MAXULEV && currentXP() >= xpToReachNextLevel(currentExperienceLevel()))
        pluslvl(TRUE);
}

void
pluslvl(incr)
boolean incr; /* true iff via incremental experience growth */
{             /*        (false for potion of gain level)    */
    int hpinc, eninc;

    if (!incr)
        You_feel("more experienced.");

    /* increase hit points (when polymorphed, do monster form first
       in order to retain normal human/whatever increase for later) */
    if (areYouPolymorphed()) {
        hpinc = monhp_per_lvl(&youmonst);
        increaseMaximumHitPointsAsMonster(hpinc);
        increaseCurrentHitPointsAsMonster(hpinc);
    }
    hpinc = newhp();
    increaseMaximumHitPoints(hpinc);
    increaseCurrentHitPoints(hpinc);

    /* increase spell power/energy points */
    eninc = newpw();
    increaseMaximumMagicalEnergy(eninc);
    increaseCurrentMagicalEnergy(eninc);

    /* increase level (unless already maxxed) */
    if (currentExperienceLevel() < MAXULEV) {
        /* increase experience points to reflect new level */
        if (incr) {
            long tmp = xpToReachNextLevel(currentExperienceLevel() + 1);
            if (currentXP() >= tmp)
                setCurrentXP(tmp - 1);
        } else {
            setCurrentXP(xpToReachNextLevel(currentExperienceLevel()));
        }
        setCurrentExperienceLevel(currentExperienceLevel() + 1);
        if (highestExperienceLevelSoFar() < currentExperienceLevel()) {
            setHighestExperienceLevelSoFar(currentExperienceLevel());
        }
        pline("Welcome to experience level %d.", currentExperienceLevel());
        adjabil(currentExperienceLevel() - 1, currentExperienceLevel()); /* give new intrinsics */
        reset_rndmonst(NON_PM);          /* new monster selection */
    }
    context.botl = 1;
}

/* compute a random amount of experience points suitable for the hero's
   experience level:  base number of points needed to reach the current
   level plus a random portion of what it takes to get to the next level */
long
rndexp(gaining)
boolean gaining; /* gaining XP via potion vs setting XP for polyself */
{
    long minexp, maxexp, diff, factor, result;

    minexp = (currentExperienceLevel() == 1) ? 0L : xpToReachNextLevel(currentExperienceLevel() - 1);
    maxexp = xpToReachNextLevel(currentExperienceLevel());
    diff = maxexp - minexp, factor = 1L;
    /* make sure that `diff' is an argument which rn2() can handle */
    while (diff >= (long) LARGEST_INT)
        diff /= 2L, factor *= 2L;
    result = minexp + factor * (long) rn2((int) diff);
    /* 3.4.1:  if already at level 30, add to current experience
       points rather than to threshold needed to reach the current
       level; otherwise blessed potions of gain level can result
       in lowering the experience points instead of raising them */
    if (currentExperienceLevel() == MAXULEV && gaining) {
        result += (currentXP() - minexp);
        /* avoid wrapping (over 400 blessed potions needed for that...) */
        if (result < currentXP())
            result = currentXP();
    }
    return result;
}

/*exper.c*/
