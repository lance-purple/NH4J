/* NetHack 3.6	steed.c	$NHDT-Date: 1445906867 2015/10/27 00:47:47 $  $NHDT-Branch: master $:$NHDT-Revision: 1.47 $ */
/* Copyright (c) Kevin Hugo, 1998-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* Monsters that might be ridden */
static NEARDATA const char steeds[] = { S_QUADRUPED, S_UNICORN, S_ANGEL,
                                        S_CENTAUR,   S_DRAGON,  S_JABBERWOCK,
                                        '\0' };

STATIC_DCL boolean FDECL(landing_spot, (coord *, int, int));
STATIC_DCL void FDECL(maybewakesteed, (struct monst *));

/* caller has decided that hero can't reach something while mounted */
void
rider_cant_reach()
{
    You("aren't skilled enough to reach from %s.", y_monnam(u.usteed));
}

/*** Putting the saddle on ***/

/* Can this monster wear a saddle? */
boolean
can_saddle(mtmp)
struct monst *mtmp;
{
    int pmid = pmid4mon(mtmp);
    return (index(steeds, monsterClass(pmid)) && (monsterSize(pmid) >= MZ_MEDIUM)
            && (!isHumanoid(pmid) || monsterClass(pmid) == S_CENTAUR) && !isAmorphous(pmid)
            && !isNoncorporeal(pmid) && !isWhirly(pmid) && !isUnsolid(pmid));
}

int
use_saddle(otmp)
struct obj *otmp;
{
    struct monst *mtmp;
    int chance;
    const char *s;

    if (!u_handsy())
        return 0;

    /* Select an animal */
    if (swallowed() || underwater() || !getdir((char *) 0)) {
        pline1(Never_mind);
        return 0;
    }
    if (!directionX() && !directionY()) {
        pline("Saddle yourself?  Very funny...");
        return 0;
    }
    if (!isok(currentX() + directionX(), currentY() + directionY())
        || !(mtmp = m_at(currentX() + directionX(), currentY() + directionY())) || !canspotmon(mtmp)) {
        pline("I see nobody there.");
        return 1;
    }

    /* Is this a valid monster? */
    if (mtmp->misc_worn_check & W_SADDLE || which_armor(mtmp, W_SADDLE)) {
        pline("%s doesn't need another one.", Monnam(mtmp));
        return 1;
    }
    int pmid = pmid4mon(mtmp);
    if (touchPetrifies(pmid) && !uarmg && !youResistStoning()) {
        char kbuf[BUFSZ];

        You("touch %s.", mon_nam(mtmp));
        if (!(monsterPolymorphsWhenStoned(pmid4you()) && polymon(PM_STONE_GOLEM))) {
            javaString monsterName = monsterTypeName(pmid4mon(mtmp));
            Sprintf(kbuf, "attempting to saddle %s", an(monsterName.c_str));
            releaseJavaString(monsterName);
            instapetrify(kbuf);
        }
    }
    if ((pmid == PM_INCUBUS) || (pmid == PM_SUCCUBUS)) {
        pline("Shame on you!");
        exercise(A_WIS, FALSE);
        return 1;
    }
    if (mtmp->isminion || mtmp->isshk || mtmp->ispriest || mtmp->isgd
        || mtmp->iswiz) {
        pline("I think %s would mind.", mon_nam(mtmp));
        return 1;
    }
    if (!can_saddle(mtmp)) {
        You_cant("saddle such a creature.");
        return 1;
    }

    /* Calculate your chance */
    chance = ACURR(A_DEX) + ACURR(A_CHA) / 2 + 2 * mtmp->mtame;
    chance += currentExperienceLevel() * (mtmp->mtame ? 20 : 5);
    if (!mtmp->mtame)
        chance -= 10 * mtmp->m_lev;
    if (Role_if(PM_KNIGHT))
        chance += 20;
    switch (weaponSkill(P_RIDING)) {
    case P_ISRESTRICTED:
    case P_UNSKILLED:
    default:
        chance -= 20;
        break;
    case P_BASIC:
        break;
    case P_SKILLED:
        chance += 15;
        break;
    case P_EXPERT:
        chance += 30;
        break;
    }
    if (youAreConfused() || youKeepFumbling() || youHaveSlipperyFingers())
        chance -= 20;
    else if (uarmg && (s = OBJ_DESCR(objects[uarmg->otyp])) != (char *) 0
             && !strncmp(s, "riding ", 7))
        /* Bonus for wearing "riding" (but not fumbling) gloves */
        chance += 10;
    else if (uarmf && (s = OBJ_DESCR(objects[uarmf->otyp])) != (char *) 0
             && !strncmp(s, "riding ", 7))
        /* ... or for "riding boots" */
        chance += 10;
    if (otmp->cursed)
        chance -= 50;

