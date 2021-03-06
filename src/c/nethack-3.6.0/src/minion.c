/* NetHack 3.6	minion.c	$NHDT-Date: 1432512773 2015/05/25 00:12:53 $  $NHDT-Branch: master $:$NHDT-Revision: 1.33 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

void
newemin(mtmp)
struct monst *mtmp;
{
    if (!mtmp->mextra)
        mtmp->mextra = newmextra();
    if (!EMIN(mtmp)) {
        EMIN(mtmp) = (struct emin *) alloc(sizeof(struct emin));
        (void) memset((genericptr_t) EMIN(mtmp), 0, sizeof(struct emin));
    }
}

void
free_emin(mtmp)
struct monst *mtmp;
{
    if (mtmp->mextra && EMIN(mtmp)) {
        free((genericptr_t) EMIN(mtmp));
        EMIN(mtmp) = (struct emin *) 0;
    }
    mtmp->isminion = 0;
}

/* count the number of monsters on the level */
int
monster_census(spotted)
boolean spotted; /* seen|sensed vs all */
{
    struct monst *mtmp;
    int count = 0;

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (DEADMONSTER(mtmp))
            continue;
        if (spotted && !canspotmon(mtmp))
            continue;
        ++count;
    }
    return count;
}

/* mon summons a monster */
int
msummon(mon)
struct monst *mon;
{
    int pmid;
    int dtype = NON_PM;
    int cnt = 0;
    aligntyp atyp;
    int maligntype;

    if (mon) {
        pmid = pmid4mon(mon);
        maligntype = monsterAlignment(pmid);
        atyp = mon->ispriest ? EPRI(mon)->shralign
                             : mon->isminion ? EMIN(mon)->min_align
                                             : (maligntype == A_NONE)
                                                   ? A_NONE
                                                   : sgn(maligntype);
    } else {
        pmid = PM_WIZARD_OF_YENDOR;
        maligntype = monsterAlignment(pmid);
        atyp = (maligntype == A_NONE) ? A_NONE : sgn(maligntype);
    }

    if (isDemonPrince(pmid) || (pmid == PM_WIZARD_OF_YENDOR)) {
        dtype = (!rn2(20)) ? dprince(atyp) : (!rn2(4)) ? dlord(atyp)
                                                       : ndemon(atyp);
        cnt = (!rn2(4) && isNamelessMajorDemon(dtype)) ? 2 : 1;
    } else if (isDemonLord(pmid)) {
        dtype = (!rn2(50)) ? dprince(atyp) : (!rn2(20)) ? dlord(atyp)
                                                        : ndemon(atyp);
        cnt = (!rn2(4) && isNamelessMajorDemon(dtype)) ? 2 : 1;
    } else if (isNamelessMajorDemon(pmid)) {
        dtype = (!rn2(20)) ? dlord(atyp) : (!rn2(6)) ? ndemon(atyp)
                                                     : pmid;
        cnt = 1;
    } else if (isLawfulMinion(pmid)) {
        dtype = (isLord(pmid) && !rn2(20))
                    ? llord()
                    : (isLord(pmid) || !rn2(6)) ? lminion() : pmid;
        cnt = (!rn2(4) && !isLord(dtype)) ? 2 : 1;
    } else if (pmid == PM_ANGEL) {
        /* non-lawful angels can also summon */
        if (!rn2(6)) {
            switch (atyp) { /* see summon_minion */
            case A_NEUTRAL:
                dtype = PM_AIR_ELEMENTAL + rn2(4);
                break;
            case A_CHAOTIC:
            case A_NONE:
                dtype = ndemon(atyp);
                break;
            }
        } else {
            dtype = PM_ANGEL;
        }
        cnt = (!rn2(4) && !isLord(dtype)) ? 2 : 1;
    }

    if (dtype == NON_PM)
        return 0;

    /* sanity checks */
    if (cnt > 1 && (monsterGenerationMask(dtype) & G_UNIQ))
        cnt = 1;
    /*
     * If this daemon is unique and being re-summoned (the only way we
     * could get this far with an extinct dtype), try another.
     */
    if (mvitals[dtype].mvflags & G_GONE) {
        dtype = ndemon(atyp);
        if (dtype == NON_PM)
            return 0;
    }

