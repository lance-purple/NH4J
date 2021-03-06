/* NetHack 3.6	fountain.c	$NHDT-Date: 1444937416 2015/10/15 19:30:16 $  $NHDT-Branch: master $:$NHDT-Revision: 1.55 $ */
/*	Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"

STATIC_DCL void NDECL(dowatersnakes);
STATIC_DCL void NDECL(dowaterdemon);
STATIC_DCL void NDECL(dowaternymph);
STATIC_PTR void FDECL(gush, (int, int, genericptr_t));
STATIC_DCL void NDECL(dofindgem);

/* used when trying to dip in or drink from fountain or sink or pool while
   levitating above it, or when trying to move downwards in that state */
void
floating_above(what)
const char *what;
{
    const char *umsg = "are floating high above the %s.";

    if (currentlyTrapped() && (currentTrapType() == TT_INFLOOR || currentTrapType() == TT_LAVA)) {
        /* when stuck in floor (not possible at fountain or sink location,
           so must be attempting to move down), override the usual message */
        umsg = "are trapped in the %s.";
        what = surface(currentX(), currentY()); /* probably redundant */
    }
    You(umsg, what);
}

/* Fountain of snakes! */
STATIC_OVL void
dowatersnakes()
{
    register int num = rn1(5, 2);
    struct monst *mtmp;

    if (!(mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
        if (youCanSee())
            pline("An endless stream of %s pours forth!",
                  youAreHallucinating() ? makeplural(rndmonnam(NULL)) : "snakes");
        else
            You_hear("%s hissing!", something);
        while (num-- > 0)
            if ((mtmp = makeMonsterOfType(PM_WATER_MOCCASIN, currentX(), currentY(),
                                NO_MM_FLAGS)) != 0
                && t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water demon */
STATIC_OVL void
dowaterdemon()
{
    struct monst *mtmp;

    if (!(mvitals[PM_WATER_DEMON].mvflags & G_GONE)) {
        if ((mtmp = makeMonsterOfType(PM_WATER_DEMON, currentX(), currentY(), NO_MM_FLAGS)) != 0) {
            if (youCanSee())
                You("unleash %s!", a_monnam(mtmp));
            else
                You_feel("the presence of evil.");

            /* Give those on low levels a (slightly) better chance of survival
             */
            if (rnd(100) > (80 + level_difficulty())) {
                int gender = pronoun_gender(mtmp);
		javaString subjective = subjectivePronoun(gender);
		javaString possessive = possessivePronoun(gender);
                pline("Grateful for %s release, %s grants you a wish!",
                      possessive.c_str, subjective.c_str);
		releaseJavaString(subjective);
		releaseJavaString(possessive);
                /* give a wish and discard the monster (mtmp set to null) */
                mongrantswish(&mtmp);
            } else if (t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
        }
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water Nymph */
STATIC_OVL void
dowaternymph()
{
    register struct monst *mtmp;

    if (!(mvitals[PM_WATER_NYMPH].mvflags & G_GONE)
        && (mtmp = makeMonsterOfType(PM_WATER_NYMPH, currentX(), currentY(), NO_MM_FLAGS)) != 0) {
        if (youCanSee())
            You("attract %s!", a_monnam(mtmp));
        else
            You_hear("a seductive voice.");
        mtmp->msleeping = 0;
        if (t_at(mtmp->mx, mtmp->my))
            (void) mintrap(mtmp);
    } else if (youCanSee())
        pline("A large bubble rises to the surface and pops.");
    else
        You_hear("a loud pop.");
}

/* Gushing forth along LOS from (currentX(), currentY()) */
void
dogushforth(drinking)
int drinking;
{
    int madepool = 0;

    do_clear_area(currentX(), currentY(), 7, gush, (genericptr_t) &madepool);
    if (!madepool) {
        if (drinking)
            Your("thirst is quenched.");
        else
            pline("Water sprays all over you.");
    }
}

STATIC_PTR void
gush(x, y, poolcnt)
int x, y;
genericptr_t poolcnt;
{
    register struct monst *mtmp;
    register struct trap *ttmp;

    if (((x + y) % 2) || (x == currentX() && y == currentY())
        || (rn2(1 + distmin(currentX(), currentY(), x, y))) || (levl[x][y].typ != ROOM)
        || (sobj_at(BOULDER, x, y)) || nexttodoor(x, y))
        return;

    if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
        return;

    if (!((*(int *) poolcnt)++))
        pline("Water gushes forth from the overflowing fountain!");

    /* Put a pool at x, y */
    levl[x][y].typ = POOL;
    /* No kelp! */
    del_engr_at(x, y);
    water_damage_chain(level.objects[x][y], TRUE);

    if ((mtmp = m_at(x, y)) != 0)
        (void) minliquid(mtmp);
    else
        newsym(x, y);
}

/* Find a gem in the sparkling waters. */
STATIC_OVL void
dofindgem()
{
    if (youCanSee())
        You("spot a gem in the sparkling waters!");
    else
        You_feel("a gem here!");
    (void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE - 1), currentX(), currentY(),
                     FALSE, FALSE);
    SET_FOUNTAIN_LOOTED(currentX(), currentY());
    newsym(currentX(), currentY());
    exercise(A_WIS, TRUE); /* a discovery! */
}

void
dryup(x, y, isyou)
xchar x, y;
boolean isyou;
{
    if (IS_FOUNTAIN(levl[x][y].typ)
        && (!rn2(3) || FOUNTAIN_IS_WARNED(x, y))) {
        if (isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x, y)) {
            struct monst *mtmp;

            SET_FOUNTAIN_WARNED(x, y);
            /* Warn about future fountain use. */
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                if (isWatchman(pmid4mon(mtmp)) && couldsee(mtmp->mx, mtmp->my)
                    && mtmp->mpeaceful) {
                    pline("%s yells:", Amonnam(mtmp));
                    verbalize("Hey, stop using that fountain!");
                    break;
                }
            }
            /* You can see or hear this effect */
            if (!mtmp)
                pline_The("flow reduces to a trickle.");
            return;
        }
        if (isyou && wizard) {
            if (yn("Dry up fountain?") == 'n')
                return;
        }
        /* replace the fountain with ordinary floor */
        levl[x][y].typ = ROOM;
        levl[x][y].looted = 0;
        levl[x][y].blessedftn = 0;
        if (cansee(x, y))
            pline_The("fountain dries up!");
        /* The location is seen if the hero/monster is invisible
           or felt if the hero is blind. */
        newsym(x, y);
        level.flags.nfountains--;
        if (isyou && in_town(x, y))
            (void) angry_guards(FALSE);
    }
}

void
drinkfountain()
{
    /* What happens when you drink from a fountain? */
    register boolean mgkftn = (levl[currentX()][currentY()].blessedftn == 1);
    register int fate = rnd(30);

    if (youAreLevitating()) {
        floating_above("fountain");
        return;
    }

    if (mgkftn && currentLuck() >= 0 && fate >= 10) {
        int i, ii, littleluck = (currentLuck() < 4);

        pline("Wow!  This makes you feel great!");
        /* blessed restore ability */
        for (ii = 0; ii < A_MAX; ii++)
            if (yourCurrentAttr(ii) < yourAttrMax(ii)) {
                setYourCurrentAttr(ii, yourAttrMax(ii));
                context.botl = 1;
            }
        /* gain ability, blessed if "natural" luck is high */
        i = rn2(A_MAX); /* start at a random attribute */
        for (ii = 0; ii < A_MAX; ii++) {
            if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
                break;
            if (++i >= A_MAX)
                i = 0;
        }
        display_nhwindow(WIN_MESSAGE, FALSE);
        pline("A wisp of vapor escapes the fountain...");
        exercise(A_WIS, TRUE);
        levl[currentX()][currentY()].blessedftn = 0;
        return;
    }

    if (fate < 10) {
        pline_The("cool draught refreshes you.");
        increaseCurrentNutrition(rnd(10)); /* don't choke on water */
        newuhs(FALSE);
        if (mgkftn)
            return;
    } else {
        switch (fate) {
        case 19: /* Self-knowledge */
            You_feel("self-knowledgeable...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            exercise(A_WIS, TRUE);
            pline_The("feeling subsides.");
            break;
        case 20: /* Foul water */
            pline_The("water is foul!  You gag and vomit.");
            morehungry(rn1(20, 11));
            vomit();
            break;
        case 21: /* Poisonous */
            pline_The("water is contaminated!");
            if (youResistPoison()) {
                pline("Perhaps it is runoff from the nearby %s farm.",
                      fruitname(FALSE));
                losehp(rnd(4), "unrefrigerated sip of juice", KILLED_BY_AN);
                break;
            }
            losestr(rn1(4, 3));
            losehp(rnd(10), "contaminated water", KILLED_BY);
            exercise(A_CON, FALSE);
            break;
        case 22: /* Fountain of snakes! */
            dowatersnakes();
            break;
        case 23: /* Water demon */
            dowaterdemon();
            break;
        case 24: /* Curse an item */ {
            register struct obj *obj;

            pline("This water's no good!");
            morehungry(rn1(20, 11));
            exercise(A_CON, FALSE);
            for (obj = invent; obj; obj = obj->nobj)
                if (!rn2(5))
                    curse(obj);
            break;
        }
        case 25: /* See invisible */
            if (youCannotSee()) {
                if (youAreFullyInvisible()) {
                    You("feel transparent.");
                } else {
                    You("feel very self-conscious.");
                    pline("Then it passes.");
                }
            } else {
                You_see("an image of someone stalking you.");
                pline("But it disappears.");
            }
            setYourIntrinsicMask(SEE_INVIS, FROMOUTSIDE);
            newsym(currentX(), currentY());
            exercise(A_WIS, TRUE);
            break;
        case 26: /* See Monsters */
            (void) monster_detect((struct obj *) 0, 0);
            exercise(A_WIS, TRUE);
            break;
        case 27: /* Find a gem in the sparkling waters. */
            if (!FOUNTAIN_IS_LOOTED(currentX(), currentY())) {
                dofindgem();
                break;
            }
        case 28: /* Water Nymph */
            dowaternymph();
            break;
        case 29: /* Scare */
        {
            register struct monst *mtmp;

            pline("This water gives you bad breath!");
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                monflee(mtmp, 0, FALSE, FALSE);
            }
            break;
        }
        case 30: /* Gushing forth in this room */
            dogushforth(TRUE);
            break;
        default:
            pline("This tepid water is tasteless.");
            break;
        }
    }
    dryup(currentX(), currentY(), TRUE);
}

void
dipfountain(obj)
register struct obj *obj;
{
    if (youAreLevitating()) {
        floating_above("fountain");
        return;
    }

    /* Don't grant Excalibur when there's more than one object.  */
    /* (quantity could be > 1 if merged daggers got polymorphed) */
    if (obj->otyp == LONG_SWORD && obj->quan == 1L && currentExperienceLevel() >= 5 && !rn2(6)
        && !obj->oartifact
        && !exist_artifact(LONG_SWORD, artiname(ART_EXCALIBUR))) {
        if (currentAlignmentType() != A_LAWFUL) {
            /* Ha!  Trying to cheat her. */
            pline(
             "A freezing mist rises from the water and envelopes the sword.");
            pline_The("fountain disappears!");
            curse(obj);
            if (obj->spe > -6 && !rn2(3))
                obj->spe--;
            obj->oerodeproof = FALSE;
            exercise(A_WIS, FALSE);
        } else {
            /* The lady of the lake acts! - Eric Backus */
            /* Be *REAL* nice */
            pline(
              "From the murky depths, a hand reaches up to bless the sword.");
            pline("As the hand retreats, the fountain disappears!");
            obj = oname(obj, artiname(ART_EXCALIBUR));
            discover_artifact(ART_EXCALIBUR);
            bless(obj);
            obj->oeroded = obj->oeroded2 = 0;
            obj->oerodeproof = TRUE;
            exercise(A_WIS, TRUE);
        }
        update_inventory();
        levl[currentX()][currentY()].typ = ROOM;
        levl[currentX()][currentY()].looted = 0;
        newsym(currentX(), currentY());
        level.flags.nfountains--;
        if (in_town(currentX(), currentY()))
            (void) angry_guards(FALSE);
        return;
    } else {
        int er = water_damage(obj, NULL, TRUE);

        if (obj->otyp == POT_ACID
            && er != ER_DESTROYED) { /* Acid and water don't mix */
            useup(obj);
            return;
        } else if (er != ER_NOTHING && !rn2(2)) { /* no further effect */
            return;
        }
    }

    switch (rnd(30)) {
    case 16: /* Curse the item */
        curse(obj);
        break;
    case 17:
    case 18:
    case 19:
    case 20: /* Uncurse the item */
        if (obj->cursed) {
            if (youCanSee())
                pline_The("water glows for a moment.");
            uncurse(obj);
        } else {
            pline("A feeling of loss comes over you.");
        }
        break;
    case 21: /* Water Demon */
        dowaterdemon();
        break;
    case 22: /* Water Nymph */
        dowaternymph();
        break;
    case 23: /* an Endless Stream of Snakes */
        dowatersnakes();
        break;
    case 24: /* Find a gem */
        if (!FOUNTAIN_IS_LOOTED(currentX(), currentY())) {
            dofindgem();
            break;
        }
    case 25: /* Water gushes forth */
        dogushforth(FALSE);
        break;
    case 26: /* Strange feeling */
        pline("A strange tingling runs up your %s.", body_part(ARM));
        break;
    case 27: /* Strange feeling */
        You_feel("a sudden chill.");
        break;
    case 28: /* Strange feeling */
        pline("An urge to take a bath overwhelms you.");
        {
            long money = money_cnt(invent);
            struct obj *otmp;
            if (money > 10) {
                /* Amount to lose.  Might get rounded up as fountains don't
                 * pay change... */
                money = somegold(money) / 10;
                for (otmp = invent; otmp && money > 0; otmp = otmp->nobj)
                    if (otmp->oclass == COIN_CLASS) {
                        int denomination = objects[otmp->otyp].oc_cost;
                        long coin_loss =
                            (money + denomination - 1) / denomination;
                        coin_loss = min(coin_loss, otmp->quan);
                        otmp->quan -= coin_loss;
                        money -= coin_loss * denomination;
                        if (!otmp->quan)
                            delobj(otmp);
                    }
                You("lost some of your money in the fountain!");
                CLEAR_FOUNTAIN_LOOTED(currentX(), currentY());
                exercise(A_WIS, FALSE);
            }
        }
        break;
    case 29: /* You see coins */
        /* We make fountains have more coins the closer you are to the
         * surface.  After all, there will have been more people going
         * by.	Just like a shopping mall!  Chris Woodbury  */

        if (FOUNTAIN_IS_LOOTED(currentX(), currentY()))
            break;
        SET_FOUNTAIN_LOOTED(currentX(), currentY());
        (void) mkgold((long) (rnd((levelsInCurrentDungeon() - currentDungeonLevel()
                                   + 1) * 2) + 5),
                      currentX(), currentY());
        if (youCanSee())
            pline("Far below you, you see coins glistening in the water.");
        exercise(A_WIS, TRUE);
        newsym(currentX(), currentY());
        break;
    }
    update_inventory();
    dryup(currentX(), currentY(), TRUE);
}

void
breaksink(x, y)
int x, y;
{
    if (cansee(x, y) || (x == currentX() && y == currentY()))
        pline_The("pipes break!  Water spurts out!");
    level.flags.nsinks--;
    levl[x][y].doormask = 0;
    levl[x][y].typ = FOUNTAIN;
    level.flags.nfountains++;
    newsym(x, y);
}

void
drinksink()
{
    struct obj *otmp;
    struct monst *mtmp;

    if (youAreLevitating()) {
        floating_above("sink");
        return;
    }
    switch (rn2(20)) {
    case 0:
        You("take a sip of very cold water.");
        break;
    case 1:
        You("take a sip of very warm water.");
        break;
    case 2:
        You("take a sip of scalding hot water.");
        if (youResistFire())
            pline("It seems quite tasty.");
        else
            losehp(rnd(6), "sipping boiling water", KILLED_BY);
        /* boiling water burns considered fire damage */
        break;
    case 3:
        if (mvitals[PM_SEWER_RAT].mvflags & G_GONE)
            pline_The("sink seems quite dirty.");
        else {
            mtmp = makeMonsterOfType(PM_SEWER_RAT, currentX(), currentY(), NO_MM_FLAGS);
            if (mtmp)
                pline("Eek!  There's %s in the sink!",
                      (youCannotSee() || !canspotmon(mtmp)) ? "something squirmy"
                                                   : a_monnam(mtmp));
        }
        break;
    case 4:
        do {
            otmp = mkobj(POTION_CLASS, FALSE);
            if (otmp->otyp == POT_WATER) {
                obfree(otmp, (struct obj *) 0);
                otmp = (struct obj *) 0;
            }
        } while (!otmp);
        otmp->cursed = otmp->blessed = 0;
        pline("Some %s liquid flows from the faucet.",
              youCannotSee() ? "odd" : hcolor(OBJ_DESCR(objects[otmp->otyp])));
        otmp->dknown = !(youCannotSee() || youAreHallucinating());
        otmp->quan++;       /* Avoid panic upon useup() */
        otmp->fromsink = 1; /* kludge for docall() */
        (void) dopotion(otmp);
        obfree(otmp, (struct obj *) 0);
        break;
    case 5:
        if (!(levl[currentX()][currentY()].looted & S_LRING)) {
            You("find a ring in the sink!");
            (void) mkobj_at(RING_CLASS, currentX(), currentY(), TRUE);
            levl[currentX()][currentY()].looted |= S_LRING;
            exercise(A_WIS, TRUE);
            newsym(currentX(), currentY());
        } else
            pline("Some dirty water backs up in the drain.");
        break;
    case 6:
        breaksink(currentX(), currentY());
        break;
    case 7:
        pline_The("water moves as though of its own will!");
        if ((mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE)
            || !makeMonsterOfType(PM_WATER_ELEMENTAL, currentX(), currentY(), NO_MM_FLAGS))
            pline("But it quiets down.");
        break;
    case 8:
        pline("Yuk, this water tastes awful.");
        more_experienced(1, 0);
        newexplevel();
        break;
    case 9:
        pline("Gaggg... this tastes like sewage!  You vomit.");
        morehungry(rn1(30 - ACURR(A_CON), 11));
        vomit();
        break;
    case 10:
        pline("This water contains toxic wastes!");
        if (!youAreUnchanging()) {
            You("undergo a freakish metamorphosis!");
            polyself(0);
        }
        break;
    /* more odd messages --JJB */
    case 11:
        You_hear("clanking from the pipes...");
        break;
    case 12:
        You_hear("snatches of song from among the sewers...");
        break;
    case 19:
        if (youAreHallucinating()) {
            pline("From the murky drain, a hand reaches up... --oops--");
            break;
        }
    default:
        You("take a sip of %s water.",
            rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot");
    }
}

/*fountain.c*/