    /* [intended] steed becomes alert if possible */
    maybewakesteed(mtmp);

    /* Make the attempt */
    if (rn2(100) < chance) {
        You("put the saddle on %s.", mon_nam(mtmp));
        if (otmp->owornmask)
            remove_worn_item(otmp, FALSE);
        freeinv(otmp);
        /* mpickobj may free otmp it if merges, but we have already
           checked for a saddle above, so no merger should happen */
        (void) mpickobj(mtmp, otmp);
        mtmp->misc_worn_check |= W_SADDLE;
        otmp->owornmask = W_SADDLE;
        otmp->leashmon = mtmp->m_id;
        update_mon_intrinsics(mtmp, otmp, TRUE, FALSE);
    } else
        pline("%s resists!", Monnam(mtmp));
    return 1;
}

/*** Riding the monster ***/

/* Can we ride this monster?  Caller should also check can_saddle() */
boolean
can_ride(mtmp)
struct monst *mtmp;
{
    int upmid = pmid4you();
    return (mtmp->mtame && isHumanoid(upmid)
            && !isVerySmallMonster(upmid) && !isBigMonster(upmid)
            && (!underwater() || isSwimmer(upmid)));
}

int
doride()
{
    boolean forcemount = FALSE;

    if (u.usteed) {
        dismount_steed(DISMOUNT_BYCHOICE);
    } else if (getdir((char *) 0) && isok(currentX() + directionX(), currentY() + directionY())) {
        if (wizard && yn("Force the mount to succeed?") == 'y')
            forcemount = TRUE;
        return (mount_steed(m_at(currentX() + directionX(), currentY() + directionY()), forcemount));
    } else {
        return 0;
    }
    return 1;
}