    /* some candidates can generate a group of monsters, so simple
       count of non-null makeMonsterOfType() result is not sufficient */
    int census = monster_census(FALSE);
    int result = 0;

    while (cnt > 0) {
        struct monst *mtmp = makeMonsterOfType(dtype, currentX(), currentY(), MM_EMIN);
        if (mtmp) {
            result++;
            /* an angel's alignment should match the summoner */
            if (dtype == PM_ANGEL) {
                mtmp->isminion = 1;
                EMIN(mtmp)->min_align = atyp;
                /* renegade if same alignment but not peaceful
                   or peaceful but different alignment */
                EMIN(mtmp)->renegade =
                    (atyp != currentAlignmentType()) ^ !mtmp->mpeaceful;
            }
        }
        cnt--;
    }

    /* how many monsters exist now compared to before? */
    if (result)
    {
        result = monster_census(FALSE) - census;
    }

    return result;
}

void
summon_minion(alignment, talk)
aligntyp alignment;
boolean talk;
{
    register struct monst *mon;
    int mnum;

    switch ((int) alignment) {
    case A_LAWFUL:
        mnum = lminion();
        break;
    case A_NEUTRAL:
        mnum = PM_AIR_ELEMENTAL + rn2(4);
        break;
    case A_CHAOTIC:
    case A_NONE:
        mnum = ndemon(alignment);
        break;
    default:
        impossible("unaligned player?");
        mnum = ndemon(A_NONE);
        break;
    }
    if (mnum == NON_PM) {
        mon = 0;
    } else if (mnum == PM_ANGEL) {
        mon = makeMonsterOfType(mnum, currentX(), currentY(), MM_EMIN);
        if (mon) {
            mon->isminion = 1;
            EMIN(mon)->min_align = alignment;
            EMIN(mon)->renegade = FALSE;
        }
    } else if (mnum != PM_SHOPKEEPER && mnum != PM_GUARD
               && mnum != PM_ALIGNED_PRIEST && mnum != PM_HIGH_PRIEST) {
        /* This was mons[mnum].pxlth == 0 but is this restriction
           appropriate or necessary now that the structures are separate? */
        mon = makeMonsterOfType(mnum, currentX(), currentY(), MM_EMIN);
        if (mon) {
            mon->isminion = 1;
            EMIN(mon)->min_align = alignment;
            EMIN(mon)->renegade = FALSE;
        }
    } else {
        mon = makeMonsterOfType(mnum, currentX(), currentY(), NO_MM_FLAGS);
    }
    if (mon) {
        if (talk) {
	    javaString deity = nameOfAlignedDeityFromYourPantheon(alignment);
            pline_The("voice of %s booms:", deity.c_str);
	    releaseJavaString(deity);

            verbalize("Thou shalt pay for thine indiscretion!");
            if (youCanSee())
	    {
                pline("%s appears before you.", Amonnam(mon));
	    }
            mon->mstrategy &= ~STRAT_APPEARMSG;
        }
        mon->mpeaceful = FALSE;
        /* don't call set_malign(); player was naughty */
    }
}

#define Athome (areYouInHell() && (mtmp->cham == NON_PM))