/* Start riding, with the given monster */
boolean
mount_steed(mtmp, force)
struct monst *mtmp; /* The animal */
boolean force;      /* Quietly force this animal */
{
    struct obj *otmp;
    char buf[BUFSZ];

    /* Sanity checks */
    if (u.usteed) {
        You("are already riding %s.", mon_nam(u.usteed));
        return (FALSE);
    }

    /* Is the player in the right form? */
    if (youAreHallucinating() && !force) {
        pline("Maybe you should find a designated driver.");
        return (FALSE);
    }
    /* While riding Wounded_legs refers to the steed's,
     * not the hero's legs.
     * That opens up a potential abuse where the player
     * can mount a steed, then dismount immediately to
     * heal leg damage, because leg damage is always
     * healed upon dismount (Wounded_legs context switch).
     * By preventing a hero with wounded legs from
     * mounting a steed, the potential for abuse is
     * reduced.  However, dismounting still immediately
     * heals the steed's wounded legs.  [In 3.4.3 and
     * earlier, that unintentionally made the hero's
     * temporary 1 point Dex loss become permanent.]
     */
    if (youHaveWoundedLegs()) {
        Your("%s are in no shape for riding.", makeplural(body_part(LEG)));
        if (force && wizard && yn("Heal your legs?") == 'y') {
            setYourIntrinsic(WOUNDED_LEGS, 0L);
            setYourExtrinsic(WOUNDED_LEGS, 0L);
	}
        else
            return (FALSE);
    }

    int upmid = pmid4you();

    if (areYouPolymorphed() && (!isHumanoid(upmid) || isVerySmallMonster(upmid)
                   || isBigMonster(upmid) || isSlithy(upmid))) {
        You("won't fit on a saddle.");
        return (FALSE);
    }
    if (!force && (near_capacity() > SLT_ENCUMBER)) {
        You_cant("do that while carrying so much stuff.");
        return (FALSE);
    }

    /* Can the player reach and see the monster? */
    if (!mtmp || (!force && ((youCannotSee() && !youHaveTelepathyWhenBlind()) || mtmp->mundetected
                             || mtmp->m_ap_type == M_AP_FURNITURE
                             || mtmp->m_ap_type == M_AP_OBJECT))) {
        pline("I see nobody there.");
        return (FALSE);
    }
    if (swallowed() || u.ustuck || currentlyTrapped() || youAreBeingPunished()
        || !test_move(currentX(), currentY(), mtmp->mx - currentX(), mtmp->my - currentY(),
                      TEST_MOVE)) {
        if (youAreBeingPunished() || !(swallowed() || u.ustuck || currentlyTrapped()))
            You("are unable to swing your %s over.", body_part(LEG));
        else
            You("are stuck here for now.");
        return (FALSE);
    }

    /* Is this a valid monster? */
    otmp = which_armor(mtmp, W_SADDLE);
    if (!otmp) {
        pline("%s is not saddled.", Monnam(mtmp));
        return (FALSE);
    }
    int pmid = pmid4mon(mtmp);
    if (touchPetrifies(pmid) && !youResistStoning()) {
        char kbuf[BUFSZ];

        You("touch %s.", mon_nam(mtmp));
        javaString monsterName = monsterTypeName(pmid4mon(mtmp));
        Sprintf(kbuf, "attempting to ride %s", an(monsterName.c_str));
        releaseJavaString(monsterName);
        instapetrify(kbuf);
    }
    if (!mtmp->mtame || mtmp->isminion) {
        pline("I think %s would mind.", mon_nam(mtmp));
        return (FALSE);
    }
    if (mtmp->mtrapped) {
        struct trap *t = t_at(mtmp->mx, mtmp->my);

        You_cant("mount %s while %s's trapped in %s.", mon_nam(mtmp),
                 mhe(mtmp), an(defsyms[trap_to_defsym(t->ttyp)].explanation));
        return (FALSE);
    }

    if (!force && !Role_if(PM_KNIGHT) && !(--mtmp->mtame)) {
        /* no longer tame */
        newsym(mtmp->mx, mtmp->my);
        pline("%s resists%s!", Monnam(mtmp),
              mtmp->mleashed ? " and its leash comes off" : "");
        if (mtmp->mleashed)
            m_unleash(mtmp, FALSE);
        return (FALSE);
    }
    if (!force && underwater() && !isSwimmer(pmid)) {
        You_cant("ride that creature while under water.");
        return (FALSE);
    }
    if (!can_saddle(mtmp) || !can_ride(mtmp)) {
        You_cant("ride such a creature.");
        return (0);
    }

    /* Is the player impaired? */
    if (!force && !isFloater(pmid) && !isFlyer(pmid) && youAreLevitating()
        && !youCanLevitateAtWill()) {
        You("cannot reach %s.", mon_nam(mtmp));
        return (FALSE);
    }
    if (!force && uarm && is_metallic(uarm) && greatest_erosion(uarm)) {
        Your("%s armor is too stiff to be able to mount %s.",
             uarm->oeroded ? "rusty" : "corroded", mon_nam(mtmp));
        return (FALSE);
    }
    if (!force
        && (youAreConfused() || youKeepFumbling() || youHaveSlipperyFingers() || youHaveWoundedLegs() || otmp->cursed
            || (currentExperienceLevel() + mtmp->mtame < rnd(MAXULEV / 2 + 5)))) {
        if (youAreLevitating()) {
            pline("%s slips away from you.", Monnam(mtmp));
            return FALSE;
        }
        You("slip while trying to get on %s.", mon_nam(mtmp));

        Sprintf(buf, "slipped while mounting %s",
                /* "a saddled mumak" or "a saddled pony called Dobbin" */
                x_monnam(mtmp, ARTICLE_A, (char *) 0,
                         SUPPRESS_IT | SUPPRESS_INVISIBLE
                             | SUPPRESS_HALLUCINATION,
                         TRUE));
        losehp(Maybe_Half_Phys(rn1(5, 10)), buf, NO_KILLER_PREFIX);
        return (FALSE);
    }

    /* Success */
    maybewakesteed(mtmp);
    if (!force) {
        if (youAreLevitating() && !isFloater(pmid) && !isFlyer(pmid))
            /* Must have Lev_at_will at this point */
            pline("%s magically floats up!", Monnam(mtmp));
        You("mount %s.", mon_nam(mtmp));
    }
    /* setuwep handles polearms differently when you're mounted */
    if (uwep && is_pole(uwep))
        unweapon = FALSE;
    u.usteed = mtmp;
    remove_monster(mtmp->mx, mtmp->my);
    teleds(mtmp->mx, mtmp->my, TRUE);
    return (TRUE);
}