/* returns 1 if it won't attack. */
int
demon_talk(mtmp)
register struct monst *mtmp;
{
    long cash, demand, offer;

    if (uwep && uwep->oartifact == ART_EXCALIBUR) {
        pline("%s looks very angry.", Amonnam(mtmp));
        mtmp->mpeaceful = mtmp->mtame = 0;
        set_malign(mtmp);
        newsym(mtmp->mx, mtmp->my);
        return 0;
    }

    if (is_fainted()) {
        reset_faint(); /* if fainted - wake up */
    } else {
        stop_occupation();
        if (multi > 0) {
            nomul(0);
            unmul((char *) 0);
        }
    }

    /* Slight advantage given. */
    if (isDemonPrince(pmid4mon(mtmp)) && mtmp->minvis) {
        boolean wasunseen = !canspotmon(mtmp);

        mtmp->minvis = mtmp->perminvis = 0;
        if (wasunseen && canspotmon(mtmp)) {
            pline("%s appears before you.", Amonnam(mtmp));
            mtmp->mstrategy &= ~STRAT_APPEARMSG;
        }
        newsym(mtmp->mx, mtmp->my);
    }
    if (monsterClass(pmid4you()) == S_DEMON) { /* Won't blackmail their own. */
        pline("%s says, \"Good hunting, %s.\"", Amonnam(mtmp),
              flags.female ? "Sister" : "Brother");
        if (!tele_restrict(mtmp))
            (void) rloc(mtmp, TRUE);
        return (1);
    }
    cash = money_cnt(invent);
    demand =
        (cash * (rnd(80) + 20 * Athome))
        / (100 * (1 + (sgn(currentAlignmentType()) == sgn(monsterAlignment(pmid4mon(mtmp))))));

    if (!demand || multi < 0) { /* you have no gold or can't move */
        mtmp->mpeaceful = 0;
        set_malign(mtmp);
        return 0;
    } else {
        /* make sure that the demand is unmeetable if the monster
           has the Amulet, preventing monster from being satisfied
           and removed from the game (along with said Amulet...) */
        if (mon_has_amulet(mtmp))
            demand = cash + (long) rn1(1000, 40);

        pline("%s demands %ld %s for safe passage.", Amonnam(mtmp), demand,
              currency(demand));

        if ((offer = bribe(mtmp)) >= demand) {
            pline("%s vanishes, laughing about cowardly mortals.",
                  Amonnam(mtmp));
        } else if (offer > 0L && (long) rnd(40) > (demand - offer)) {
            pline("%s scowls at you menacingly, then vanishes.",
                  Amonnam(mtmp));
        } else {
            pline("%s gets angry...", Amonnam(mtmp));
            mtmp->mpeaceful = 0;
            set_malign(mtmp);
            return 0;
        }
    }
    mongone(mtmp);
    return (1);
}

long
bribe(mtmp)
struct monst *mtmp;
{
    char buf[BUFSZ];
    long offer;
    long umoney = money_cnt(invent);

    getlin("How much will you offer?", buf);
    if (sscanf(buf, "%ld", &offer) != 1)
        offer = 0L;

    /*Michael Paddon -- fix for negative offer to monster*/
    /*JAR880815 - */
    if (offer < 0L) {
        You("try to shortchange %s, but fumble.", mon_nam(mtmp));
        return 0L;
    } else if (offer == 0L) {
        You("refuse.");
        return 0L;
    } else if (offer >= umoney) {
        You("give %s all your gold.", mon_nam(mtmp));
        offer = umoney;
    } else {
        You("give %s %ld %s.", mon_nam(mtmp), offer, currency(offer));
    }
    (void) money2mon(mtmp, offer);
    context.botl = 1;
    return (offer);
}

int
dprince(atyp)
aligntyp atyp;
{
    int tryct, pm;

    for (tryct = !areYouInEndgame() ? 20 : 0; tryct > 0; --tryct) {
        pm = rn1(PM_DEMOGORGON + 1 - PM_ORCUS, PM_ORCUS);
        if (!(mvitals[pm].mvflags & G_GONE)
            && (atyp == A_NONE || sgn(monsterAlignment(pm)) == sgn(atyp)))
            return (pm);
    }
    return (dlord(atyp)); /* approximate */
}

int
dlord(atyp)
aligntyp atyp;
{
    int tryct, pm;

    for (tryct = !areYouInEndgame() ? 20 : 0; tryct > 0; --tryct) {
        pm = rn1(PM_YEENOGHU + 1 - PM_JUIBLEX, PM_JUIBLEX);
        if (!(mvitals[pm].mvflags & G_GONE)
            && (atyp == A_NONE || sgn(monsterAlignment(pm)) == sgn(atyp)))
            return (pm);
    }
    return (ndemon(atyp)); /* approximate */
}