/* You and your steed have moved */
void
exercise_steed()
{
    if (!u.usteed)
        return;

    /* It takes many turns of riding to exercise skill */
    increaseTurnsRiddenSinceLastSkillCheck(1);
    if (turnsRiddenSinceLastSkillCheck() >= 100) {
        setTurnsRiddenSinceLastSkillCheck(0);
        use_skill(P_RIDING, 1);
    }
    return;
}

/* The player kicks or whips the steed */
void
kick_steed()
{
    char He[4];
    if (!u.usteed)
        return;

    /* [ALI] Various effects of kicking sleeping/paralyzed steeds */
    if (u.usteed->msleeping || !u.usteed->mcanmove) {
        /* We assume a message has just been output of the form
         * "You kick <steed>."
         */
        Strcpy(He, mhe(u.usteed));
        *He = highc(*He);
        if ((u.usteed->mcanmove || u.usteed->mfrozen) && !rn2(2)) {
            if (u.usteed->mcanmove)
                u.usteed->msleeping = 0;
            else if (u.usteed->mfrozen > 2)
                u.usteed->mfrozen -= 2;
            else {
                u.usteed->mfrozen = 0;
                u.usteed->mcanmove = 1;
            }
            if (u.usteed->msleeping || !u.usteed->mcanmove)
                pline("%s stirs.", He);
            else
                pline("%s rouses %sself!", He, mhim(u.usteed));
        } else
            pline("%s does not respond.", He);
        return;
    }

    /* Make the steed less tame and check if it resists */
    if (u.usteed->mtame)
        u.usteed->mtame--;
    if (!u.usteed->mtame && u.usteed->mleashed)
        m_unleash(u.usteed, TRUE);
    if (!u.usteed->mtame
        || (currentExperienceLevel() + u.usteed->mtame < rnd(MAXULEV / 2 + 5))) {
        newsym(u.usteed->mx, u.usteed->my);
        dismount_steed(DISMOUNT_THROWN);
        return;
    }

    pline("%s gallops!", Monnam(u.usteed));
    increaseRemainingGallopTime(rn1(20, 30));
    return;
}

/*
 * Try to find a dismount point adjacent to the steed's location.
 * If all else fails, try canPlaceMonsterNear().  Use this as a last
 * resort because canPlaceMonsterNear() chooses its point randomly,
 * possibly even outside the room's walls, which is not what we want.
 * Adapted from mail daemon code.
 */
STATIC_OVL boolean
landing_spot(spot, reason, forceit)
coord *spot; /* landing position (we fill it in) */
int reason;
int forceit;
{
    int i = 0, x, y, distance, min_distance = -1;
    boolean found = FALSE;
    struct trap *t;

    /* avoid known traps (i == 0) and boulders, but allow them as a backup */
    if (reason != DISMOUNT_BYCHOICE || youAreStunned() || youAreConfused() || youKeepFumbling())
        i = 1;
    for (; !found && i < 2; ++i) {
        for (x = currentX() - 1; x <= currentX() + 1; x++)
            for (y = currentY() - 1; y <= currentY() + 1; y++) {
                if (!isok(x, y) || (x == currentX() && y == currentY()))
                    continue;

                if (accessible(x, y) && !MON_AT(x, y)) {
                    distance = distanceSquaredToYou(x, y);
                    if (min_distance < 0 || distance < min_distance
                        || (distance == min_distance && rn2(2))) {
                        if (i > 0 || (((t = t_at(x, y)) == 0 || !t->tseen)
                                      && (!sobj_at(BOULDER, x, y)
                                          || throwsRocks(pmid4you())))) {
                            spot->x = x;
                            spot->y = y;
                            min_distance = distance;
                            found = TRUE;
                        }
                    }
                }
            }
    }

    /* If we didn't find a good spot and forceit is on, try canPlaceMonsterNear(). */
    if (forceit && min_distance < 0
        && !canPlaceMonsterNear(spot, currentX(), currentY(), pmid4you(), 0))
        return FALSE;

    return found;
}

/* Stop riding the current steed */
void
dismount_steed(reason)
int reason; /* Player was thrown off etc. */
{
    struct monst *mtmp;
    struct obj *otmp;
    coord cc;
    const char *verb = "fall";
    boolean repair_leg_damage = youHaveWoundedLegs();
    unsigned save_utrap = currentlyTrapped();
    boolean have_spot = landing_spot(&cc, reason, 0);

    mtmp = u.usteed; /* make a copy of steed pointer */
    /* Sanity check */
    if (!mtmp) /* Just return silently */
        return;

    /* Check the reason for dismounting */
    otmp = which_armor(mtmp, W_SADDLE);
    switch (reason) {
    case DISMOUNT_THROWN:
        verb = "are thrown";
    case DISMOUNT_FELL:
        You("%s off of %s!", verb, mon_nam(mtmp));
        if (!have_spot)
            have_spot = landing_spot(&cc, reason, 1);
        losehp(Maybe_Half_Phys(rn1(10, 10)), "riding accident", KILLED_BY_AN);
        set_wounded_legs(BOTH_SIDES, (int) yourIntrinsic(WOUNDED_LEGS) + rn1(5, 5));
        repair_leg_damage = FALSE;
        break;
    case DISMOUNT_POLY:
        You("can no longer ride %s.", mon_nam(u.usteed));
        if (!have_spot)
            have_spot = landing_spot(&cc, reason, 1);
        break;
    case DISMOUNT_ENGULFED:
        /* caller displays message */
        break;
    case DISMOUNT_BONES:
        /* hero has just died... */
        break;
    case DISMOUNT_GENERIC:
        /* no messages, just make it so */
        break;
    case DISMOUNT_BYCHOICE:
    default:
        if (otmp && otmp->cursed) {
            You("can't.  The saddle %s cursed.",
                otmp->bknown ? "is" : "seems to be");
            otmp->bknown = TRUE;
            return;
        }
        if (!have_spot) {
            You("can't. There isn't anywhere for you to stand.");
            return;
        }
        if (!has_mname(mtmp)) {
            javaString monsterName = monsterTypeName(pmid4mon(mtmp));
            pline("You've been through the dungeon on %s with no name.",
                  an(monsterName.c_str));
            releaseJavaString(monsterName);
            if (youAreHallucinating())
                pline("It felt good to get out of the rain.");
        } else
            You("dismount %s.", mon_nam(mtmp));
    }
    /* While riding, YouHaveWoundedLegs() refers to the steed's legs;
       after dismounting, it reverts to the hero's legs. */
    if (repair_leg_damage) {
        /* [TODO: make heal_legs() take a parameter to handle this] */
        in_steed_dismounting = TRUE;
        heal_legs();
        in_steed_dismounting = FALSE;
    }

    /* Release the steed and saddle */
    u.usteed = 0;
    setRemainingGallopTime(0L);

    /* Set player and steed's position.  Try moving the player first
       unless we're in the midst of creating a bones file. */
    if (reason == DISMOUNT_BONES) {
        /* move the steed to an adjacent square */
        if (canPlaceMonsterNear(&cc, currentX(), currentY(), pmid4mon(mtmp), 0))
            rloc_to(mtmp, cc.x, cc.y);
        else /* evidently no room nearby; move steed elsewhere */
            (void) rloc(mtmp, FALSE);
        return;
    }
    if (mtmp->mhp > 0) {
        place_monster(mtmp, currentX(), currentY());
        if (!swallowed() && !u.ustuck && have_spot) {
            int pmid = pmid4mon(mtmp);

            /* The steed may drop into water/lava */
            if (!isFlyer(pmid) && !isFloater(pmid) && !isClinger(pmid)) {
                if (is_pool(currentX(), currentY())) {
                    if (!underwater())
                        pline("%s falls into the %s!", Monnam(mtmp),
                              surface(currentX(), currentY()));
                    if (!isSwimmer(pmid) && !isAmphibious(pmid)) {
                        killed(mtmp);
                        adjalign(-1);
                    }
                } else if (is_lava(currentX(), currentY())) {
                    pline("%s is pulled into the lava!", Monnam(mtmp));
                    if (!likesLava(pmid)) {
                        killed(mtmp);
                        adjalign(-1);
                    }
                }
            }
            /* Steed dismounting consists of two steps: being moved to another
             * square, and descending to the floor.  We have functions to do
             * each of these activities, but they're normally called
             * individually and include an attempt to look at or pick up the
             * objects on the floor:
             * teleds() --> spoteffects() --> pickup()
             * float_down() --> pickup()
             * We use this kludge to make sure there is only one such attempt.
             *
             * Clearly this is not the best way to do it.  A full fix would
             * involve having these functions not call pickup() at all,
             * instead
             * calling them first and calling pickup() afterwards.  But it
             * would take a lot of work to keep this change from having any
             * unforeseen side effects (for instance, you would no longer be
             * able to walk onto a square with a hole, and autopickup before
             * falling into the hole).
             */
            /* [ALI] No need to move the player if the steed died. */
            if (mtmp->mhp > 0) {
                /* Keep steed here, move the player to cc;
                 * teleds() clears currentlyTrapped()
                 */
                in_steed_dismounting = TRUE;
                teleds(cc.x, cc.y, TRUE);
                in_steed_dismounting = FALSE;

                /* Put your steed in your trap */
                if (save_utrap)
                    (void) mintrap(mtmp);
            }
            /* Couldn't... try placing the steed */
        } else if (canPlaceMonsterNear(&cc, currentX(), currentY(), pmid4mon(mtmp), 0)) {
            /* Keep player here, move the steed to cc */
            rloc_to(mtmp, cc.x, cc.y);
            /* Player stays put */
            /* Otherwise, kill the steed */
        } else {
            killed(mtmp);
            adjalign(-1);
        }
    }

    /* Return the player to the floor */
    if (reason != DISMOUNT_ENGULFED) {
        in_steed_dismounting = TRUE;
        (void) float_down(0L, W_SADDLE);
        in_steed_dismounting = FALSE;
        context.botl = 1;
        (void) encumber_msg();
        vision_full_recalc = 1;
    } else
        context.botl = 1;
    /* polearms behave differently when not mounted */
    if (uwep && is_pole(uwep))
        unweapon = TRUE;
    return;
}