/* create lawful (good) lord */
int
llord()
{
    if (!(mvitals[PM_ARCHON].mvflags & G_GONE))
        return (PM_ARCHON);

    return (lminion()); /* approximate */
}

int
lminion()
{
    int tryct;
    int pmid;

    for (tryct = 0; tryct < 20; tryct++) {
        pmid = pickMonsterTypeOfClass(S_ANGEL, 0);
        if ((NON_PM != pmid) && !isLord(pmid)) {
            return pmid;
        }
    }

    return NON_PM;
}

int
ndemon(atyp)
aligntyp atyp;
{
    int tryct;
    int pmid;

    for (tryct = 0; tryct < 20; tryct++) {
        pmid = pickMonsterTypeOfClass(S_DEMON, 0);
        if ((NON_PM != pmid) && isNamelessMajorDemon(pmid)
            && (atyp == A_NONE || sgn(monsterAlignment(pmid)) == sgn(atyp)))
            return (pmid);
    }

    return NON_PM;
}

/* guardian angel has been affected by conflict so is abandoning hero */
void
lose_guardian_angel(mon)
struct monst *mon; /* if null, angel hasn't been created yet */
{
    coord mm;
    int i;

    if (mon) {
        if (canspotmon(mon)) {
            if (!youAreDeaf()) {
                pline("%s rebukes you, saying:", Monnam(mon));
                verbalize("Since you desire conflict, have some more!");
            } else {
                pline("%s vanishes!", Monnam(mon));
            }
        }
        mongone(mon);
    }
    /* create 2 to 4 hostile angels to replace the lost guardian */
    for (i = rn1(3, 2); i > 0; --i) {
        mm.x = currentX();
        mm.y = currentY();
        if (canPlaceMonsterNear(&mm, mm.x, mm.y, PM_ANGEL, 0))
            (void) makeRoamingMonsterOfType(PM_ANGEL, currentAlignmentType(), mm.x, mm.y, FALSE);
    }
}

/* just entered the Astral Plane; receive tame guardian angel if worthy */
void
gain_guardian_angel()
{
    struct monst *mtmp;
    struct obj *otmp;
    coord mm;

    Hear_again(); /* attempt to cure any deafness now (divine
                     message will be heard even if that fails) */
    if (youCauseConflict()) {
        pline("A voice booms:");
        verbalize("Thy desire for conflict shall be fulfilled!");
        /* send in some hostile angels instead */
        lose_guardian_angel((struct monst *) 0);
    } else if (currentAlignmentRecord() > 8) { /* fervent */
        pline("A voice whispers:");
        verbalize("Thou hast been worthy of me!");
        mm.x = currentX();
        mm.y = currentY();
        if (canPlaceMonsterNear(&mm, mm.x, mm.y, PM_ANGEL, 0)
            && (mtmp = makeRoamingMonsterOfType(PM_ANGEL, currentAlignmentType(), mm.x, mm.y,
                                 TRUE)) != 0) {
            mtmp->mstrategy &= ~STRAT_APPEARMSG;
            if (youCanSee())
                pline("An angel appears near you.");
            else
                You_feel("the presence of a friendly angel near you.");
            /* guardian angel -- the one case mtame doesn't
             * imply an edog structure, so we don't want to
             * call tamedog().
             */
            mtmp->mtame = 10;
            /* make him strong enough vs. endgame foes */
            mtmp->m_lev = rn1(8, 15);
            mtmp->mhp = mtmp->mhpmax =
                d((int) mtmp->m_lev, 10) + 30 + rnd(30);
            if ((otmp = select_hwep(mtmp)) == 0) {
                otmp = mksobj(SILVER_SABER, FALSE, FALSE);
                if (mpickobj(mtmp, otmp))
                    panic("merged weapon?");
            }
            bless(otmp);
            if (otmp->spe < 4)
                otmp->spe += rnd(4);
            if ((otmp = which_armor(mtmp, W_ARMS)) == 0
                || otmp->otyp != SHIELD_OF_REFLECTION) {
                (void) mongets(mtmp, AMULET_OF_REFLECTION);
                m_dowear(mtmp, TRUE);
            }
        }
    }
}

/*minion.c*/