/* when attempting to saddle or mount a sleeping steed, try to wake it up
   (for the saddling case, it won't be u.usteed yet) */
STATIC_OVL void
maybewakesteed(steed)
struct monst *steed;
{
    int frozen = (int) steed->mfrozen;
    boolean wasimmobile = steed->msleeping || !steed->mcanmove;

    steed->msleeping = 0;
    if (frozen) {
        frozen = (frozen + 1) / 2; /* half */
        /* might break out of timed sleep or paralysis */
        if (!rn2(frozen)) {
            steed->mfrozen = 0;
            steed->mcanmove = 1;
        } else {
            /* didn't awake, but remaining duration is halved */
            steed->mfrozen = frozen;
        }
    }
    if (wasimmobile && !steed->msleeping && steed->mcanmove)
        pline("%s wakes up.", Monnam(steed));
    /* regardless of waking, terminate any meal in progress */
    finish_meating(steed);
}

/* decide whether hero's steed is able to move;
   doesn't check for holding traps--those affect the hero directly */
boolean
stucksteed(checkfeeding)
boolean checkfeeding;
{
    struct monst *steed = u.usteed;

    if (steed) {
        /* check whether steed can move */
        if (steed->msleeping || !steed->mcanmove) {
            pline("%s won't move!", upstart(y_monnam(steed)));
            return TRUE;
        }
        /* optionally check whether steed is in the midst of a meal */
        if (checkfeeding && steed->meating) {
            pline("%s is still eating.", upstart(y_monnam(steed)));
            return TRUE;
        }
    }
    return FALSE;
}

void
place_monster(mon, x, y)
struct monst *mon;
int x, y;
{
    if (mon == u.usteed
        /* special case is for convoluted vault guard handling */
        || (DEADMONSTER(mon) && !(mon->isgd && x == 0 && y == 0))) {
        impossible("placing %s onto map?",
                   (mon == u.usteed) ? "steed" : "defunct monster");
        return;
    }
    mon->mx = x, mon->my = y;
    level.monsters[x][y] = mon;
}

/*steed.c*/
